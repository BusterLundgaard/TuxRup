#include "callback_map.h"

#include "utilities/util.h"
#include "utilities/pointer_name_conversion.h"
#include "globals.h"

#include <dlfcn.h>

guint callback_key_hash(gconstpointer key) {
    const callback_identifier* k = key;
    guint hash = g_int_hash(&k->callback);  // Hash the enum as an integer
    hash ^= g_direct_hash(k->widget);  // Combine with widget hash
    return hash;
}

gboolean callback_key_equal(gconstpointer a, gconstpointer b) {
    const callback_identifier *ka = a;
    const callback_identifier *kb = b;
    return (ka->widget == kb->widget) && (ka->callback == kb->callback);
}

void callback_key_free(gpointer data) {
    g_free((gpointer)data);  
}

void callback_value_free(void* data){
    callback_info* cb = data;
    g_free(cb->function_name);
    g_free(cb->identifier_pointers);
    if(cb->dl_handle != NULL){
        dlclose(cb->dl_handle);
    }
    g_free(cb);
}

callback_info* callback_map_add_original(GtkWidget* widget, enum gtk_callback_category callback_category, gpointer original_pointer, gpointer original_user_data){
    callback_identifier* cb_key = malloc(sizeof(callback_identifier));
    *cb_key = (callback_identifier){};
    cb_key->widget = widget;
    cb_key->callback = callback_category;
    
    callback_info* cb_info = malloc(sizeof(callback_info));
    *cb_info = (callback_info) {};
    cb_info->id = cb_key;
    cb_info->original_function_pointer = original_pointer;
	cb_info->original_user_data = original_user_data;
    
    g_hash_table_insert(widget_callback_table, cb_key, cb_info);
    return cb_info;
}

callback_info* callback_map_add_new(GtkWidget* widget, enum gtk_callback_category callback_category){
    return callback_map_add_original(widget, callback_category, NULL, NULL);
}


bool callback_map_exists(GtkWidget* widget, enum gtk_callback_category callback_category){
    callback_identifier cb_key = {widget, callback_category};
    return g_hash_table_contains(widget_callback_table, &cb_key); 
}

callback_info* callback_map_get(GtkWidget* widget, enum gtk_callback_category callback_category){
    printf("looking up info for callback for widget %p with category %d\n", widget, (int)callback_category);
    callback_identifier cb_key = {widget, callback_category};
    if(!g_hash_table_contains(widget_callback_table, &cb_key)){
        printf("Warning: Tried to get from the callback_map, but key did not exist! Returning NULL\n");
        return NULL;
    }
    return g_hash_table_lookup(widget_callback_table, &cb_key);
}
