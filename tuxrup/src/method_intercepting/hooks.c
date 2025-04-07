
#include "hooks.h"

#include "../globals.h"
#include "../utilities/util.h"

#include "gtk_events.h"
#include "overrides.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void set_original_function(gpointer* fun_pointer, char* name){
    if(*fun_pointer == NULL){
        *fun_pointer = dlsym(RTLD_NEXT, name);
        if(!fun_pointer){
            printf("error loading function %s.\n", name);
        }
    }
}


// ====================================
// CSS
// =====================================
gtk_css_provider_load_from_file_t gtk_css_provider_load_from_file_ORIGINAL = NULL;
void gtk_css_provider_load_from_file(GtkCssProvider *provider, GFile *file)
{   
    set_original_function((gpointer*)&gtk_css_provider_load_from_file_ORIGINAL, "gtk_css_provider_load_from_file");
    return gtk_css_provider_load_from_file_OVERRIDE(provider, file);
}

// ====================================
// Initialization
// ====================================
gtk_application_new_t gtk_application_new_ORIGINAL = NULL;

GtkApplication *gtk_application_new(const char *application_id, GApplicationFlags flags){
    on_init();
    set_original_function((gpointer*)&gtk_application_new_ORIGINAL, "gtk_application_new");
    return gtk_application_new_OVERRIDE(application_id, flags);
}

g_application_run_t g_application_run_ORIGINAL = NULL;
int g_application_run(GApplication* application, int argc, char** argv){
    set_original_function((gpointer*)&g_application_run_ORIGINAL, "g_application_run");
    return g_application_run_OVERRIDE(application,argc,argv);
}

gtk_window_present_t gtk_window_present_ORIGINAL = NULL;
void gtk_window_present(GtkWindow *window)
{
    set_original_function((gpointer*)&gtk_window_present_ORIGINAL, "gtk_window_present");
    return gtk_window_present_OVERRIDE(window);
}

// =======================================
// SIGNALS / CALLBACKS
// =======================================
g_signal_connect_data_t g_signal_connect_data_ORIGINAL = NULL;
gulong
g_signal_connect_data(gpointer instance,
                      const gchar *detailed_signal,
                      GCallback c_handler,
                      gpointer data,
                      GClosureNotify destroy_data,
                      GConnectFlags connect_flags)
{
    set_original_function((gpointer*)&g_signal_connect_data_ORIGINAL, "g_signal_connect_data");
    return g_signal_connect_data_OVERRIDE(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}