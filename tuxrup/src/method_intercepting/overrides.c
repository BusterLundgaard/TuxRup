#include "hooks.h"
#include "gtk_events.h"
#include "events_util.h"
#include "../right_click/context_menu.h"
#include "../utilities/util.h"
#include "../utilities/pointer_name_conversion.h"
#include "../utilities/hashing.h"

// =========================================================
// CSS OVERRIDES
// =========================================================

#ifdef USE_GTK3
gboolean gtk_css_provider_load_from_file_OVERRIDE(GtkCssProvider* provider, GFile* file, GError** err){
#else
void gtk_css_provider_load_from_file_OVERRIDE(GtkCssProvider* provider, GFile* file){
#endif
    GFile* fixed_file = file;
    gchar* path = g_file_get_path(file);
    if((path != NULL) && !file_exists(path)){
        gchar* fixed_path = fix_broken_fucking_css_path(path);
        fixed_file = g_file_new_for_path(fixed_path);
    }
    
    gchar *content = NULL;
    gsize length = 0;
    GError *error = NULL;
    if (g_file_load_contents(fixed_file, NULL, &content, &length, NULL, &error)) {
        append_text_to_file("./all_css.css", content);
        g_free(content);
    } else {
        g_print("Error reading the file: %s\n", error->message);
        g_error_free(error);
    }

    #ifdef USE_GTK3
    gtk_css_provider_load_from_file_ORIGINAL(provider, fixed_file, err);
    #else
    gtk_css_provider_load_from_file_ORIGINAL(provider, fixed_file);
    #endif
}

// =============================================================
// INTIAILIZATION OVERRIDES
// ============================================================
static bool initialized = false;
static bool window_presented = false;

#ifdef USE_GTK3
void register_all_children(GtkWidget* widget){
    g_hash_table_insert(known_widgets, widget, NULL);

    if (GTK_IS_WIDGET(widget) && GTK_IS_CONTAINER(widget)) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
        int index = 0;
        for (GList *l = children; l != NULL; l = l->next, index++) {
            GtkWidget *child = GTK_WIDGET(l->data);
            register_all_children(child);
        }
        g_list_free(children);  
    }
}
#else
void register_all_children(GtkWidget* widget){
    g_hash_table_insert(known_widgets, widget, NULL);

    if (GTK_IS_WIDGET(widget)) {
        GtkWidget *child = NULL;
        GtkWidget *next_child = gtk_widget_get_first_child(widget);
        while (next_child) {
            child = next_child;
            register_all_children(child);
            next_child = gtk_widget_get_next_sibling(child);
        }
    }
}
#endif

#ifdef USE_GTK3
void debug_add_red_background_class(){
	GtkCssProvider *provider = gtk_css_provider_new();    
    gtk_css_provider_load_from_data(provider,    
        ".known { background: red; }",    
        -1, NULL);    
    GdkScreen *screen = gtk_widget_get_screen(application_root);
    gtk_style_context_add_provider_for_screen(screen,
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider); 
}
#endif

// Goes through all known widgets and adds right click actions to the ones where it applies
void on_widget_known(GtkWidget* widget){
	enum widget_type_category widget_category = get_widget_type_category(widget);
	if(widget_category != GTK_CATEGORY_UNDEFINED && widget_category != GTK_CATEGORY_Window){
		gtk_style_context_add_class(gtk_widget_get_style_context(widget), "known");
		add_right_click_action(widget, open_right_click_context_menu, widget);
	}
}
void add_right_click_actions(){
	GHashTableIter iter;
	gpointer key;
	g_hash_table_iter_init(&iter, known_widgets);
	for(; g_hash_table_iter_next(&iter, &key, NULL); ){
		on_widget_known(key);
	}
}

GtkApplication* gtk_application_new_OVERRIDE(const char* application_id, GApplicationFlags flags){
    if(!initialized){
        initialized = true;
        on_init();    
    }
    return gtk_application_new_ORIGINAL(application_id, flags);
}

int g_application_run_OVERRIDE(GApplication* application, int argc, char** argv){
    if(!initialized){
        initialized = true;
        on_init();    
    }
    return g_application_run_ORIGINAL(application, argc, argv);
}

void on_window_present(GtkWindow* window){
    if(!initialized){
        initialized = true;
        on_init();    
    }

    register_all_children((GtkWidget*)window);
	add_right_click_actions();
#ifdef USE_GTK3
	debug_add_red_background_class();
#endif
    if(!window_presented){
        application_root = (GtkWidget*)window;
        window_presented = true;
		/* test_function_overwriting2(); */
    }
}

void gtk_window_present_OVERRIDE(GtkWindow *window){
	on_window_present(window);
    gtk_window_present_ORIGINAL(window);
}
void gtk_widget_show_all_OVERRIDE(GtkWidget* widget){
	if(GTK_IS_WINDOW(widget)){
		g_print("calling on_window_present()");
		on_window_present(GTK_WINDOW(widget));
	}
	gtk_widget_show_all_ORIGINAL(widget);
}


//===================================================================
// WIDGET APPEND ACTIONS
// ==================================================================
#ifdef USE_GTK3
void gtk_container_add_OVERRIDE(GtkContainer* cont, GtkWidget* widget){
	g_hash_table_insert(known_widgets, widget, NULL);
	on_widget_known(widget);
}
#endif

// ===================================================================
// SIGNAL OVERRIDES
// =====================================================================
gulong
g_signal_connect_data_OVERRIDE(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags){
    if(!initialized){
        initialized = true;
        on_init();    
    }

    if(instance == NULL)
    {goto call_original;}

    GtkWidget* widget = get_widget_from_connect_signal(instance);
    if(widget == NULL)
    {goto call_original;}

    enum widget_type_category widget_category = get_widget_type_category(widget);
    if(widget_category == GTK_CATEGORY_UNDEFINED)
    {goto call_original;}

    enum gtk_callback_category callback_category = get_callback_category_from_connect_signal(instance, detailed_signal);
    if(callback_category == GTK_CALLBACK_UNDEFINED)
    {goto call_original;}
    
    if(!is_callback_remapable(widget_category, callback_category))
	{goto call_original;}

	callback_map_add_original(widget, callback_category, c_handler, data);

    call_original:
    return g_signal_connect_data_ORIGINAL(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}


