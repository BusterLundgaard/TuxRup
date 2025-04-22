#include "edit_callbacks_window.h"

#include "document_parsing.h"
#include "fix_function_body.h"
#include "reference_type.h"

#include "../globals.h"
#include "../callback_map.h"
#include "../function_dispatcher.h"

#include "../utilities/util.h"
#include "../utilities/hashing.h"
#include "../utilities/pointer_name_conversion.h"

#include "../method_intercepting/hooks.h"

#include <dlfcn.h>
// =====================================================
// GLOBALS AND TYPEDEFS
// =====================================================
static GtkWidget* active_widget = NULL;

typedef struct {
    enum gtk_callback_category callbacks[MAPPABLE_ACTIONS_LEN];
    int size;
} callbacks_list;

static callbacks_list applicable_callbacks;

// ======================================================
// GENERAL UTILITY METHODS
// ======================================================
// Gets all the callbacks that can possibly be applied to a given widget (looks at its categories and sees what callbacks we currently support editing for this widget category)
callbacks_list set_applicable_callbacks_from_active_widget(){
    enum widget_type_category active_widget_category = get_widget_type_category(active_widget);
    applicable_callbacks = (callbacks_list){.callbacks = {}, .size=0};
   
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
}

static void create_empty_callback_file(guint hash){
    copy_file("../templates/new_callback.c", g_strdup_printf("./runtime_files/%u.c", hash));
}

static FILE* open_file_vscode(char* path, int line){
    char* command = g_strdup_printf("nano %s", path);
    printf("opening with command %s\n", command); 
    system(command);
}

