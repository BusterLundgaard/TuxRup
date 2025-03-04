#ifndef MODIFY_CALLBACK_H
#define MODIFY_CALLBACK_H

#include <gtk/gtk.h>

typedef struct {
    char* function_name;
    char* document_path;
    char* before_code;
    char* args_code;
    char* function_code;
    char* after_code;
    char* definitions_code;
} callback_code_information;

typedef struct {
    int number;
    bool is_function;
    char* return_type;
    char* args_types;
} reference_type;

callback_code_information* get_callback_code_information(void* callback, char* callback_name);

void modify_callback(GtkWidget* widget, char* callback_name, char* new_function_code);

void create_code_editing_menu(
    GtkWidget* widget, 
    char* callback_name, 
    callback_code_information* code_info
    );

#endif