#include <dlfcn.h>

#include "edit_callbacks_window.h"
#include "globals.h"
#include "util.h"
#include "callback_map.h"
#include "hashing.h"
#include "document_parsing.h"

static GtkWidget* active_widget = NULL;

gboolean cleanup_callback_editor_window(GtkWidget* closed_window, gpointer user_data){
    // Cleanup here
}

typedef struct {
    enum gtk_callback_category callbacks[MAPPABLE_ACTIONS_LEN];
    int size;
} callbacks_list;

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
            g_free(file_path);
            return;
    }

    // Build the compilation command
    char *compile_command = g_strdup_printf(
        "gcc %s -fPIC -shared -o %s `pkg-config --cflags --libs %s`", 
        c_file_path, shared_library_path, gtk_pkg_config
    );

    // Run the compilation command
    int ret = system(compile_command);
    if (ret == 0) {
        g_print("Successfully compiled %s\n", file_path);
    } else {
        g_warning("Failed to compile %s\n", file_path);
    }

    // Clean up
    g_free(compile_command);
    g_free(file_path);
}

static void set_widget_callback(GtkWidget* widget, enum gtk_callback_category callback, char* shared_library_path){
    remove_callback(widget, callback);
    callback_info* info = callback_map_get(widget, callback);
    
    info->dl_handle = dlopen(shared_library_path, RTLD_LAZY); 
}

int create_version_of_document_for_code_editing(callback_info info, bool edited_before, char* output_path);
    GString* editing_document = g_string_new(g_strdup(info->before_code->str));

    g_string_append(editing_document, "\n //Edit the function inside of here. Don't edit anything else! \n")
    g_string_append(editing_document, g_strdup_printf("\nvoid %s(%s){\n", info->function_name, info->original_function_args)); 
    
    if(edited_before){
        FILE* modified_code_file = fopen(g_strdup_printf("./runtime_files/%u.c", info->hash))
        char line[1024];
        while(fgets(line, sizeof(line), before_buffer)){
            g_string_append(editing_document, line);
        }
    } else {
        g_string_append(editing_document, info->orignal_function_code->str);
    }

    g_string_append(editing_document, "}\n\n");
    g_string_append(editing_document, info->after_code->str);
    g_file_set_contents(output_path, editing_document->str, strlen(before_buffer->str), NULL);      
}

static void on_edit_callback_button(GtkWidget *widget, gpointer data){
    enum gtk_callback_category callback = *(enum gtk_callback_category*)data;
    guint hash = compute_calback_hash(widget, callback);
    char* modified_code_location = g_strdup_printf("./runtime_files/%u.c", hash);

    bool exists = callback_map_exists(widget, callback);
    if(!exists){
        copy_file("../templates/new_callback.c", modified_code_location);
        callback_map_add_new(widget, callback);
        open_callback_file_vscode(modified_code_location, 5);
        return;
    }
   
    callback_info* info = callback_map_get(widget, callback);
    if(info->original_function_pointer != NULL ){
        bool has_been_modified = (info->dl_handle != NULL);
        insert_modified_function_into_document_and_get_location(info, has_been_modified, editing_document_path);
        open_file_vscode(editing_document_path, info->original_funtion_location);
        return; 
    } 
    else {
        open_file_vscode(modified_code_location, 5);
    }
}


static void compile_and_set_callback_function_original(GtkWidget* widget, enum gtk_callback_category callback){
    callback_info* info = callback_map_get(widget, callback);
    guint hash = compute_calback_hash(widget, callback);

    char* modified_code_path = g_strdup_printf("./runtime_files/%u.c", hash);
    char* compiled_library_path = g_strdup_printf("./runtime_files/%u.so", hash);
    compile_callback_file(modified_code_path, compiled_library_path);
    info->dl_handle = dlopen(compiled_library_path, RTLD_LAZY);
    normal_g_signal_connect_data(widget, get_callback_type_category_str(callback), G_CALLBACK(function_dispatcher), args->callback_name, NULL, (GConnectFlags)0);
}

