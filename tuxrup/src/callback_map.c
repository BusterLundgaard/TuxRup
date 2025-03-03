#include <dlfcn.h>

#include "callback_map.h"

guint callback_key_hash(gconstpointer key){
    const callback_identifier *k = key;
    guint hash = g_str_hash(k->callback_name);
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