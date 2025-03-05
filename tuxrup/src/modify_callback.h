#ifndef MODIFY_CALLBACK_H
#define MODIFY_CALLBACK_H

#include <gtk/gtk.h>
#include "callback_information.h"

callback_code_information* get_callback_code_information(void* callback, char* callback_name);

void create_code_editing_menu(
    GtkWidget* widget, 
    char* callback_name, 
    callback_code_information* code_info
    );

#endif