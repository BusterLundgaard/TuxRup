#ifndef TOOLBAR_H
#define TOOLBAR_H
#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else
    #include <gtk-4.0/gtk/gtk.h>
#endif

GMenu* create_toolbar(GApplication* app);
void enable_toolbar(GtkApplication* app, GtkWindow* win, GMenu* menu_bar);

#endif
