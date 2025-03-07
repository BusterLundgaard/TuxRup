#ifndef FUNCTION_DISPATCHER_H
#define FUNCTION_DISPATCHER_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

void function_dispatcher(GtkWidget* widget, gpointer data);

#endif