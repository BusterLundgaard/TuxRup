#include <gtk-3.0/gtk/gtk.h>

extern GtkWidget* application_root;
extern GtkWidget* tuxrup_root;
extern GtkWidget* selected_widget;

typedef gulong (*g_signal_connect_data_t)(gpointer instance,
                                          const gchar *detailed_signal,
                                          GCallback c_handler,
                                          gpointer data,
                                          GClosureNotify destroy_data,
                                          GConnectFlags connect_flags);
g_signal_connect_data_t g_signal_connect_data_original;
