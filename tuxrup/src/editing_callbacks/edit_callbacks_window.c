#include <dlfcn.h>

#include "edit_callbacks_window.h"
#include "globals.h"
#include "util.h"
#include "callback_map.h"
#include "hashing.h"
#include "document_parsing.h"
#include "fix_function_body.h"
#include "function_dispatcher.h"
#include "reference_type.h"

// =====================================================
// GLOBALS AND TYPEDEFS
// =====================================================
static GtkWidget* active_widget = NULL;

typedef struct {
    enum gtk_callback_category callbacks[MAPPABLE_ACTIONS_LEN];
    int size;
} callbacks_list;


// ======================================================
// GENERAL UTILITY METHODS
// ======================================================
// Gets all the callbacks that can possibly be applied to a given widget (looks at its categories and sees what callbacks we currently support editing for this widget category)
callbacks_list get_applicable_callbacks_from_active_widget(){
    enum widget_type_category active_widget_category = get_widget_type_category(active_widget);
    callbacks_list applicable_callbacks = {.callbacks={}, .size=0}; 
   
    for(int i = 0; i < MAPPABLE_ACTIONS_LEN; i++){
        int j = 0;
        while(remapable_events[i].valid_widget_types[j] != GTK_CATEGORY_UNDEFINED){
            if(remapable_events[i].valid_widget_types[j] == active_widget_category){
                applicable_callbacks.callbacks[applicable_callbacks.size] = remapable_events[i].action_name;
                applicable_callbacks.size++;
                break;
            }
            j++;
        }
    }
    return applicable_callbacks;
}

static void create_empty_callback_file(guint hash){
    copy_file("../templates/new_callback.c", g_strdup_printf("./runtime_files/%u.c", hash));
}

static FILE* open_file_vscode(char* path, int line){
    system(g_strdup_printf("code -g %s:%d", path, line));
    return fopen(path, "r");
}

static void compile_callback_file(char* c_file_path, char* shared_library_path){
    // Determine the GTK version and corresponding compile command
    const char *gtk_pkg_config = NULL; 
    switch (gtk_version) {
        case GTK_VERSION_4: gtk_pkg_config = "gtk4"; break;
        case GTK_VERSION_3: gtk_pkg_config = "gtk+-3.0"; break;
        case GTK_VERSION_2: gtk_pkg_config = "gtk+-2.0"; break;
        default:
            g_warning("Unknown GTK version: %d", gtk_version);
            return;
    }

    // Build the compilation command
    char *compile_command = g_strdup_printf(
        "gcc %s -fPIC -shared -o %s `pkg-config --cflags --libs %s`", 
        c_file_path, shared_library_path, gtk_pkg_config
    );

    // Run the compilation command
    int ret = system(compile_command);
    if (ret != 0) {
        g_warning("Failed to compile %s\n", shared_library_path);
    }

    // Clean up
    g_free(compile_command);
}


// ========================================================
// UPDATING AND SETTING CALLBACK INFORMATION
// =======================================================
static void set_widget_callback(callback_info* info){
    remove_callback(info->id->widget, info->id->callback);
    normal_g_signal_connect_data(info->id->widget, info->id->callback, G_CALLBACK(function_dispatcher), NULL, NULL, (GConnectFlags)0); 
    callback_info* info = callback_map_get(info->id->widget, info->id->callback);
    info->dl_handle = dlopen(info->shared_library_path, RTLD_LAZY);
}

char* set_callback_code_information(callback_info* info){
    *(info->original_document_path) = get_document_path(info->function_name);

    CXCursor c = get_root_cursor(info->original_document_path);
    CXCursor c_func = get_function_cursor(c, info->function_name);
    CXCursor c_func_body = get_function_body_cursor(c_func);
    
    set_before_after_code_args args = {.before_code=info->original_before_code, .after_code=info->original_after_code, .modified_function_location=&c_func, .line=&info->original_function_location};
    clang_visitChildren(c, set_before_after_code, &args);
    clang_visitChildren(c, set_definitions_code, info->original_definitions_code);
    set_function_arguments(c_func, info->original_function_args);

    info->hash = compute_callback_hash(info->id->widget, info->id->callback);
    info->modified_code_path = g_strdup_printf("./runtime_files/%u.c", info->hash);
    info->shared_library_path = g_strdup_printf("./runtime_files/%u.so", info->hash);
}


