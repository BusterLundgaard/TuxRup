#ifndef OVERRIDES_H
#define OVERRIDES_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif
#include "../globals.h"
#include "../callback_map.h"

// Initialization hooks
GtkApplication* gtk_application_new_OVERRIDE(const char* application_id, GApplicationFlags flags);
int g_application_run_OVERRIDE(GApplication* application, int argc, char** argv);
void gtk_window_present_OVERRIDE(GtkWindow *window);
void gtk_widget_show_all_OVERRIDE(GtkWidget* widget);

// CSS hooks
#ifdef USE_GTK3
gboolean gtk_css_provider_load_from_file_OVERRIDE(GtkCssProvider* provider, GFile* file, GError** err);
#else
void gtk_css_provider_load_from_file_OVERRIDE(GtkCssProvider* provider, GFile* file);
#endif

// Signal hooks
gulong g_signal_connect_data_OVERRIDE(gpointer instance,
                                          const gchar *detailed_signal,
                                          GCallback c_handler,
                                          gpointer data,
                                          GClosureNotify destroy_data,
                                          GConnectFlags connect_flags);
#endif
