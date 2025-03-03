#include <gtk/gtk.h>

#include "globals.h"
#include "callback_map.h"
#include "gtk_events.h"
#include "compute_widget_hash.h"
#include "util.h"
#include "pointer_name_conversion.h"
#include "modify_callback.h"

void on_added_to_dom(GtkWidget* widget, gpointer data){
    guint hash = compute_widget_hash(widget, widget_hashes);
    g_hash_table_insert(widget_hashes, widget, &hash);
}

gulong
on_g_signal_connect_data(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags){

    // if(!g_hash_table_contains(widget_hashes, instance)){
    //     guint undetermined_hash = 0;
    //     g_hash_table_insert(widget_hashes, instance, (gpointer)(&undetermined_hash));
    //     normal_g_signal_connect_data(instance, "notify::root", G_CALLBACK(on_added_to_dom), NULL, ((void*)0), (GConnectFlags) 0);
    // }

    if(is_mappable_action(detailed_signal)){
        callback_identifier cb_key = {
            .widget = instance, 
            .callback_name = detailed_signal};
        // callback_info cb_info = {
        //     .original_function_pointer = (callback_type)c_handler,
        //     .function_name = get_identifier_from_pointer(c_handler),
        //     .dl_handle = NULL,
        //     .identifier_pointers = NULL,
        //     .identifier_pointers_n = 0
        // };
        callback_info cb_info = {
            .original_function_pointer = (callback_type)c_handler,
            .function_name = "",
            .dl_handle = NULL,
            .identifier_pointers = NULL,
            .identifier_pointers_n = 0
        };
        g_hash_table_insert(widget_callback_table, &cb_key, &cb_info);
        simple_tests(c_handler, (GtkWidget*)instance, "clicked");
    }
}


void on_gtk_window_present(GtkWindow *window)
{
    g_print("Print on present yay!\n");
    //clang_tests("../program_src/hello_world.c");
}

void on_init(GtkApplication* app){
    g_print("Print on init yay!\n");
    working_directory = get_working_directory();
    widget_hashes = g_hash_table_new(g_direct_hash, g_direct_equal);
    widget_callback_table = g_hash_table_new_full(callback_key_hash, callback_key_equal, callback_key_free, callback_value_free);
}
