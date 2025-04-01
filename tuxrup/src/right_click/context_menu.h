#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

void open_right_click_context_menu(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);