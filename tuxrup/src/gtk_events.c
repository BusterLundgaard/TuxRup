#include <gtk/gtk.h>

#include "globals.h"
#include "callback_map.h"
#include "gtk_events.h"
#include "compute_widget_hash.h"
#include "util.h"
#include "pointer_name_conversion.h"
#include "modify_callback.h"

void on_right_click(GtkGestureClick *gesture,int, double x, double y, gpointer* self){
    GtkWidget* widget = GTK_WIDGET(self); //widget that was clicked
    //guint hash_value = *((guint*)(g_hash_table_lookup(widget_hashes, widget)));
    
    callback_identifier cb_id = {widget, "clicked"};
    gpointer value = g_hash_table_lookup(widget_callback_table, &cb_id);
    if(value == NULL){
        g_print("Couldn't find the callback information for the widget you just right-clicked..\n");
        return;
    }

    modify_callback(((callback_info*)value)->original_function_pointer, widget, "clicked", "");
    
    // g_print("hash of cb_id you right clicked is: %u\n", callback_key_hash(&cb_id));
    // gpointer key, value;
    // GHashTableIter iter;
    // g_hash_table_iter_init(&iter, widget_callback_table);
    // for(; g_hash_table_iter_next(&iter, &key, &value); ){
    //     printf("the map contains some key with hash: %u\n", callback_key_hash(key));
    //     printf("is this key equal to ours? %d\n", callback_key_equal(key, (gpointer)(&cb_id)));
    // }
    //g_print("amount of things in table: %d", g_hash_table_size(widget_callback_table));
    
    //gpointer value = g_hash_table_lookup(widget_callback_table, &cb_id);
    //callback_info* cb = (callback_info*)value;
    //printf("found out that the callback is %p\n", cb->original_function_pointer);



    //modify_callback(  )
}

// static void show_code_editor(GtkGestureClick *gesture,int, double x, double y, gpointer* self){

void on_added_to_dom(GtkWidget* widget, gpointer data){
    guint* hash = malloc(sizeof(guint));  
    *hash = compute_widget_hash(widget, widget_hashes);
    const gchar* widget_type = g_type_name(G_OBJECT_TYPE(widget));    
    g_hash_table_insert(widget_hashes, (gpointer)widget, (gpointer)hash);
}


gulong
on_g_signal_connect_data(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags){

    if(widget_hashes == NULL)
    {return 0;}

    if(is_mappable_action(detailed_signal)){
        callback_identifier* cb_key = malloc(sizeof(callback_identifier));
        callback_info* cb_info = malloc(sizeof(callback_info));
        
        cb_key->widget = instance;
        cb_key->callback_name = g_strdup(detailed_signal);

        cb_info->original_function_pointer = (callback_type)c_handler;
        cb_info->function_name = get_identifier_from_pointer(c_handler);
        cb_info->dl_handle = NULL;
        cb_info->identifier_pointers = NULL;
        cb_info->identifier_pointers_n = 0;

        g_hash_table_insert(widget_callback_table, cb_key, cb_info);
    }

    if(!g_hash_table_contains(widget_hashes, instance)){
        g_hash_table_insert(widget_hashes, instance, NULL);
        normal_g_signal_connect_data(instance, "notify::root", G_CALLBACK(on_added_to_dom), NULL, ((void*)0), (GConnectFlags)0);

        if(is_mappable_action(detailed_signal)){
            GtkGesture* gesture = gtk_gesture_click_new();
            gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3);
            gtk_widget_add_controller(GTK_WIDGET(instance), GTK_EVENT_CONTROLLER(gesture));
            
            normal_g_signal_connect_data(gesture, "pressed", G_CALLBACK(on_right_click), instance, NULL, (GConnectFlags)0);
        }
    }

    return 0;
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
