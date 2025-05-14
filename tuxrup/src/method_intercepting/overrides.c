#include "hooks.h"
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

// -------------------------------
// helper methods
// -------------------------------
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
void debug_add_red_background_class(GtkWidget* window){
	GtkCssProvider *provider = gtk_css_provider_new();    
    gtk_css_provider_load_from_data(provider,    
        ".known { background: red; } \n .tuxrup_selected{ background: blue }",    
        -1, NULL);    
    GdkScreen *screen = gtk_widget_get_screen(window);
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
		/* gtk_style_context_add_class(gtk_widget_get_style_context(widget), "known"); */
		add_right_click_action(widget, select_widget_for_action, widget);
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

//------------------------------
// actual overrides
// ----------------------------
GtkApplication* gtk_application_new_OVERRIDE(const char* application_id, GApplicationFlags flags){
    /* if(!initialized){ */
    /*     initialized = true; */
    /*     on_init(); */    
    /* } */
    return gtk_application_new_ORIGINAL(application_id, flags);
}

int g_application_run_OVERRIDE(GApplication* application, int argc, char** argv){
    /* if(!initialized){ */
    /*     initialized = true; */
    /*     on_init(); */    
    /* } */
    return g_application_run_ORIGINAL(application, argc, argv);
}

__attribute__((constructor))
void real_init(){
	g_print("i am called as the very first!!!!!\n");
}

void on_init(){
	set_gtk_version();

	// Initialize global maps
	known_widgets = g_hash_table_new(g_direct_hash, g_direct_equal);
	computed_hashes = g_hash_table_new(g_direct_hash, g_direct_equal);
	widget_callback_table = g_hash_table_new_full(callback_key_hash, callback_key_equal, callback_key_free, callback_value_free);
	widget_to_css_filepath_map = g_hash_table_new(g_direct_hash, g_direct_equal);

	// Get executable metadata like working directory and debug symbols
	executable_path = get_executable_directory();
	working_directory = get_working_directory();
	initialize_debugging_symbols(executable_path);
	set_program_name();
	set_program_src_code();

	// Clear temporary files
	FILE *file = fopen("all_css.css", "w"); 
	if (file) {fclose(file);} 

	// Finally open the tuxrup menu
	open_actions_menu();
}

void gtk_window_present_OVERRIDE(GtkWindow *window){
	gtk_window_present_ORIGINAL(window);
    if(!initialized){
		application_root = (GtkWidget*)window;
        initialized = true;
	   	on_init();    
    }
	register_all_children((GtkWidget*)window);
	add_right_click_actions();
}
void gtk_widget_show_all_OVERRIDE(GtkWidget* window){
	g_print("gtk_widget_show_all was called\n");
	gtk_widget_show_all_ORIGINAL(window);
    if(!initialized){
		application_root = (GtkWidget*)window;
        initialized = true;
	   	on_init();    
    }
	register_all_children((GtkWidget*)window);
	add_right_click_actions();
	debug_add_red_background_class((GtkWidget*)window);
}

// ===================================================================
// WIDGET APPEND ACTIONS
// ==================================================================
#ifdef USE_GTK3
void add_widget(GtkWidget* widget){
	if(known_widgets == NULL){
		known_widgets = g_hash_table_new(g_direct_hash, g_direct_equal);
	}
	g_print("inserting widgets via the hooked append function\n");
    g_hash_table_insert(known_widgets, widget, NULL);
	on_widget_known(widget);
}
void gtk_container_add_OVERRIDE(GtkContainer *container, GtkWidget *child){add_widget(child);gtk_container_add_ORIGINAL(container, child);}
void gtk_box_pack_start_OVERRIDE(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding){add_widget(child);gtk_box_pack_start_ORIGINAL(box, child, expand, fill, padding);}
void gtk_box_pack_end_OVERRIDE(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding){add_widget(child);gtk_box_pack_end_ORIGINAL(box, child, expand, fill, padding);}
void gtk_box_insert_child_after_OVERRIDE(GtkBox *box, GtkWidget *child, GtkWidget *sibling) {add_widget(child);gtk_box_insert_child_after_ORIGINAL(box, child, sibling);}
void gtk_grid_attach_OVERRIDE(GtkGrid *grid, GtkWidget *child, gint left, gint top, gint width, gint height){add_widget(child);gtk_grid_attach_ORIGINAL(grid, child, left, top, width, height);}
void gtk_grid_attach_next_to_OVERRIDE(GtkGrid *grid, GtkWidget *child, GtkWidget *sibling, GtkPositionType side, gint width, gint height){add_widget(child);gtk_grid_attach_next_to_ORIGINAL(grid, child, sibling, side, width, height);}
void gtk_fixed_put_OVERRIDE(GtkFixed *fixed, GtkWidget *widget, gint x, gint y){add_widget(widget);gtk_fixed_put_ORIGINAL(fixed, widget, x, y);}
gint gtk_notebook_append_page_OVERRIDE(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label){add_widget(child);gtk_notebook_append_page_ORIGINAL(notebook, child, tab_label);}
gint gtk_notebook_insert_page_OVERRIDE(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label, gint position){add_widget(child);gtk_notebook_insert_page_ORIGINAL(notebook, child, tab_label, position);}
void gtk_paned_pack1_OVERRIDE(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink){add_widget(child);gtk_paned_pack1_ORIGINAL(paned, child, resize, shrink);}
void gtk_paned_pack2_OVERRIDE(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink){add_widget(child);gtk_paned_pack2_ORIGINAL(paned, child, resize, shrink);}
void gtk_scrolled_window_add_with_viewport_OVERRIDE(GtkScrolledWindow *scrolled_window, GtkWidget *child){add_widget(child);gtk_scrolled_window_add_with_viewport_ORIGINAL(scrolled_window, child);}
void gtk_overlay_add_overlay_OVERRIDE(GtkOverlay *overlay, GtkWidget *child){add_widget(child);gtk_overlay_add_overlay_ORIGINAL(overlay, child);}
void gtk_stack_add_titled_OVERRIDE(GtkStack *stack, GtkWidget *child, const gchar *name, const gchar *title){add_widget(child);gtk_stack_add_titled_ORIGINAL(stack, child, name, title);}
void gtk_stack_add_named_OVERRIDE(GtkStack *stack, GtkWidget *child, const gchar *name){add_widget(child);gtk_stack_add_named_ORIGINAL(stack, child, name);}
void gtk_header_bar_pack_start_OVERRIDE(GtkHeaderBar *bar, GtkWidget *child){add_widget(child);gtk_header_bar_pack_start_ORIGINAL(bar, child);}
void gtk_header_bar_pack_end_OVERRIDE(GtkHeaderBar *bar, GtkWidget *child){add_widget(child);gtk_header_bar_pack_end_ORIGINAL(bar, child);}
void gtk_menu_shell_append_OVERRIDE(GtkMenuShell *menu_shell, GtkWidget *child){add_widget(child);gtk_menu_shell_append_ORIGINAL(menu_shell, child);}
void gtk_menu_shell_prepend_OVERRIDE(GtkMenuShell *menu_shell, GtkWidget *child){add_widget(child);gtk_menu_shell_prepend_ORIGINAL(menu_shell, child);}
void gtk_menu_shell_insert_OVERRIDE(GtkMenuShell *menu_shell, GtkWidget *child, gint position){add_widget(child);gtk_menu_shell_insert_ORIGINAL(menu_shell, child, position);}
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
	if(widget_callback_table == NULL){
		widget_callback_table = g_hash_table_new_full(callback_key_hash, callback_key_equal, callback_key_free, callback_value_free);
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


