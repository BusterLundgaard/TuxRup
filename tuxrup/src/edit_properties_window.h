#ifndef EDIT_PROPERTIES_WINDOW_H
#define EDIT_PROPERTIES_WINDOW_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

void open_edit_properties_window(GtkWidget* widget);

#endif