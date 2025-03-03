#include <dlfcn.h>

#include "function_dispatcher.h"
#include "callback_map.h"
#include "globals.h"

void function_dispatcher(GtkWidget* widget, gpointer data){
    callback_identifier cb_id = {widget, (char*)data};
    callback_info* cb = (callback_info*)g_hash_table_lookup(widget_callback_table, &cb_id);
    if(cb == NULL){return;}

    callback_type their_foo = (callback_type) dlsym(cb->dl_handle, cb->function_name);
    their_foo(widget, cb->identifier_pointers);
}