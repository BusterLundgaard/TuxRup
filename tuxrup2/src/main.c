#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

//------------------------------------
// GlOBALS
// -----------------------------------
GtkApplication* app = NULL;

// ------------------------------------
// UTIL
// ------------------------------------
gpointer* get_original_function_pointer(char* name){
	void* p = dlsym(RTLD_NEXT, name);
	if(p == NULL){
		g_print("Could not load function %s\n", name);
		exit(1);
	}
	return p;
}

// ----------------------------------------------------------------
// CATCHING ADDED WIDGETS
// -----------------------------------------------------------------

// Make this widget customizable 
void make_widget_customizable(GtkWidget* widget){
	g_print("adding widget %p\n", widget);
}

typedef void (*gtk_container_add_t)(GtkContainer *container, GtkWidget *widget);
/* typedef void (*gtk_box_pack_start_t)(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding); */
/* typedef void (*gtk_box_pack_end_t)(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding); */
/* typedef void (*gtk_box_insert_child_after_t)(GtkBox *box, GtkWidget *child, GtkWidget *sibling) ; */
/* typedef void (*gtk_grid_attach_t)(GtkGrid *grid, GtkWidget *child, gint left, gint top, gint width, gint height); */
/* typedef void (*gtk_grid_attach_next_to_t)(GtkGrid *grid, GtkWidget *child, GtkWidget *sibling, GtkPositionType side, gint width, gint height); */
/* typedef void (*gtk_fixed_put_t)(GtkFixed *fixed, GtkWidget *widget, gint x, gint y); */
/* typedef gint (*gtk_notebook_append_page_t)(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label); */
/* typedef gint (*gtk_notebook_insert_page_t)(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label, gint position); */
/* typedef void (*gtk_paned_pack1_t)(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink); */
/* typedef void (*gtk_paned_pack2_t)(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink); */
/* typedef void (*gtk_scrolled_window_add_with_viewport_t)(GtkScrolledWindow *scrolled_window, GtkWidget *child); */
/* typedef void (*gtk_overlay_add_overlay_t)(GtkOverlay *overlay, GtkWidget *widget); */
/* typedef void (*gtk_stack_add_titled_t)(GtkStack *stack, GtkWidget *child, const gchar *name, const gchar *title); */
/* typedef void (*gtk_stack_add_named_t)(GtkStack *stack, GtkWidget *child, const gchar *name); */
/* typedef void (*gtk_header_bar_pack_start_t)(GtkHeaderBar *bar, GtkWidget *child); */
/* typedef void (*gtk_header_bar_pack_end_t)(GtkHeaderBar *bar, GtkWidget *child); */
/* typedef void (*gtk_menu_shell_append_t)(GtkMenuShell *menu_shell, GtkWidget *child); */
/* typedef void (*gtk_menu_shell_prepend_t)(GtkMenuShell *menu_shell, GtkWidget *child); */
/* typedef void (*gtk_menu_shell_insert_t)(GtkMenuShell *menu_shell, GtkWidget *child, gint position); */

gtk_container_add_t gtk_container_add_original;
/* gtk_box_pack_start_t gtk_box_pack_start_original; */
/* gtk_box_pack_end_t gtk_box_pack_end_original; */
/* gtk_box_insert_child_after_t gtk_box_insert_child_after_original; */
/* gtk_grid_attach_t gtk_grid_attach_original; */
/* gtk_grid_attach_next_to_t gtk_grid_attach_next_to_original; */
/* gtk_fixed_put_t gtk_fixed_put_original; */
/* gtk_notebook_append_page_t gtk_notebook_append_page_original; */
/* gtk_notebook_insert_page_t gtk_notebook_insert_page_original; */
/* gtk_paned_pack1_t gtk_paned_pack1_original; */
/* gtk_paned_pack2_t gtk_paned_pack2_original; */
/* gtk_scrolled_window_add_with_viewport_t gtk_scrolled_window_add_with_viewport_original; */
/* gtk_overlay_add_overlay_t gtk_overlay_add_overlay_original; */
/* gtk_stack_add_titled_t gtk_stack_add_titled_original; */
/* gtk_stack_add_named_t gtk_stack_add_named_original; */
/* gtk_header_bar_pack_start_t gtk_header_bar_pack_start_original; */
/* gtk_header_bar_pack_end_t gtk_header_bar_pack_end_original; */
/* gtk_menu_shell_append_t gtk_menu_shell_append_original; */
/* gtk_menu_shell_prepend_t gtk_menu_shell_prepend_original; */
/* gtk_menu_shell_insert_t gtk_menu_shell_insert_original; */

