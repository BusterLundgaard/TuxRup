#include <dlfcn.h>

#include "callback_map.h"
#include "util.h"

guint callback_key_hash(gconstpointer key){
    const callback_identifier* k = key;
    guint hash = hash_string(k->callback_name);
    hash ^= g_direct_hash(k->widget);
    return hash;
}

gboolean callback_key_equal(gconstpointer a, gconstpointer b){
    const callback_identifier *ka = a;
    const callback_identifier *kb = b;
    return (ka->widget == kb->widget) && strcmp(ka->callback_name, kb->callback_name) == 0;
}

void callback_key_free(gpointer data){
    callback_identifier *k = data;
    g_free(k->callback_name);
    g_free((gpointer)k);
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

void add_callback_to_table(GtkWidget* widget, const gchar* detailed_signal, gpointer c_handler, char* function_name){
    callback_identifier* cb_key = malloc(sizeof(callback_identifier));
    cb_key->widget = widget;
    cb_key->callback_name = g_strdup(detailed_signal);

    callback_info* cb_info = malloc(sizeof(callback_info));
    cb_info->original_function_pointer = (callback_type)c_handler;
    cb_info->function_name = function_name;
    cb_info->dl_handle = NULL;
    cb_info->identifier_pointers = NULL;
    cb_info->identifier_pointers_n = 0;

    g_hash_table_insert(widget_callback_table, cb_key, cb_info);
}