static void compile_callback_file(char* c_file_path, char* shared_library_path){
    // Determine the GTK version and corresponding compile command
    const char *gtk_pkg_config = NULL; 
    switch (gtk_ver) {
        case GTK_VERSION_4: gtk_pkg_config = "gtk4"; break;
        case GTK_VERSION_3: gtk_pkg_config = "gtk+-3.0"; break;
        case GTK_VERSION_2: gtk_pkg_config = "gtk+-2.0"; break;
        default:
            g_warning("Unknown GTK version: %d", gtk_ver);
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
    if(info->dl_handle != NULL){
        dlclose(info->dl_handle);
        info->dl_handle = NULL;
    }
    g_signal_connect_data_ORIGINAL(
        info->id->widget, 
        get_callback_type_category_str(info->id->callback),
        G_CALLBACK(function_dispatcher), 
        &(info->id->callback), 
        NULL, (GConnectFlags)0);
    info->dl_handle = dlopen(info->shared_library_path, RTLD_LAZY);
}

char* set_callback_code_information(callback_info* info){
    info->hash = compute_callback_hash(info->id->widget, info->id->callback);
    info->modified_code_path = g_strdup_printf("./runtime_files/%u.c", info->hash);
    info->shared_library_path = g_strdup_printf("./runtime_files/%u.so", info->hash);

    if(has_debug_symbols() || (debug_symbols_path != NULL)){
        info->function_name = get_identifier_from_pointer(info->original_function_pointer);
        g_print("set function name to %s\n", info->function_name);
    } else {
        printf("Not enough debug information in executable to be able to edit callbacks!\n");
        return NULL;
    }

    if(info->original_function_pointer != NULL){
        info->original_document_path = get_document_path(info->function_name);
        
        CXCursor c = get_root_cursor(info->original_document_path);
        CXCursor c_func = get_function_cursor(c, info->function_name);
        CXCursor c_func_body = get_function_body_cursor(c_func);
        
        info->original_before_code = g_string_new("");
        info->original_after_code = g_string_new("");
        info->original_function_location = 0;
        info->original_definitions_code = g_string_new("");
        info->original_function_args = g_string_new("");
        info->original_function_code = g_string_new("");
        set_before_after_code_args args = {
            .before_code=info->original_before_code, 
            .after_code=info->original_after_code, 
            .modified_function_location=clang_getCursorLocation(c_func), 
            .line=&info->original_function_location};
        clang_visitChildren(c, set_before_after_code, &args);
        clang_visitChildren(c, set_definitions_code, info->original_definitions_code);
        clang_visitChildren(c_func_body, write_cursor_to_buffer, info->original_function_code);
        set_function_arguments(c_func, info->original_function_args);
    }
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
    g_string_append(editing_document, g_strdup_printf("\nvoid %s(%s){\n", info->function_name, info->original_function_args->str)); 
    
    if(edited_before){
        FILE* modified_code_file = fopen(info->modified_code_path, "r");
        char line[1024];
        while(fgets(line, sizeof(line), modified_code_file)){
            g_string_append(editing_document, line);
        }
    } else {
        g_string_append(editing_document, info->original_function_code->str);
    }

    g_string_append(editing_document, "}\n\n");
    g_string_append(editing_document, info->original_after_code->str);
    g_file_set_contents(output_path, editing_document->str, strlen(editing_document->str), NULL);      
}


static void on_edit_callback_button(GtkWidget* widget, gpointer data){
    enum gtk_callback_category callback = *(enum gtk_callback_category*)data;
    
    // If creating completely new callback: 
    if(!callback_map_exists(active_widget, callback)){
        callback_info* info = callback_map_add_new(active_widget, callback);
        set_callback_code_information(info);
        copy_file("../tempplates/empty_modified_callback.c", info->modified_code_path);
        open_file_vscode(info->modified_code_path, 5);
        return;
    }

    callback_info* info = callback_map_get(active_widget, callback);
    set_callback_code_information(info);

    g_print("you are editing a callback with the pointer %p\n", info->original_function_pointer);
    g_print("name of function at this pointer is %s \n", info->function_name);

    if(info->original_function_pointer != NULL){
        bool has_been_modified = (info->dl_handle != NULL);
        create_version_of_document_for_code_editing(info, has_been_modified, "./runtime_files/edit.c");
        open_file_vscode("./runtime_files/edit.c", info->original_function_location);
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
    CXCursor c = get_root_cursor(info->original_document_path);
    CXCursor c_func = get_function_cursor(c, info->function_name);
    CXCursor c_func_body = get_function_body_cursor(c_func);

    GHashTable* undefined_identifers = g_hash_table_new(g_str_hash, g_str_equal); //[MEMLEAK] This needs to have a custom destroy function for the value!
    GHashTable* declarations = g_hash_table_new(g_str_hash, g_str_equal); 
    find_undefined_references_args set_undefined_args = {undefined_identifers, declarations};
    clang_visitChildren(c_func_body, set_undefined_references, &set_undefined_args);

    return undefined_identifers;
}

//[MEMLEAK]
static void isolate_modified_function(callback_info* info, char* modified_code, GHashTable* undefined_identifiers, char* output_path){
    char* fixed_function_body = create_fixed_function_body(modified_code, undefined_identifiers);
    char* final_document = g_strdup_printf(
        "%s \n void %s(%s){\n %s \n}", 
        info->original_definitions_code->str, 
        "customfunction",    
        info->original_function_args->str,
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
static void set_undefined_identifiers_on_info_map(callback_info* info, GHashTable* undefined_identifiers){
    info->identifier_pointers_n = g_hash_table_size(undefined_identifiers);
    info->identifier_pointers = malloc(sizeof(void*) * (info->identifier_pointers_n));
    iterate_hash_table(undefined_identifiers, set_undefined_identifiers_iter, info);
}

static void on_edit_callback_done_button(GtkWidget* widget, gpointer data){
    enum gtk_callback_category callback = *(enum gtk_callback_category*)data;
    callback_info* info = callback_map_get(active_widget, callback);

    if(info->original_function_pointer == NULL){ 
        // This callback is entirely new and created by the user
        compile_callback_file(info->modified_code_path, info->shared_library_path);
        set_widget_callback(info);
    } else { 
        // This is an original callback the user has edited
        // ... Get the modified code from the document the user is editing, save it into modified_code_path file
        GString* function_code = get_function_code("./runtime_files/edit.c", info->function_name);
        g_file_set_contents(info->modified_code_path, function_code->str, strlen(function_code->str), NULL);

        // ... Get the undefined identifiers the user's modified function will need
        GHashTable* undefined_identifiers = get_undefined_identifiers(info, function_code->str);
        set_undefined_identifiers_on_info_map(info, undefined_identifiers);

        // ... Construct the shared library by fixing the modified function with required pointers
        isolate_modified_function(info, function_code->str, undefined_identifiers, "./temp.c");
        compile_callback_file("./temp.c", info->shared_library_path);
        //remove("./temp.c");

        // ... Update the widget callback
        set_widget_callback(info);
    }
}


// ===========================================================================
// BUILDING THE WINDOW
// ===========================================================================
GtkWidget* create_edit_callback_field(enum gtk_callback_category* callback){
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);;
    gtk_widget_set_halign(hbox, GTK_ALIGN_END);

    GtkWidget* edit_button = gtk_button_new_with_label(callback_map_exists(active_widget, *callback) ? "Edit" : "Create");
    g_signal_connect_data_ORIGINAL(edit_button, "clicked", G_CALLBACK(on_edit_callback_button), callback, NULL, (GConnectFlags)0);

    GtkWidget* edit_done_button = gtk_button_new_with_label("done editing (but don't click before you idiot!)");
    g_signal_connect_data_ORIGINAL(edit_done_button, "clicked", G_CALLBACK(on_edit_callback_done_button), callback, NULL, (GConnectFlags)0);

    add_widget_to_box(hbox, edit_button);
    add_widget_to_box(hbox, edit_done_button);
    
    GtkWidget* label = gtk_label_new(get_callback_type_category_str(*callback));
    add_widget_to_box(hbox, label);

    return hbox;
}


void set_source_code_location(GtkEntry* entry, gpointer user_data){
    GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
    const char* current_text = gtk_entry_buffer_get_text(buffer);
    program_src_folder = current_text;
}
void set_debug_location(GtkEntry* entry, gpointer user_data){
    GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
    const char* current_text = gtk_entry_buffer_get_text(buffer);
    debug_symbols_path = current_text;
}

void do_my_dumb_test(
    GtkWidget *widget,
    gpointer   data){
        g_print("pointer to button_A_callback is %p\n", get_pointer_from_identifier("button_A_callback"));
    }

void build_edit_callbacks_window(GtkWidget* window){
    GtkWidget* vbox = create_and_add_scrollable_item_list(window, 200, 750);
    set_applicable_callbacks_from_active_widget();

    for(int i = 0; i < applicable_callbacks.size; i++){
        GtkWidget* edit_callback_field = create_edit_callback_field(&applicable_callbacks.callbacks[i]);
        gtk_box_append(GTK_BOX(vbox), edit_callback_field);
    }

    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(hbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(hbox, GTK_ALIGN_CENTER);
    GtkWidget* source_code_location_entry = gtk_entry_new();
    GtkWidget* source_code_label = gtk_label_new("source code location");
    g_signal_connect_data_ORIGINAL(source_code_location_entry, "changed", G_CALLBACK(set_source_code_location), NULL, NULL, (GConnectFlags)0);
    gtk_box_append(GTK_BOX(hbox), source_code_label);
    gtk_box_append(GTK_BOX(hbox), source_code_location_entry);
    gtk_box_append(GTK_BOX(vbox), hbox);

    GtkWidget* hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(hbox2, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(hbox2, GTK_ALIGN_CENTER);
    GtkWidget* debug_location_entry = gtk_entry_new();
    GtkWidget* debug_label = gtk_label_new("debug symbols location");
    g_signal_connect_data_ORIGINAL(debug_location_entry, "changed", G_CALLBACK(set_debug_location), NULL, NULL, (GConnectFlags)0);
    gtk_box_append(GTK_BOX(hbox2), debug_label);
    gtk_box_append(GTK_BOX(hbox2), debug_location_entry);
    gtk_box_append(GTK_BOX(vbox), hbox2);

    GtkWidget* dumb_test = gtk_button_new_with_label("fuck off");
    g_signal_connect_data_ORIGINAL(dumb_test, "clicked", G_CALLBACK(do_my_dumb_test), NULL, NULL, (GConnectFlags)0);
    gtk_box_append(GTK_BOX(vbox), dumb_test);
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
   
    g_signal_connect_data_ORIGINAL(window, "destroy", G_CALLBACK(cleanup_callback_editor_window), NULL, NULL, (GConnectFlags)0);
    gtk_window_present(GTK_WINDOW(window));
}