
#include "hooks.h"

#include "../globals.h"
#include "../utilities/util.h"

#include "overrides.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void set_original_function(gpointer* fun_pointer, char* name){
    if(*fun_pointer == NULL){
        *fun_pointer = dlsym(RTLD_NEXT, name);
        if(!fun_pointer){
            printf("error loading function %s.\n", name);
        }
    }
}


// ====================================
// CSS
// =====================================
gtk_css_provider_load_from_file_t gtk_css_provider_load_from_file_ORIGINAL = NULL;
#ifdef USE_GTK3
gboolean gtk_css_provider_load_from_file(GtkCssProvider *provider, GFile *file, GError** err)
{   
    set_original_function((gpointer*)&gtk_css_provider_load_from_file_ORIGINAL, "gtk_css_provider_load_from_file");
    return gtk_css_provider_load_from_file_OVERRIDE(provider, file, err);
}
#else
void gtk_css_provider_load_from_file(GtkCssProvider *provider, GFile *file)
{   
    set_original_function((gpointer*)&gtk_css_provider_load_from_file_ORIGINAL, "gtk_css_provider_load_from_file");
    return gtk_css_provider_load_from_file_OVERRIDE(provider, file);
}
#endif

// ====================================
// Initialization
// ====================================
gtk_application_new_t gtk_application_new_ORIGINAL = NULL;

GtkApplication *gtk_application_new(const char *application_id, GApplicationFlags flags){
    set_original_function((gpointer*)&gtk_application_new_ORIGINAL, "gtk_application_new");
    return gtk_application_new_OVERRIDE(application_id, flags);
}

g_application_run_t g_application_run_ORIGINAL = NULL;
int g_application_run(GApplication* application, int argc, char** argv){
    set_original_function((gpointer*)&g_application_run_ORIGINAL, "g_application_run");
    return g_application_run_OVERRIDE(application,argc,argv);
}

gtk_window_present_t gtk_window_present_ORIGINAL = NULL;
void gtk_window_present(GtkWindow *window)
{
    set_original_function((gpointer*)&gtk_window_present_ORIGINAL, "gtk_window_present");
    return gtk_window_present_OVERRIDE(window);
}

gtk_widget_show_all_t gtk_widget_show_all_ORIGINAL = NULL;
void gtk_widget_show_all(GtkWidget *widget)
{
    set_original_function((gpointer*)&gtk_widget_show_all_ORIGINAL, "gtk_widget_show_all");
    return gtk_widget_show_all_OVERRIDE(widget);
}

// ===========================================
// Widget adding 
// ==========================================
#ifdef USE_GTK3
gtk_box_pack_start_t gtk_box_pack_start_ORIGINAL=NULL;
gtk_container_add_t gtk_container_add_ORIGINAL=NULL;
gtk_box_pack_end_t gtk_box_pack_end_ORIGINAL=NULL;
gtk_box_insert_child_after_t gtk_box_insert_child_after_ORIGINAL=NULL;
gtk_grid_attach_t gtk_grid_attach_ORIGINAL=NULL;
gtk_grid_attach_next_to_t gtk_grid_attach_next_to_ORIGINAL=NULL;
gtk_fixed_put_t gtk_fixed_put_ORIGINAL=NULL;
gtk_notebook_append_page_t gtk_notebook_append_page_ORIGINAL=NULL;
gtk_notebook_insert_page_t gtk_notebook_insert_page_ORIGINAL=NULL;
gtk_paned_pack1_t gtk_paned_pack1_ORIGINAL=NULL;
gtk_paned_pack2_t gtk_paned_pack2_ORIGINAL=NULL;
gtk_scrolled_window_add_with_viewport_t gtk_scrolled_window_add_with_viewport_ORIGINAL=NULL;
gtk_overlay_add_overlay_t gtk_overlay_add_overlay_ORIGINAL=NULL;
gtk_stack_add_titled_t gtk_stack_add_titled_ORIGINAL=NULL;
gtk_stack_add_named_t gtk_stack_add_named_ORIGINAL=NULL;
gtk_header_bar_pack_start_t gtk_header_bar_pack_start_ORIGINAL=NULL;
gtk_header_bar_pack_end_t gtk_header_bar_pack_end_ORIGINAL=NULL;
gtk_menu_shell_append_t gtk_menu_shell_append_ORIGINAL=NULL;
gtk_menu_shell_prepend_t gtk_menu_shell_prepend_ORIGINAL=NULL;
gtk_menu_shell_insert_t gtk_menu_shell_insert_ORIGINAL=NULL;