void gtk_container_add(GtkContainer *container, GtkWidget *child){make_widget_customizable(child); gtk_container_add_original = (gtk_container_add_t)get_original_function_pointer("gtk_container_add"); gtk_container_add_original(container, child);}
/* void gtk_box_pack_start(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding){make_widget_customizable(child);gtk_box_pack_start_original = (gtk_box_pack_start_t)get_original_function_pointer("gtk_box_pack_start");gtk_box_pack_start_original(box, child, expand, fill, padding);} */
/* void gtk_box_pack_end(GtkBox *box, GtkWidget *child, gboolean expand, gboolean fill, guint padding){make_widget_customizable(child);gtk_box_pack_end_original =  (gtk_box_pack_end_t)get_original_function_pointer("gtk_box_pack_end");gtk_box_pack_end_original(box, child, expand, fill, padding);} */
/* void gtk_box_insert_child_after(GtkBox *box, GtkWidget *child, GtkWidget *sibling) {make_widget_customizable(child);gtk_box_insert_child_after_original =  (gtk_box_insert_child_after_t)get_original_function_pointer("gtk_box_insert_child_after");gtk_box_insert_child_after_original(box, child, sibling);} */
/* void gtk_grid_attach(GtkGrid *grid, GtkWidget *child, gint left, gint top, gint width, gint height){make_widget_customizable(child);gtk_grid_attach_original =  (gtk_grid_attach_t)get_original_function_pointer("gtk_grid_attach");gtk_grid_attach_original(grid, child, left, top, width, height);} */
/* void gtk_grid_attach_next_to(GtkGrid *grid, GtkWidget *child, GtkWidget *sibling, GtkPositionType side, gint width, gint height){make_widget_customizable(child);gtk_grid_attach_next_to_original =  (gtk_grid_attach_next_to_t)get_original_function_pointer("gtk_grid_attach_next_to");gtk_grid_attach_next_to_original(grid, child, sibling, side, width, height);} */
/* void gtk_fixed_put(GtkFixed *fixed, GtkWidget *widget, gint x, gint y){make_widget_customizable(widget);gtk_fixed_put_original =  (gtk_fixed_put_t)get_original_function_pointer("gtk_fixed_put");gtk_fixed_put_original(fixed, widget, x, y);} */
/* gint gtk_notebook_append_page(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label){make_widget_customizable(child);gtk_notebook_append_page_original =  (gtk_notebook_append_page_t)get_original_function_pointer("gtk_notebook_append_page");gtk_notebook_append_page_original(notebook, child, tab_label);} */
/* gint gtk_notebook_insert_page(GtkNotebook *notebook, GtkWidget *child, GtkWidget *tab_label, gint position){make_widget_customizable(child);gtk_notebook_insert_page_original =  (gtk_notebook_insert_page_t)get_original_function_pointer("gtk_notebook_insert_page");gtk_notebook_insert_page_original(notebook, child, tab_label, position);} */
/* void gtk_paned_pack1(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink){make_widget_customizable(child);gtk_paned_pack1_original =  (gtk_paned_pack1_t)get_original_function_pointer("gtk_paned_pack1");gtk_paned_pack1_original(paned, child, resize, shrink);} */
/* void gtk_paned_pack2(GtkPaned *paned, GtkWidget *child, gboolean resize, gboolean shrink){make_widget_customizable(child);gtk_paned_pack2_original =  (gtk_paned_pack2_t)get_original_function_pointer("gtk_paned_pack2");gtk_paned_pack2_original(paned, child, resize, shrink);} */
/* void gtk_scrolled_window_add_with_viewport(GtkScrolledWindow *scrolled_window, GtkWidget *child){make_widget_customizable(child);gtk_scrolled_window_add_with_viewport_original =  (gtk_scrolled_window_add_with_viewport_t)get_original_function_pointer("gtk_scrolled_window_add_with_viewport");gtk_scrolled_window_add_with_viewport_original(scrolled_window, child);} */
/* void gtk_overlay_add_overlay(GtkOverlay *overlay, GtkWidget *child){make_widget_customizable(child);gtk_overlay_add_overlay_original =  (gtk_overlay_add_overlay_t)get_original_function_pointer("gtk_overlay_add_overlay");gtk_overlay_add_overlay_original(overlay, child);} */
/* void gtk_stack_add_titled(GtkStack *stack, GtkWidget *child, const gchar *name, const gchar *title){make_widget_customizable(child);gtk_stack_add_titled_original =  (gtk_stack_add_titled_t)get_original_function_pointer("gtk_stack_add_titled");gtk_stack_add_titled_original(stack, child, name, title);} */
/* void gtk_stack_add_named(GtkStack *stack, GtkWidget *child, const gchar *name){make_widget_customizable(child);gtk_stack_add_named_original =  (gtk_stack_add_named_t)get_original_function_pointer("gtk_stack_add_named");gtk_stack_add_named_original(stack, child, name);} */
/* void gtk_header_bar_pack_start(GtkHeaderBar *bar, GtkWidget *child){make_widget_customizable(child);gtk_header_bar_pack_start_original =  (gtk_header_bar_pack_start_t)get_original_function_pointer("gtk_header_bar_pack_start");gtk_header_bar_pack_start_original(bar, child);} */
/* void gtk_header_bar_pack_end(GtkHeaderBar *bar, GtkWidget *child){make_widget_customizable(child);gtk_header_bar_pack_end_original =  (gtk_header_bar_pack_end_t)get_original_function_pointer("gtk_header_bar_pack_end");gtk_header_bar_pack_end_original(bar, child);} */
/* void gtk_menu_shell_append(GtkMenuShell *menu_shell, GtkWidget *child){make_widget_customizable(child);gtk_menu_shell_append_original =  (gtk_menu_shell_append_t)get_original_function_pointer("gtk_menu_shell_append");gtk_menu_shell_append_original(menu_shell, child);} */
/* void gtk_menu_shell_prepend(GtkMenuShell *menu_shell, GtkWidget *child){make_widget_customizable(child);gtk_menu_shell_prepend_original =  (gtk_menu_shell_prepend_t)get_original_function_pointer("gtk_menu_shell_prepend");gtk_menu_shell_prepend_original(menu_shell, child);} */
/* void gtk_menu_shell_insert(GtkMenuShell *menu_shell, GtkWidget *child, gint position){make_widget_customizable(child);gtk_menu_shell_insert_original =  (gtk_menu_shell_insert_t)get_original_function_pointer("gtk_menu_shell_insert");gtk_menu_shell_insert_original(menu_shell, child, position);} */


