#ifndef MODIFY_CALLBACK_H
#define MODIFY_CALLBACK_H

#include <gtk/gtk.h>

void modify_callback(void* callback, GtkWidget* widget, char* callback_name, char* new_function_code);

void simple_tests(void* callback, GtkWidget* widget, char* callback_name);

#endif