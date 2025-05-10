#ifndef EDIT_CALLBACKS_WINDOW_H
#define EDIT_CALLBACKS_WINDOW_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

void open_edit_callbacks_window(GtkWidget* widget);
void testing_create_isolated_function();

void on_edit_callback_button(GtkWidget* widget, gpointer data);
void on_edit_callback_done_button(GtkWidget* widget, gpointer data);
#endif
