#ifndef GTK_EVENTS_H
#define GTK_EVENTS_H

#include <gtk/gtk.h>
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
void on_init(GtkApplication* app);

#endif