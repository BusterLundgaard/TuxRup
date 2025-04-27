#ifndef CONTEXT_MENU_H
#define CONTEXT_MENU_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#ifdef USE_GTK3
gboolean open_right_click_context_menu(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
#else
gboolean open_right_click_context_menu(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);
#endif

#endif