// ==============================================================
// ON EDITING CALLBACKS
// ==============================================================

// Create a copy of the document containing the function you wish to edit
// Insert a few comments before the function
// If function has been edited before, then also insert the edits the user made 
int create_version_of_document_for_code_editing(callback_info* info, bool edited_before, char* output_path){
    char* before_code = info->original_before_code->str;
    GString* editing_document = g_string_new(g_strdup(before_code));

    g_string_append(editing_document, "\n //Edit the function inside of here. Don't edit anything else! \n");
    g_string_append(editing_document, g_strdup_printf("\nvoid %s(%s){\n", info->function_name, info->original_function_args)); 
    
    if(edited_before){
        FILE* modified_code_file = fopen(info->modified_code_path, 'r');
        char line[1024];
        while(fgets(line, sizeof(line), modified_code_file)){
            g_string_append(editing_document, line);
        }
    } else {
        g_string_append(editing_document, info->original_function_code->str);
    }

    g_string_append(editing_document, "}\n\n");
    g_string_append(editing_document, info->original_after_code->str);
    g_file_set_contents(output_path, editing_document->str, strlen(info->original_before_code->str), NULL);      
}

static void on_edit_callback_button(GtkWidget *widget, gpointer data){
    enum gtk_callback_category callback = *(enum gtk_callback_category*)data;
    callback_info* info = callback_map_get(widget, callback);
    set_callback_code_information(info);

    if(callback_map_exists(widget, callback)){
        copy_file("../templates/new_callback.c", info->modified_code_path);
        callback_map_add_new(widget, callback);
        open_callback_file_vscode(info->modified_code_path, 5);
    } 
    else if(info->original_function_pointer != NULL ){
        bool has_been_modified = (info->dl_handle != NULL);
        create_version_of_document_for_code_editing(info, has_been_modified, "../runtime_files/edit.c");
        open_file_vscode(info->modified_code_path, info->original_function_location);
    } 
    else {
        open_file_vscode(info->modified_code_path, 5);
    }
}


// ==============================================================
// ON FINALIZING THE EDITED CALLBACK / EDITING DONE
// ==============================================================

//[MEMLEAK]
GString* get_function_code(char* document_path, char* function_name){
    GString* code_buffer = g_string_new("");
    clang_visitChildren(  
        get_function_body_cursor(get_function_cursor(get_root_cursor(document_path), function_name)),
        write_cursor_to_buffer,
        code_buffer
    );
    return code_buffer;
}

//[MEMLEAK]
static GHashTable* get_undefined_identifiers(callback_info* info, char* function_code){
    char* temporary_document = g_strdup_printf(
        "%s \n void %s(%s){\n %s \n}", 
        info->original_definitions_code, 
        info->function_name,    
        info->original_function_args,
        function_code
        );

    g_file_set_contents("temp.c", temporary_document, strlen(temporary_document), NULL);
    CXCursor c = get_root_cursor("temp.c");
    CXCursor c_func = get_function_cursor(c, info->function_name);
    CXCursor c_func_body = get_function_body_cursor(c_func);

    GHashTable* undefined_identifers = g_hash_table_new(g_str_hash, g_str_equal); //[MEMLEAK] This needs to have a custom destroy function for the value!
    GHashTable* declarations = g_hash_table_new(g_str_hash, g_str_equal); 
    find_undefined_references_args set_undefined_args = {undefined_identifers, declarations};
    clang_visitChildren(c_func_body, set_undefined_references, &set_undefined_args);

    remove("temp.c");
    return undefined_identifers;
}

