#ifndef MODIFY_CALLBACK_H
#define MODIFY_CALLBACK_H

#include <gtk/gtk.h>

typedef struct {
    GString* function_name;
    GString* document_path;
    GString* before_code;
    GString* args_code;
    GString* function_code;
    GString* after_code;
} callback_code_information;

void modify_callback(GtkWidget* widget, char* callback_name, char* new_function_code);

void simple_tests(void* callback, GtkWidget* widget, char* callback_name);

#endif