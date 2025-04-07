#ifndef HOOKS_H
#define HOOKS_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif
#include "../globals.h"
#include "../callback_map.h"

// Initialization hooks
typedef GtkApplication* (*gtk_application_new_t)(const char* application_id, GApplicationFlags flags);
extern gtk_application_new_t gtk_application_new_ORIGINAL;

typedef int (*g_application_run_t)(GApplication* application, int argc, char** argv);
extern g_application_run_t g_application_run_ORIGINAL;

typedef void (*gtk_window_present_t)(GtkWindow *window);
extern gtk_window_present_t gtk_window_present_ORIGINAL;

// CSS hooks
typedef void(*gtk_css_provider_load_from_file_t)(GtkCssProvider* provider, GFile* file);
extern gtk_css_provider_load_from_file_t gtk_css_provider_load_from_file_ORIGINAL;

// Signal hooks
typedef gulong (*g_signal_connect_data_t)(gpointer instance,
                                          const gchar *detailed_signal,
                                          GCallback c_handler,
                                          gpointer data,
                                          GClosureNotify destroy_data,
                                          GConnectFlags connect_flags);
extern g_signal_connect_data_t g_signal_connect_data_ORIGINAL;

#endif