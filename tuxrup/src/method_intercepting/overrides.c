#include "hooks.h"
#include "gtk_events.h"
#include "events_util.h"
#include "../right_click/context_menu.h"
#include "../utilities/util.h"
#include "../utilities/pointer_name_conversion.h"

// =========================================================
// CSS OVERRIDES
// =========================================================

void gtk_css_provider_load_from_file_OVERRIDE(GtkCssProvider* provider, GFile* file){
    GFile* fixed_file = file;
    gchar* path = g_file_get_path(file);
    if((path != NULL) && !file_exists(path)){
        gchar* fixed_path = fix_broken_fucking_css_path(path);
        fixed_file = g_file_new_for_path(fixed_path);
    }
    
    gchar *content = NULL;
    gsize length = 0;
    GError *error = NULL;
    if (g_file_load_contents(fixed_file, NULL, &content, &length, NULL, &error)) {
        append_text_to_file("./all_css.css", content);
        g_free(content);
    } else {
        g_print("Error reading the file: %s\n", error->message);
        g_error_free(error);
    }

    gtk_css_provider_load_from_file_ORIGINAL(provider, fixed_file);
}

// =============================================================
// INTIAILIZATION OVERRIDES
// ============================================================
GtkApplication* gtk_application_new_OVERRIDE(const char* application_id, GApplicationFlags flags){
    return gtk_application_new_ORIGINAL(application_id, flags);
}

int g_application_run_OVERRIDE(GApplication* application, int argc, char** argv){
    int res = g_application_run_ORIGINAL(application, argc, argv);
    return res;
}

static bool first_window_present = true;
void gtk_window_present_OVERRIDE(GtkWindow *window){
    if(first_window_present){
        application_root = (GtkWidget*)window;
        first_window_present=false;
    }
    return gtk_window_present_ORIGINAL(window);
}

// ===================================================================
// SIGNAL OVERRIDES
// =====================================================================
gulong
g_signal_connect_data_OVERRIDE(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags){

    if(instance == NULL || widget_hashes == NULL)
    {goto call_original;}

    GtkWidget* widget = get_widget_from_connect_signal(instance);
    if(widget == NULL)
    {goto call_original;}

    enum widget_type_category widget_category = get_widget_type_category(widget);
    if(widget_category == GTK_CATEGORY_UNDEFINED)
    {goto call_original;}
        
    // // If this is the first time we see this widget, add it to the map of widget hashes, and add a "on_added_to_dom" signal for it
    // // We have to do this because there is no general "add_to_dom" function from a shared library we can overwrite
    if(!widget_seen_before(widget)){
        g_hash_table_insert(widget_hashes, widget, NULL);
        g_signal_connect_data_ORIGINAL(widget, "notify::root", G_CALLBACK(on_added_to_dom), NULL, ((void*)0), (GConnectFlags)0);
        
        add_right_click_action(widget, open_right_click_context_menu, widget);
    }

    enum gtk_callback_category callback_category = get_callback_category_from_connect_signal(instance, detailed_signal);
    if(callback_category == GTK_CALLBACK_UNDEFINED)
    {goto call_original;}
    
    if(is_callback_remapable(widget_category, callback_category)){
        char* function_name = get_identifier_from_pointer(c_handler);
        char* callback_name = get_callback_type_category_str(callback_category);
        printf("Added callback %s on widget %p to function name %s\n", callback_name, widget, function_name);
        callback_map_add_original(widget, callback_category, c_handler);
    }

    call_original:
    return g_signal_connect_data_ORIGINAL(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}


