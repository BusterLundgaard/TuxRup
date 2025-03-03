#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "globals.h"
#include "gtk_events.h"

typedef GtkApplication* (*gtk_application_new_t)(const char* application_id, GApplicationFlags flags);

GtkApplication *gtk_application_new(const char *application_id, GApplicationFlags flags){
    static gtk_application_new_t original_gtk_application_new = NULL;
    if (!original_gtk_application_new)
    {
        original_gtk_application_new = (gtk_application_new_t)dlsym(RTLD_NEXT, "gtk_application_new");
        if (!original_gtk_application_new)
        {
            fprintf(stderr, "Error finding original gtk_application_new: %s\n", dlerror());
            return 0;
        }
    }

    GtkApplication* app = original_gtk_application_new(application_id, flags);
    on_init(app);
    return app;
}


gulong
g_signal_connect_data(gpointer instance,
                      const gchar *detailed_signal,
                      GCallback c_handler,
                      gpointer data,
                      GClosureNotify destroy_data,
                      GConnectFlags connect_flags)
{
    static g_signal_connect_data_t original_g_signal_connect_data = NULL;
    if (!original_g_signal_connect_data)
    {
        original_g_signal_connect_data = (g_signal_connect_data_t)dlsym(RTLD_NEXT, "g_signal_connect_data");
        if (!original_g_signal_connect_data)
        {
            fprintf(stderr, "Error finding original g_signal_connect_data: %s\n", dlerror());
            return 0; 
        }
    }

    normal_g_signal_connect_data = original_g_signal_connect_data;

    on_g_signal_connect_data(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
    
    original_g_signal_connect_data(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}


typedef void (*gtk_window_present_t)(GtkWindow *window); // Define the function type

void gtk_window_present(GtkWindow *window)
{
    static gtk_window_present_t original_gtk_window_present = NULL;
    if (!original_gtk_window_present)
    {
        original_gtk_window_present = (gtk_window_present_t)dlsym(RTLD_NEXT, "gtk_window_present");
        if (!original_gtk_window_present)
        {
            fprintf(stderr, "Error finding original gtk_window_present: %s\n", dlerror());
            return; 
        }
    }

    // Print your custom message
    on_gtk_window_present(window);

    // Call the original gtk_window_present function
    original_gtk_window_present(window);
}