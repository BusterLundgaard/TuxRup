#include "modify_callback.h"
#include "globals.h"
#include "document_parsing.h"
#include "callback_map.h"
#include "function_dispatcher.h"
#include "pointer_name_conversion.h"
#include "util.h"
#include "reference_type.h"
#include "fix_function_body.h"
#include "callback_information.h"
#include <dlfcn.h>

typedef struct {
    GtkWidget* modified_widget;
    callback_code_information* code_info;
    char* callback_name;
    GtkWidget* modified_code_textview;
} code_editing_done_button_arguments;

void on_code_editing_done(GtkWidget* self, gpointer data){
    code_editing_done_button_arguments* args = (code_editing_done_button_arguments*)data;
    callback_code_information* code_info = args->code_info;

    char* modified_code = get_textview_text(args->modified_code_textview);

    char* temporary_document = g_strdup_printf(
        "%s \n void %s(%s){\n %s \n}", 
        code_info->before_code, 
        code_info->function_name,    
        code_info->args_code,
        modified_code
        );

    // get undefined identifiers by saving modified code in a new document and parsing:
    g_file_set_contents("temp.c", temporary_document, strlen(temporary_document), NULL);
    CXCursor c = get_root_cursor("temp.c");
    CXCursor c_func = get_function_cursor(c, code_info->function_name);
    CXCursor c_func_body = get_function_body_cursor(c_func);

    GHashTable* undefined_identifers = g_hash_table_new(g_str_hash, g_str_equal); // This needs to have a custom destroy function for the value!
    GHashTable* declarations = g_hash_table_new(g_str_hash, g_str_equal); // This does not, it only stores keys (works as a set), not values
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

    // // compile the shared library
    gpointer map_value = g_hash_table_lookup(widget_hashes, args->modified_widget);
    if(map_value==NULL){
        g_print("Could not find the widget we're trying to modify!\n");
        return;
    }
    guint widget_hash = *((guint*)map_value);
    guint callback_hash = widget_hash ^ hash_string(args->callback_name);
    char* shared_library_path = g_strdup_printf("./%u.so", callback_hash);
    int result = system(g_strdup_printf("gcc -g -shared $(pkg-config --cflags gtk4) %s -o %s $(pkg-config --libs gtk4)", final_document_path, shared_library_path));
    if(result != 0){
        g_print("Could not compile! Quitting modification process\n");
        return;
    }

    // update info in widget_callback_table
    callback_identifier cb_id = {args->modified_widget, args->callback_name};
    gpointer value = g_hash_table_lookup(widget_callback_table, &cb_id);
    if(value == NULL){
        g_print("Couldn't find the callback information for the widget you are modifying..\n");
        return;
    }
    callback_info* cb_info = (callback_info*)value;

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

    // load the shared library handle
    cb_info->dl_handle = dlopen(shared_library_path, RTLD_LAZY);

    // finally change the callback to the function dispatcher:
    remove_callback(args->modified_widget, args->callback_name);
    normal_g_signal_connect_data(args->modified_widget, args->callback_name, G_CALLBACK(function_dispatcher), args->callback_name, NULL, (GConnectFlags)0);

    // free memory that needs to be freed:
    g_hash_table_destroy(undefined_identifers);
    g_hash_table_destroy(declarations);
    free_callback_code_information(code_info);
    free(args);
    g_free(fixed_function_body);
    g_free(final_document);
    g_free(temporary_document);
}


void create_code_editing_menu(
    GtkWidget* widget, 
    char* callback_name, 
    callback_code_information* code_info
    ){

    #ifdef USE_GTK3
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_toplevel(widget));  // Get the parent window
    #else
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_native(widget));  // Get the parent window
    #endif
    
    GtkApplication *app = GTK_APPLICATION(gtk_window_get_application(parent_window)); // Get the application

    // Create a new window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Code Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 800);
    
    // Create a vertical box layout
    #ifdef USE_GTK3
    GtkAdjustment* v_adj = gtk_adjustment_new(0,0,0,0,0,0);
    GtkAdjustment* h_adj = gtk_adjustment_new(0,0,0,0,0,0);
    GtkWidget* scrolled_window = gtk_scrolled_window_new(v_adj, h_adj);
    #else 
    GtkWidget* scrolled_window = gtk_scrolled_window_new();
    #endif
    gtk_widget_set_size_request(scrolled_window, 380, 280);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *before_text = create_textview(code_info->before_code, FALSE);
    GtkWidget *var_text = create_textview("// Define your variables here\n", TRUE);
    GtkWidget *header_text = create_textview(g_strdup_printf("void %s(%s){", code_info->function_name, code_info->args_code) , FALSE);
    GtkWidget *code_text = create_textview(code_info->function_code, TRUE);
    GtkWidget *after_text = create_textview(g_strdup_printf("}\n %s", code_info->after_code), FALSE);
    GtkWidget* done_button = gtk_button_new_with_label("Done");
    
    #ifdef USE_GTK3 
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);
    gtk_container_add(GTK_CONTAINER(scrolled_window), vbox);
    gtk_container_add(GTK_CONTAINER(vbox), before_text);
    gtk_container_add(GTK_CONTAINER(vbox), var_text);
    gtk_container_add(GTK_CONTAINER(vbox), header_text);
    gtk_container_add(GTK_CONTAINER(vbox), code_text);
    gtk_container_add(GTK_CONTAINER(vbox), after_text);
    gtk_container_add(GTK_CONTAINER(vbox), done_button);
    #else 
    gtk_window_set_child(GTK_WINDOW(window), scrolled_window);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), vbox);
    gtk_box_append(GTK_BOX(vbox), before_text);
    gtk_box_append(GTK_BOX(vbox), var_text);
    gtk_box_append(GTK_BOX(vbox), header_text);
    gtk_box_append(GTK_BOX(vbox), code_text);
    gtk_box_append(GTK_BOX(vbox), after_text);
    gtk_box_append(GTK_BOX(vbox), done_button);
    #endif

    code_editing_done_button_arguments* code_editing_done_button_args = malloc(sizeof(code_editing_done_button_arguments));
    code_editing_done_button_args->modified_widget = widget;
    code_editing_done_button_args->code_info = code_info;
    code_editing_done_button_args->callback_name = callback_name;
    code_editing_done_button_args->modified_code_textview = code_text;
    normal_g_signal_connect_data(done_button, "clicked", G_CALLBACK(on_code_editing_done), code_editing_done_button_args, NULL, (GConnectFlags)0);

    // Display the window
    gtk_window_present(GTK_WINDOW(window));
}