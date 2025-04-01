#include "globals.h"
#include "callback_map.h"
#include "gtk_events.h"
#include "utilities/hashing.h" 
#include "utilities/util.h"
#include "utilities/pointer_name_conversion.h"
#include "right_click/context_menu.h"

#include <sys/stat.h>

void on_added_to_dom(GtkWidget* widget, gpointer data){
    guint* hash = malloc(sizeof(guint));  
    *hash = compute_widget_hash(widget);
    const gchar* widget_type = g_type_name(G_OBJECT_TYPE(widget));    
    g_hash_table_insert(widget_hashes, (gpointer)widget, (gpointer)hash);
}

void our_gtk_css_provider_load_from_file(GFile* file){
    gchar *content = NULL;
    gsize length = 0;
    GError *error = NULL;
    if (g_file_load_contents(file, NULL, &content, &length, NULL, &error)) {
        append_text_to_file("./all_css.css", content);
        g_free(content);
    } else {
        g_print("Error reading the file: %s\n", error->message);
        g_error_free(error);
    }
}

gulong
on_g_signal_connect_data(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags){

    if(instance == NULL || widget_hashes == NULL)
    {return 0;}

    GtkWidget* widget = get_widget_from_connect_signal(instance);
    if(widget == NULL)
    {return 0;}

    enum widget_type_category widget_category = get_widget_type_category(widget);
    if(widget_category == GTK_CATEGORY_UNDEFINED)
    {return 0;}
        
    // // If this is the first time we see this widget, add it to the map of widget hashes, and add a "on_added_to_dom" signal for it
    // // We have to do this because there is no general "add_to_dom" function from a shared library we can overwrite
    if(!widget_seen_before(widget)){
        g_hash_table_insert(widget_hashes, widget, NULL);
        normal_g_signal_connect_data(widget, "notify::root", G_CALLBACK(on_added_to_dom), NULL, ((void*)0), (GConnectFlags)0);
        
        add_right_click_action(widget, open_right_click_context_menu, widget);
    }

    enum gtk_callback_category callback_category = get_callback_category_from_connect_signal(instance, detailed_signal);
    if(callback_category == GTK_CALLBACK_UNDEFINED)
    {return 0;}
    
    if(is_callback_remapable(widget_category, callback_category)){
        char* function_name = get_identifier_from_pointer(c_handler);
        add_callback_to_table(widget, detailed_signal, c_handler, function_name);
    }

    return 0;
}

static bool first_window_present = true;

void on_gtk_window_present(GtkWindow *window)
{
    if(first_window_present){
        application_root = (GtkWidget*)window;
        first_window_present=false;
    }
}

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
