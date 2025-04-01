#include <dlfcn.h>

#include "callback_map.h"
#include "util.h"
#include "globals.h"

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

void callback_map_add_original(GtkWidget* widget, enum gtk_callback_category callback_category, gpointer original_pointer, char* original_function_name){
    callback_identifier* cb_key = malloc(sizeof(callback_identifier));
    cb_key->widget = widget;
    cb_key->callback = callback_category;

    callback_info* cb_info = malloc(sizeof(callback_info));
    cb_info->original_function_pointer = (callback_type)original_pointer;
    cb_info->function_name = original_function_name;
    cb_info->dl_handle = NULL;
    cb_info->identifier_pointers = NULL;
    cb_info->identifier_pointers_n = 0;

    g_hash_table_insert(widget_callback_table, cb_key, cb_info);
}

void callback_map_add_new(GtkWidget* widget, enum gtk_callback_category callback_category){
    callback_identifier* cb_key = malloc(sizeof(callback_identifier));
    cb_key->widget = widget;
    cb_key->callback = callback_category;

    callback_info* cb_info = malloc(sizeof(callback_info));
    cb_info->original_function_pointer = (callback_type)NULL;
    cb_info->function_name = NULL;
    cb_info->dl_handle = NULL;
    cb_info->identifier_pointers = NULL;
    cb_info->identifier_pointers_n = 0;

    g_hash_table_insert(widget_callback_table, cb_key, cb_info);
}


bool callback_map_exists(GtkWidget* widget, enum gtk_callback_category callback_category){
    callback_identifier cb_key = {widget, callback_category};
    return g_hash_table_contains(widget_callback_table, &cb_key); 
}

callback_info* callback_map_get(GtkWidget* widget, enum gtk_callback_category callback_category){
    callback_identifier cb_key = {widget, callback_category};
    if(!g_hash_table_contains(widget_callback_table, &cb_key)){
        printf("Warning: Tried to get from the callback_map, but key did not exist! Returning NULL\n");
        return NULL;
    }
    return g_hash_table_lookup(widget_callback_table, &cb_key);
}