static void compile_and_set_callback_function_modified(GtkWidget* widget, gpointer data) {
    callback_info* info = callback_map_get(widget, callback);
    guint hash = compute_calback_hash(widget, callback);

    char* editing_document_path = "./runtime_files/copied_document_with_function_you_want_to_edit.c";
    char* original_document_path = get_document_path(info->function_name);

    char* modified_code = get_function_code(editing_document_path, info->function_name);    

    // get undefined identifiers by saving modified code in a new document and parsing:
    // ahhhhhh i am so dead and we need to make sure it gets the code before *without* functions and variables
    char* temporary_document = g_strdup_printf(
        "%s \n void %s(%s){\n %s \n}", 
        code_info->before_code_removed_symbols, 
        code_info->function_name,    
        code_info->args_code,
        modified_code
        );

    g_file_set_contents("temp.c", temporary_document, strlen(temporary_document), NULL);
    CXCursor c = get_root_cursor("temp.c");
    CXCursor c_func = get_function_cursor(c, code_info->function_name);
    CXCursor c_func_body = get_function_body_cursor(c_func);

    GHashTable* undefined_identifers = g_hash_table_new(g_str_hash, g_str_equal); //[MEMLEAK] This needs to have a custom destroy function for the value!
    GHashTable* declarations = g_hash_table_new(g_str_hash, g_str_equal); 
    find_undefined_references_args set_undefined_args = {undefined_identifers, declarations};
    clang_visitChildren(c_func_body, set_undefined_references, &set_undefined_args);

    // use these undefined to construct the new body code with prefix and postfix, and create the final document:
    char* fixed_function_body = create_fixed_function_body(modified_code, undefined_identifers);
    char* final_document = g_strdup_printf(
        "%s \n void %s(%s){\n %s \n}", 
        code_info->definitions_code, 
        code_info->function_name,    
        code_info->args_code,
        fixed_function_body
    );
    char* final_document_path = g_strdup_printf("%s_callback.c", get_basename_without_extension(code_info->document_path));
    g_file_set_contents(final_document_path, final_document, strlen(final_document), NULL);

    // compile the shared library
    char* shared_library_path = g_strdup_printf("./%u.so", hash);
    compile_callback_file(final_document_path, shared_library_path);

    // update info in widget_callback_table
    set_widget_callback(widget, callback, shared_library_path)
    cb_info->identifier_pointers_n = g_hash_table_size(undefined_identifers);
    cb_info->identifier_pointers = malloc(sizeof(void*)*cb_info->identifier_pointers_n);
    
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, undefined_identifers);
    gpointer key;

    int i = 0;
    for(; g_hash_table_iter_next(&iter, &key, &value); i++){
        char* undefined_identifier = (char*)key;
        reference_type* ident_info = (reference_type*)value;
        g_print("setting identifier_pointers[%d] = the pointer to %s\n", ident_info->number, undefined_identifier);
        cb_info->identifier_pointers[ident_info->number] = get_pointer_from_identifier(undefined_identifier);
    }

    // free memory that needs to be freed:
    g_hash_table_destroy(undefined_identifers);
    g_hash_table_destroy(declarations);
    free_callback_code_information(code_info);
    free(args);
    g_free(fixed_function_body);
    g_free(final_document);
    g_free(temporary_document);
    // you also need to remove some files here ya dumbass!
}

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

void open_edit_callbacks_window(GtkWidget* widget) {
    //Create window with item list
    GtkWidget* window = create_window(widget, "Edit widget properties", 200, 800);
    GtkWidget* vbox = create_and_add_scrollable_item_list(window,200, 750);

    // Set active widget:
    active_widget = widget;   

    // Actually build the window contents
    callbacks_list applicable_callbacks = get_applicable_callbacks_from_active_widget();
    for(int i = 0; i < applicable_callbacks.size; i++){
        GtkWidget* edit_callback_field = create_edit_callback_field(applicable_callbacks.callbacks[i]);
        gtk_box_append(GTK_BOX(vbox), edit_callback_field);
    }

    // Add close callback and present
    normal_g_signal_connect_data(window, "destroy", G_CALLBACK(cleanup_callback_editor_window), NULL, NULL, (GConnectFlags)0);
    gtk_window_present(GTK_WINDOW(window));
}