#ifndef CONTEXT_MENU_H
#define CONTEXT_MENU_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

void open_actions_menu();
int select_widget_for_action(GtkWidget* widget, GdkEventButton* event, void* user_data);

#endif