// -----------------------------------------
// CREATING AND REFRESHING THE TUXRUP WINDOW
// ----------------------------------------
void refresh(GtkWidget* original_window, GtkWidget* tuxrup_window){
	gtk_window_set_title(GTK_WINDOW(tuxrup_window), "Tuxrup");	

	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	GtkWidget* button = gtk_button_new_with_label("wow");	
	
	gtk_container_add_original(GTK_CONTAINER(tuxrup_window), box);
	gtk_container_add_original(GTK_CONTAINER(box), button);
}

// --------------------------------------------
// INITIALIZATION
// ---------------------------------------------
typedef void(*gtk_widget_show_all_t)(GtkWidget*);
bool initialized = false;

// This function is called as the very first function, ie. before ANYTHING else. Can't rely on any kind of GTK context, since GTK hasn't even run it's application yet.
__attribute__((constructor))
void pre_init(){
	g_print("hello there!!!\n");
}

// This function is run right before the application shows its first window
void init(){
}

void gtk_widget_show_all(GtkWidget *widget)
{
	if(initialized){return;}

	initialized = true;

	app = gtk_window_get_application(GTK_WINDOW(gtk_widget_get_toplevel(widget)));
	gtk_widget_show_all_t gtk_widget_show_all_original = (gtk_widget_show_all_t)get_original_function_pointer("gtk_widget_show_all");
	
	GtkWidget* tuxrup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	refresh(widget, tuxrup_window); 
	gtk_widget_show_all_original(tuxrup_window);
	
	gtk_widget_show_all_original(widget);	
}


// ------------------------------------------------------------------
// TESTS
// ----------------------------------------------------------------
bool tuxrup_test1(){
	g_print("hello from Tuxrup!!\n");
	return true;
}
bool tuxrup_test2(){
	g_print("second hello to you\n");
	bool result = true;
	if(!result){
		g_print("test 2 failed: expected 2, got 3\n");
		exit(1);
	}
	return true;
}