//[MEMLEAK]
static void isolate_modified_function(callback_info* info, char* modified_code, GHashTable* undefined_identifiers, char* output_path){
    char* fixed_function_body = create_fixed_function_body(modified_code, undefined_identifiers);
    char* final_document = g_strdup_printf(
        "%s \n void %s(%s){\n %s \n}", 
        info->original_definitions_code, 
        info->function_name,    
        info->original_function_args,
        fixed_function_body
    );
    g_file_set_contents(output_path, final_document, strlen(final_document), NULL);
}

static void set_undefined_identifiers_iter(gpointer key, gpointer value, int i, gpointer user_data){
    char* undefined_identifier = (char*)key;
    reference_type* ident_info = (reference_type*)value;
    callback_info* info = (callback_info*)user_data;
    info->identifier_pointers[ident_info->number] = get_pointer_from_identifier(undefined_identifier);
}
static void set_undefined_identifiers(callback_info* info, GHashTable* undefined_identifiers){
    info->identifier_pointers_n = g_hash_table_size(undefined_identifiers);
    info->identifier_pointers = malloc(sizeof(void*)*info->identifier_pointers_n);
    iterate_hash_table(undefined_identifiers, set_undefined_identifiers_iter);
}

static void on_edit_callback_done_button(GtkWidget* widget, gpointer data){
    enum gtk_callback_category callback = *(enum gtk_callback_category*)data;
    callback_info* info = callback_map_get(widget, callback);

    if(info->original_function_pointer == NULL){ 
        // This callback is entirely new and created by the user
        compile_callback_file(info->modified_code_path, info->shared_library_path);
        set_widget_callback(info);
    } else { 
        // This is an original callback the user has edited
        // ... Get the modified code from the document the user is editing, save it into modified_code_path file
        GString* function_code = get_function_code("./runtime_files/editing.c", info->function_name);
        g_file_set_contents(info->modified_code_path, function_code->str, sizeof(function_code->str), NULL);

        // ... Get the undefined identifiers the user's modified function will need
        GHashTable* undefined_identifiers = get_undefined_identifiers(info, function_code->str);
        set_undefined_identifiers(info, undefined_identifiers);

        // ... Construct the shared library by fixing the modified function with required pointers
        isolate_modified_function(info, function_code, undefined_identifiers, "./temp.c");
        compile_callback_file("./temp.c", info->shared_library_path);
        remove("./temp.c");

        // ... Update the widget callback
        set_widget_callback(info);
    }
}


// ===========================================================================
// BUILDING THE WINDOW
// ===========================================================================
GtkWidget* create_edit_callback_field(enum gtk_callback_category callback){
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);;
    gtk_widget_set_halign(hbox, GTK_ALIGN_END);

    GtkWidget* edit_button = gtk_button_new_with_label(callback_exists(active_widget, callback) ? "Edit" : "Create");
    normal_g_signal_connect_data(edit_button, "clicked", on_edit_callback_button, callback, NULL, (GConnectFlags)0);
    add_widget_to_box(hbox, edit_button);
    
    GtkWidget* label = gtk_label_new(get_callback_type_category_str(callback));
    add_widget_to_box(hbox, label);

    return hbox;
}


void build_edit_callbacks_window(GtkWidget* window){
    GtkWidget* vbox = create_and_add_scrollable_item_list(window, 200, 750);

    callbacks_list applicable_callbacks = get_applicable_callbacks_from_active_widget();
    for(int i = 0; i < applicable_callbacks.size; i++){
        GtkWidget* edit_callback_field = create_edit_callback_field(applicable_callbacks.callbacks[i]);
        gtk_box_append(GTK_BOX(vbox), edit_callback_field);
    }
}


// ======================================================================================
// OPEN / CLOSE
// ======================================================================================
gboolean cleanup_callback_editor_window(GtkWidget* closed_window, gpointer user_data){
    // Cleanup here
}

void open_edit_callbacks_window(GtkWidget* widget) {
    active_widget = widget;   
    GtkWidget* window = create_window(widget, "Edit widget properties", 200, 800);
    
    build_edit_callbacks_window(window); 
   
    normal_g_signal_connect_data(window, "destroy", G_CALLBACK(cleanup_callback_editor_window), NULL, NULL, (GConnectFlags)0);
    gtk_window_present(GTK_WINDOW(window));
}