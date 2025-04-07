#include "gtk_events.h"
#include "hooks.h"
#include "events_util.h"

#include "../globals.h"
#include "../callback_map.h"
#include "../utilities/hashing.h" 
#include "../utilities/util.h"
#include "../utilities/pointer_name_conversion.h"
#include "../right_click/context_menu.h"

#include <sys/stat.h>

static bool initialized = false;
void on_init(){
    if(!initialized){
        initialized = true;

        #ifdef USE_GTK3
            printf("lo from GTK 3\n");
        #else
            printf("Hello from GTK 4\n");
        #endif

        set_gtk_version();

        // Get executable metadata like working directory and debug symbols
        working_directory = get_working_directory();
        executable_path = get_executable_directory();

        // Initialize global maps
        widget_hashes = g_hash_table_new(g_direct_hash, g_direct_equal);
        widget_callback_table = g_hash_table_new_full(callback_key_hash, callback_key_equal, callback_key_free, callback_value_free);
        widget_to_css_filepath_map = g_hash_table_new(g_direct_hash, g_direct_equal);

        // Clear temporary files
        FILE *file = fopen("all_css.css", "w"); 
        if (file) {fclose(file);} 
    }
}


void on_added_to_dom(GtkWidget* widget, gpointer data){
    guint* hash = malloc(sizeof(guint));  
    *hash = compute_widget_hash(widget);
    const gchar* widget_type = g_type_name(G_OBJECT_TYPE(widget));    
    g_hash_table_insert(widget_hashes, (gpointer)widget, (gpointer)hash);
}