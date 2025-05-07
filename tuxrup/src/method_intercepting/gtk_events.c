#include <string.h>
#include "gtk_events.h"
#include "hooks.h"
#include "events_util.h"

#include "../globals.h"
#include "../callback_map.h"
#include "../utilities/hashing.h" 
#include "../utilities/util.h"
#include "../utilities/pointer_name_conversion.h"
#include "../right_click/context_menu.h"
#include "../editing_callbacks/edit_callbacks_window.h"

#include <sys/stat.h>

void set_program_name(){
	char* executable_path_copy = g_strdup(executable_path);
    program_name = g_basename(executable_path_copy);
}
void set_program_src_code(){
	for(int i = 0; i < source_code_paths_LEN; i++){
		if(source_code_paths[i].executable_name == NULL){break;}
		if(strcmp(source_code_paths[i].executable_name, program_name) == 0){
			program_src_folder = source_code_paths[i].source_code_path;
			g_print("set program_src_folder to %s\n", program_src_folder);
		}
	}
}

static bool initialized = false;
void on_init(){
    if(!initialized){
        initialized = true;

        #ifdef USE_GTK3
            printf("Hello from GTK 3\n");
        #else
            printf("Hello from GTK 4\n");
        #endif

        set_gtk_version();

        // Get executable metadata like working directory and debug symbols
        executable_path = get_executable_directory();
		working_directory = get_working_directory();
		initialize_debugging_symbols(executable_path);
		set_program_name();
		set_program_src_code();

        // Initialize global maps
        widget_hashes = g_hash_table_new(g_direct_hash, g_direct_equal);
        widget_callback_table = g_hash_table_new_full(callback_key_hash, callback_key_equal, callback_key_free, callback_value_free);
        widget_to_css_filepath_map = g_hash_table_new(g_direct_hash, g_direct_equal);

        // Clear temporary files
        FILE *file = fopen("all_css.css", "w"); 
        if (file) {fclose(file);} 

		testing_create_isolated_function();
    }
}


void on_added_to_dom(GtkWidget* widget, gpointer data){
    guint* hash = malloc(sizeof(guint));  
    *hash = compute_widget_hash(widget);
    const gchar* widget_type = g_type_name(G_OBJECT_TYPE(widget));    
    g_hash_table_insert(widget_hashes, (gpointer)widget, (gpointer)hash);
}
