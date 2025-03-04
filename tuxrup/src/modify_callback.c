#include "modify_callback.h"
#include "globals.h"
#include "document_parsing_and_generation.h"
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
    FILE* fp = popen(g_strdup_printf("ctags -R -o - %s | grep %s", program_src_folder, function_name), "r");

    char buffer[1024];
    if(fgets(buffer, sizeof(buffer), fp) == NULL){
        return NULL;
    }

    char document_path[1024];
    document_path[0]='\0';
    sscanf(buffer, "%*s %1023s", document_path);

    return g_strdup(document_path);
    //return g_strdup_printf("%s/%s", working_directory, document_path);
}

// Find and allocates callback code information on the heap 
callback_code_information* get_callback_code_information(void* callback){
    callback_code_information* info = malloc(sizeof(callback_code_information));

    // Call a libclang function here that writes to some buffers

}

// typedef struct {
//     char* function_name;
//     char* document_path;
//     char* before_code;
//     char* header_code;
//     char* function_code;
//     char* after_code;
// } callback_code_information;

void free_callback_code_information(callback_code_information* info){
    g_string_free(info->function_name, TRUE);
    g_string_free(info->document_path, TRUE);
    g_string_free(info->before_code, TRUE);
    g_string_free(info->args_code, TRUE);
    g_string_free(info->function_code, TRUE);
    g_string_free(info->after_code, TRUE);
}


void modify_callback(GtkWidget* widget, char* callback_name, char* new_function_code){
    // Look up the callback map to retrieve information
    
    // Get path and function name given the callback
    // char* function_name = get_identifier_from_pointer(callback);
    // char* document_path = get_document_path(function_name);
    
    // // Create a file with the modified document, and retrieve the identifiers the modified function will need
    // create_modified_function_document_data modified_data = create_modified_document_and_find_identifiers(document_path, function_name);
    
    // char* new_document_path = g_strdup_printf("%s_click_callback_modified.c", get_basename_without_extension(document_path));
    // g_file_set_contents(new_document_path, modified_data.buffer->str, modified_data.buffer->len, NULL);
    // g_string_free(modified_data.buffer, TRUE);
    
    // // Compile the file into a shared library whose name is the a hash of the widgets hash and the callback name
    // guint hash;
    // //hash = *((guint*)g_hash_table_lookup(widget_hashes, widget));
    // //hash ^= g_str_hash(callback_name);
    // hash = 378491;
    // char* new_shared_library_path = g_strdup_printf("%u.so", hash);
    // printf("shared library filename is: %s\n", new_shared_library_path);
    // int result = system(g_strdup_printf("gcc -g -shared $(pkg-config --cflags gtk4) %s -o %s $(pkg-config --libs gtk4)", new_document_path, new_shared_library_path));
    // g_print("result = %d\n", result);
    // if(result != 0){
    //     g_print("Could not compile! Quitting modification process\n");
    // }

    // Update the widget map with the new callback
    // callback_identifier cb_id = {widget, callback_name};
    // callback_info* cb = (callback_info*)g_hash_table_lookup(widget_callback_table, &cb_id);
    // if(cb == NULL){return;}

    // // .... Set modified function name and original pointer:
    // cb->original_function_pointer = (callback_type)callback;
    // cb->function_name = function_name;

    // // .... Set list of pointers to identifiers
    // cb->identifier_pointers_n = g_list_length(*(modified_data.required_identifiers));
    // cb->identifier_pointers = malloc((sizeof(void*))*(cb->identifier_pointers_n));
    // GList* iter;
    // int i = 0;
    // for(iter = *(modified_data.required_identifiers); iter; iter = iter->next, i++){
    //     (cb->identifier_pointers)[i] = get_pointer_from_identifier((char*)iter->data);
    // }
    // g_list_free_full(*(modified_data.required_identifiers), g_free);

    // // ... set shared library handle
    // cb->dl_handle = dlopen(new_shared_library_path, RTLD_LAZY);
    
    // // Finally, update the actual callback
    // remove_click_signal(widget);
    // normal_g_signal_connect_data(widget, callback_name, G_CALLBACK(function_dispatcher), callback_name, NULL, (GConnectFlags)0);
}

void on_code_editing_done(GtkWidget* self, gpointer data){
    // Get all the parameters you need here like code before and shit
    // Data will need to include the new code and the callback_name
}

GtkWidget* create_textview(const char* text, bool editable){
    GtkWidget *text_view = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, text, -1);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), editable);
    return text_view;
}

void create_code_editing_menu(
    GtkWidget* widget, 
    char* callback_name, 
    char* function_name,
    char* function_params,
    char* document_path,
    char* before_code, 
    char* function_code, 
    char* after_code){

    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_native(widget));  // Get the parent window
    GtkApplication *app = GTK_APPLICATION(gtk_window_get_application(parent_window)); // Get the application

    // Create a new window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Code Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 800);

    // Create a vertical box layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    // Create the code 
    GtkWidget *before_text = create_text_view(before_code, FALSE);
    gtk_box_append(GTK_BOX(vbox), before_text);

    GtkWidget *var_text = create_text_view("// Define your variables here\n", TRUE);
    gtk_box_append(GTK_BOX(vbox), var_text);

    GtkWidget *header_text = create_text_view(g_strdup_printf("void %s(%s){", function_name, function_params) , FALSE);
    gtk_box_append(GTK_BOX(vbox), header_text);

    GtkWidget *code_text = create_text_view(function_code, TRUE);
    gtk_box_append(GTK_BOX(vbox), code_text);

    GtkWidget *after_text = create_text_view(g_strdup_printf("}\n %s", after_text), FALSE);
    gtk_box_append(GTK_BOX(vbox), after_text);

    // Add done button
    GtkWidget* done_button = gtk_button_new_with_label("Done");
    gtk_box_append(GTK_BOX(vbox), done_button);
    normal_g_signal_connect_data(done_button, "clicked", G_CALLBACK(on_code_editing_done), callback_name, NULL, (GConnectFlags)0);

    // Display the window
    gtk_window_present(GTK_WINDOW(window));
}
