#include <dlfcn.h>

#include "function_dispatcher.h"

#include "callback_map.h"
#include "globals.h"
#include "utilities/pointer_name_conversion.h"

void function_dispatcher(GtkWidget* widget, gpointer data){
    callback_identifier cb_id = {widget, *((enum gtk_callback_category*)data)};
    callback_info* cb_info = (callback_info*)g_hash_table_lookup(widget_callback_table, &cb_id);
    if(cb_info == NULL){return;}

    callback_type their_foo = (callback_type) dlsym(cb_info->dl_handle, cb_info->function_name);
    if(their_foo == NULL){
        g_print("Could not find function in function_dispatcher!\n");
        return;
    }

	
	initialize_shared_lib(cb_info->shared_library_path, cb_info->dl_handle);
	sync_shared_variables_to_main(true);
	g_print("calling their function!");
    their_foo(widget, cb_info->original_user_data);
	sync_shared_variables_to_main(false);
}