void gtk_container_add(GtkContainer *container, GtkWidget *widget){set_original_function((gpointer*)&gtk_container_add_ORIGINAL, "gtk_container_add"); return gtk_container_add_OVERRIDE(container, widget);}
void gtk_box_pack_start(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding){set_original_function((gpointer*)&gtk_box_pack_start_ORIGINAL,"gtk_box_pack_start"); return gtk_box_pack_start_OVERRIDE(box, child, expand, fill, padding);}
void gtk_box_pack_end(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding){set_original_function((gpointer*)&gtk_box_pack_end_ORIGINAL,"gtk_box_pack_end"); return gtk_box_pack_end_OVERRIDE(box, child, expand, fill, padding);}
void gtk_box_insert_child_after(GtkBox *box, GtkWidget *child, GtkWidget *sibling) {set_original_function((gpointer*)&gtk_box_insert_child_after_ORIGINAL,"gtk_box_insert_child_after"); return gtk_box_insert_child_after_OVERRIDE(box, child, sibling);}
void gtk_grid_attach(GtkGrid *grid, GtkWidget *child, gint left, gint top, gint width, gint height){set_original_function((gpointer*)&gtk_grid_attach_ORIGINAL,"gtk_grid_attach"); return gtk_grid_attach_OVERRIDE(grid, child, left, top, width, height);}
void gtk_grid_attach_next_to(GtkGrid *grid, GtkWidget *child, GtkWidget *sibling, GtkPositionType side, gint width, gint height){set_original_function((gpointer*)&gtk_grid_attach_next_to_ORIGINAL,"gtk_grid_attach_next_to"); return gtk_grid_attach_next_to_OVERRIDE(grid, child, sibling, side, width, height);}
void gtk_fixed_put(GtkFixed *fixed, GtkWidget *widget, gint x, gint y){set_original_function((gpointer*)&gtk_fixed_put_ORIGINAL,"gtk_fixed_put"); return gtk_fixed_put_OVERRIDE(fixed, widget, x, y);}
gint gtk_notebook_append_page(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label){set_original_function((gpointer*)&gtk_notebook_append_page_ORIGINAL,"gtk_notebook_append_page"); return gtk_notebook_append_page_OVERRIDE(notebook, child, tab_label);}
gint gtk_notebook_insert_page(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label, gint position){set_original_function((gpointer*)&gtk_notebook_insert_page_ORIGINAL,"gtk_notebook_insert_page"); return gtk_notebook_insert_page_OVERRIDE(notebook, child, tab_label, position);}
void gtk_paned_pack1(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink){set_original_function((gpointer*)&gtk_paned_pack1_ORIGINAL,"gtk_paned_pack1"); return gtk_paned_pack1_OVERRIDE(paned, child, resize, shrink);}
void gtk_paned_pack2(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink){set_original_function((gpointer*)&gtk_paned_pack2_ORIGINAL,"gtk_paned_pack2"); return gtk_paned_pack2_OVERRIDE(paned, child, resize, shrink);}
void gtk_scrolled_window_add_with_viewport(GtkScrolledWindow *scrolled_window, GtkWidget *child){set_original_function((gpointer*)&gtk_scrolled_window_add_with_viewport_ORIGINAL,"gtk_scrolled_window_add_with_viewport"); return gtk_scrolled_window_add_with_viewport_OVERRIDE(scrolled_window, child);}
void gtk_overlay_add_overlay(GtkOverlay *overlay, GtkWidget *widget){set_original_function((gpointer*)&gtk_overlay_add_overlay_ORIGINAL,"gtk_overlay_add_overlay"); return gtk_overlay_add_overlay_OVERRIDE(overlay, widget);}
void gtk_stack_add_titled(GtkStack *stack, GtkWidget *child, const gchar *name, const gchar *title){set_original_function((gpointer*)&gtk_stack_add_titled_ORIGINAL,"gtk_stack_add_titled"); return gtk_stack_add_titled_OVERRIDE(stack, child, name, title);}
void gtk_stack_add_named(GtkStack *stack, GtkWidget *child, const gchar *name){set_original_function((gpointer*)&gtk_stack_add_named_ORIGINAL,"gtk_stack_add_named"); return gtk_stack_add_named_OVERRIDE(stack, child, name);}
void gtk_header_bar_pack_start(GtkHeaderBar *bar, GtkWidget *child){set_original_function((gpointer*)&gtk_header_bar_pack_start_ORIGINAL,"gtk_header_bar_pack_start"); return gtk_header_bar_pack_start_OVERRIDE(bar, child);}
void gtk_header_bar_pack_end(GtkHeaderBar *bar, GtkWidget *child){set_original_function((gpointer*)&gtk_header_bar_pack_end_ORIGINAL,"gtk_header_bar_pack_end"); return gtk_header_bar_pack_end_OVERRIDE(bar, child);}
void gtk_menu_shell_append(GtkMenuShell *menu_shell, GtkWidget *child){set_original_function((gpointer*)&gtk_menu_shell_append_ORIGINAL,"gtk_menu_shell_append"); return gtk_menu_shell_append_OVERRIDE(menu_shell, child);}
void gtk_menu_shell_prepend(GtkMenuShell *menu_shell, GtkWidget *child){set_original_function((gpointer*)&gtk_menu_shell_prepend_ORIGINAL,"gtk_menu_shell_prepend"); return gtk_menu_shell_prepend_OVERRIDE(menu_shell, child);}
void gtk_menu_shell_insert(GtkMenuShell *menu_shell, GtkWidget *child, gint position){set_original_function((gpointer*)&gtk_menu_shell_insert_ORIGINAL,"gtk_menu_shell_insert"); return gtk_menu_shell_insert_OVERRIDE(menu_shell, child, position);}
#endif

// =======================================
// SIGNALS / CALLBACKS
// =======================================
g_signal_connect_data_t g_signal_connect_data_ORIGINAL = NULL;
gulong
g_signal_connect_data(gpointer instance,
                      const gchar *detailed_signal,
                      GCallback c_handler,
                      gpointer data,
                      GClosureNotify destroy_data,
                      GConnectFlags connect_flags)
{
    set_original_function((gpointer*)&g_signal_connect_data_ORIGINAL, "g_signal_connect_data");
    return g_signal_connect_data_OVERRIDE(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}
