#ifndef OVERRIDES_H
#define OVERRIDES_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif
#include "../globals.h"
#include "../callback_map.h"

// Initialization hooks
GtkApplication* gtk_application_new_OVERRIDE(const char* application_id, GApplicationFlags flags);
int g_application_run_OVERRIDE(GApplication* application, int argc, char** argv);
void gtk_window_present_OVERRIDE(GtkWindow *window);
void gtk_widget_show_all_OVERRIDE(GtkWidget* widget);

// CSS hooks
#ifdef USE_GTK3
gboolean gtk_css_provider_load_from_file_OVERRIDE(GtkCssProvider* provider, GFile* file, GError** err);
#else
void gtk_css_provider_load_from_file_OVERRIDE(GtkCssProvider* provider, GFile* file);
#endif

// Widget adding
#ifdef USE_GTK3
void gtk_container_add_OVERRIDE(GtkContainer *container, GtkWidget *widget);
void gtk_box_pack_start_OVERRIDE(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
void gtk_box_pack_end_OVERRIDE(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding);
void gtk_box_insert_child_after_OVERRIDE(GtkBox *box, GtkWidget *child, GtkWidget *sibling) ;
void gtk_grid_attach_OVERRIDE(GtkGrid *grid, GtkWidget *child, gint left, gint top, gint width, gint height);
void gtk_grid_attach_next_to_OVERRIDE(GtkGrid *grid, GtkWidget *child, GtkWidget *sibling, GtkPositionType side, gint width, gint height);
void gtk_fixed_put_OVERRIDE(GtkFixed *fixed, GtkWidget *widget, gint x, gint y);
gint gtk_notebook_append_page_OVERRIDE(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label);
gint gtk_notebook_insert_page_OVERRIDE(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label, gint position);
void gtk_paned_pack1_OVERRIDE(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink);
void gtk_paned_pack2_OVERRIDE(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink);
void gtk_scrolled_window_add_with_viewport_OVERRIDE(GtkScrolledWindow *scrolled_window, GtkWidget *child);
void gtk_overlay_add_overlay_OVERRIDE(GtkOverlay *overlay, GtkWidget *widget);
void gtk_stack_add_titled_OVERRIDE(GtkStack *stack, GtkWidget *child, const gchar *name, const gchar *title);
void gtk_stack_add_named_OVERRIDE(GtkStack *stack, GtkWidget *child, const gchar *name);
void gtk_header_bar_pack_start_OVERRIDE(GtkHeaderBar *bar, GtkWidget *child);
void gtk_header_bar_pack_end_OVERRIDE(GtkHeaderBar *bar, GtkWidget *child);
void gtk_menu_shell_append_OVERRIDE(GtkMenuShell *menu_shell, GtkWidget *child);
void gtk_menu_shell_prepend_OVERRIDE(GtkMenuShell *menu_shell, GtkWidget *child);
void gtk_menu_shell_insert_OVERRIDE(GtkMenuShell *menu_shell, GtkWidget *child, gint position);

#endif

// Signal hooks
gulong g_signal_connect_data_OVERRIDE(gpointer instance,
                                          const gchar *detailed_signal,
                                          GCallback c_handler,
                                          gpointer data,
                                          GClosureNotify destroy_data,
                                          GConnectFlags connect_flags);





#endif
