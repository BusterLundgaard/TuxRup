#include "../globals.h"

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

enum gtk_callback_category get_callback_category_from_connect_signal(gpointer instance, const gchar* detailed_signal);
GtkWidget* get_widget_from_connect_signal(gpointer instance);
bool widget_seen_before(GtkWidget* widget);