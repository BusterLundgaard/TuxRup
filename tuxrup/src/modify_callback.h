#ifndef MODIFY_CALLBACK_H
#define MODIFY_CALLBACK_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include "callback_information.h"

callback_code_information* get_callback_code_information(void* callback, char* callback_name);

void create_code_editing_menu(
    GtkWidget* widget, 
    char* callback_name, 
    callback_code_information* code_info
    );

#endif