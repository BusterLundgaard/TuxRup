#ifndef HOOKS_H
#define HOOKS_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif
#include "../globals.h"
#include "../callback_map.h"

// Initialization hooks
typedef GtkApplication* (*gtk_application_new_t)(const char* application_id, GApplicationFlags flags);
extern gtk_application_new_t gtk_application_new_ORIGINAL;

typedef int (*g_application_run_t)(GApplication* application, int argc, char** argv);
extern g_application_run_t g_application_run_ORIGINAL;

typedef void (*gtk_window_present_t)(GtkWindow *window);
extern gtk_window_present_t gtk_window_present_ORIGINAL;

typedef void (*gtk_widget_show_all_t)(GtkWidget* widget);
extern gtk_widget_show_all_t gtk_widget_show_all_ORIGINAL;

// CSS hooks
#ifdef USE_GTK3
typedef gboolean(*gtk_css_provider_load_from_file_t)(GtkCssProvider* provider, GFile* file, GError** err);
#else
typedef void(*gtk_css_provider_load_from_file_t)(GtkCssProvider* provider, GFile* file);
#endif
extern gtk_css_provider_load_from_file_t gtk_css_provider_load_from_file_ORIGINAL;

// Widget adding hooks
#ifdef USE_GTK3
typedef void (*gtk_container_add_t)(GtkContainer *container, GtkWidget *widget);
typedef void (*gtk_box_pack_start_t)(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
typedef void (*gtk_box_pack_end_t)(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
typedef void (*gtk_box_insert_child_after_t)(GtkBox *box, GtkWidget *child, GtkWidget *sibling) ;
typedef void (*gtk_grid_attach_t)(GtkGrid *grid, GtkWidget *child, gint left, gint top, gint width, gint height);
typedef void (*gtk_grid_attach_next_to_t)(GtkGrid *grid, GtkWidget *child, GtkWidget *sibling, GtkPositionType side, gint width, gint height);
typedef void (*gtk_fixed_put_t)(GtkFixed *fixed, GtkWidget *widget, gint x, gint y);
typedef void (*gtk_notebook_append_page_t)(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label);
typedef void (*gtk_notebook_insert_page_t)(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label, gint position);
typedef void (*gtk_paned_pack1_t)(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink);
typedef void (*gtk_paned_pack2_t)(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink);
typedef void (*gtk_scrolled_window_add_with_viewport_t)(GtkScrolledWindow *scrolled_window, GtkWidget *child);
typedef void (*gtk_overlay_add_overlay_t)(GtkOverlay *overlay, GtkWidget *widget);
typedef void (*gtk_stack_add_titled_t)(GtkStack *stack, GtkWidget *child, const gchar *name, const gchar *title);
typedef void (*gtk_stack_add_named_t)(GtkStack *stack, GtkWidget *child, const gchar *name);
typedef void (*gtk_header_bar_pack_start_t)(GtkHeaderBar *bar, GtkWidget *child);
typedef void (*gtk_header_bar_pack_end_t)(GtkHeaderBar *bar, GtkWidget *child);
typedef void (*gtk_menu_shell_append_t)(GtkMenuShell *menu_shell, GtkWidget *child);
typedef void (*gtk_menu_shell_prepend_t)(GtkMenuShell *menu_shell, GtkWidget *child);
typedef void (*gtk_menu_shell_insert_t)(GtkMenuShell *menu_shell, GtkWidget *child, gint position);

extern gtk_container_add_t gtk_container_add_ORIGINAL;
extern gtk_box_pack_start_t gtk_box_pack_start_ORIGINAL;
extern gtk_box_pack_end_t gtk_box_pack_end_ORIGINAL;
extern gtk_box_insert_child_after_t gtk_box_insert_child_after_ORIGINAL;
extern gtk_grid_attach_t gtk_grid_attach_ORIGINAL;
extern gtk_grid_attach_next_to_t gtk_grid_attach_next_to_ORIGINAL;
extern gtk_fixed_put_t gtk_fixed_put_ORIGINAL;
extern gtk_notebook_append_page_t gtk_notebook_append_page_ORIGINAL;
extern gtk_notebook_insert_page_t gtk_notebook_insert_page_ORIGINAL;
extern gtk_paned_pack1_t gtk_paned_pack1_ORIGINAL;
extern gtk_paned_pack2_t gtk_paned_pack2_ORIGINAL;
extern gtk_scrolled_window_add_with_viewport_t gtk_scrolled_window_add_with_viewport_ORIGINAL;
extern gtk_overlay_add_overlay_t gtk_overlay_add_overlay_ORIGINAL;
extern gtk_stack_add_titled_t gtk_stack_add_titled_ORIGINAL;
extern gtk_stack_add_named_t gtk_stack_add_named_ORIGINAL;
extern gtk_header_bar_pack_start_t gtk_header_bar_pack_start_ORIGINAL;
extern gtk_header_bar_pack_end_t gtk_header_bar_pack_end_ORIGINAL;
extern gtk_menu_shell_append_t gtk_menu_shell_append_ORIGINAL;
extern gtk_menu_shell_prepend_t gtk_menu_shell_prepend_ORIGINAL;
extern gtk_menu_shell_insert_t gtk_menu_shell_insert_ORIGINAL;
#endif

// Signal hooks
typedef gulong (*g_signal_connect_data_t)(gpointer instance,
                                          const gchar *detailed_signal,
                                          GCallback c_handler,
                                          gpointer data,
                                          GClosureNotify destroy_data,
                                          GConnectFlags connect_flags);
extern g_signal_connect_data_t g_signal_connect_data_ORIGINAL;

#endif
