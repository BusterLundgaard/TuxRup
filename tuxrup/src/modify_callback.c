#include "modify_callback.h"
#include "globals.h"
#include "modified_document_generation.h"
#include "callback_map.h"
#include "function_dispatcher.h"
#include "pointer_name_conversion.h"
#include "util.h"

void remove_click_signal (GtkWidget* w){
    g_signal_handlers_disconnect_matched(
        w, 
        G_SIGNAL_MATCH_ID, 
        g_signal_lookup("clicked", G_OBJECT_TYPE(w)), 
        0, 
        NULL, 
        NULL, 
        NULL);  
}

char* get_document_path(char* function_name){
    return "";
}

void modify_callback(void* callback, GtkWidget* widget, char* callback_name, char* new_function_code){
    // Get path and function name given the callback
    char* function_name = get_identifier_from_pointer(callback);
    char* document_path = get_document_path(function_name);
    char* full_path = g_strdup_printf("%s/%s", program_src_folder, document_path);    
    
    // Create a file with the modified document, and retrieve the identifiers the modified function will need
    create_modified_function_document_data modified_data = create_modified_document_and_find_identifiers(full_path, function_name);
    char* new_document_path = g_strdup_printf("%s_click_callback_modified.c", get_basename_without_extension(full_path));
    g_file_set_contents(new_document_path, modified_data.buffer->str, modified_data.buffer->len, NULL);
    g_string_free(modified_data.buffer, TRUE);
    
    // Compile the file into a shared library whose name is the a hash of the widgets hash and the callback name
    guint hash = *((guint*)g_hash_table_lookup(widget_hashes, widget));
    hash ^= g_str_hash(callback_name);
    char* new_shared_library_path = g_strdup_printf("%u.so", hash);
    int result = system(g_strdup_printf("gcc -g $(shell pkg-config --cflags gtk4) %s -o %s $(shell pkg-config --libs gtk4)", new_document_path, new_shared_library_path));

    // Update the widget map with the new callback
    callback_identifier cb_id = {widget, callback_name};
    callback_info* cb = (callback_info*)g_hash_table_lookup(widget_callback_table, &cb_id);
    if(cb == NULL){return;}

    // .... Set modified function name and original pointer:
    cb->original_function_pointer = (callback_type)callback;
    cb->function_name = function_name;

    // .... Set list of pointers to identifiers
    cb->identifier_pointers_n = g_list_length(*(modified_data.required_identifiers));
    cb->identifier_pointers = malloc((sizeof(void*))*(cb->identifier_pointers_n));
    GList* iter;
    int i = 0;
    for(iter = *(modified_data.required_identifiers); iter; iter = iter->next, i++){
        (cb->identifier_pointers)[i] = get_pointer_from_identifier((char*)iter->data);
    }
    g_list_free_full(*(modified_data.required_identifiers), g_free);

    // ... set shared library handle
    cb->dl_handle = dlopen(new_shared_library_path, RTLD_LAZY);
    
    // Finally, update the actual callback
    remove_click_signal(widget);
    normal_g_signal_connect_data(widget, callback_name, G_CALLBACK(function_dispatcher), callback_name, NULL, (GConnectFlags)0);
}