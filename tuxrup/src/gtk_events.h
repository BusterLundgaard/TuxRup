#ifndef GTK_EVENTS_H
#define GTK_EVENTS_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif
#include "globals.h"
#include "callback_map.h"

void on_added_to_dom(GtkWidget* widget, gpointer data);

gulong
on_g_signal_connect_data(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags);

void on_gtk_window_present(GtkWindow *window);
void on_init();

void our_gtk_css_provider_load_from_file(GFile* file);

#endif