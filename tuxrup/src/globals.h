#ifndef GLOBALS_H
#define GLOBALS_H

#include <gtk/gtk.h>

typedef gulong (*g_signal_connect_data_t)(gpointer instance,
                                          const gchar *detailed_signal,
                                          GCallback c_handler,
                                          gpointer data,
                                          GClosureNotify destroy_data,
                                          GConnectFlags connect_flags);

extern g_signal_connect_data_t normal_g_signal_connect_data;

extern GHashTable *widget_callback_table;
extern GHashTable *widget_hashes;


#define MAPPABLE_ACTIONS_LEN 1
extern const char* program_src_folder;
extern const char* working_directory;
extern const char *remapable_events[MAPPABLE_ACTIONS_LEN];

#endif