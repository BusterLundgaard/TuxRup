#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

//------------------------------------
// GlOBALS
// -----------------------------------
GtkWidget* application_root = NULL; // the first window from the application we are modifying. Should remain constant.
GtkWidget* tuxrup_root = NULL; // the tuxrup window. Should remain constant.
GtkWidget* selected_widget = NULL; //the currently selected window.

typedef void(*gtk_widget_show_all_t)(GtkWidget*);
gtk_widget_show_all_t gtk_widget_show_all_original;


// ------------------------------------
// UTIL
// ------------------------------------

// widget methods
gpointer* get_original_function_pointer(char* name){
	void* p = dlsym(RTLD_NEXT, name);
	if(p == NULL){
		g_print("Could not load function %s\n", name);
		exit(1);
	}
	return p;
}

GtkWidget* make_scrolled_window(int width, int height){
	GtkAdjustment* h_adj = gtk_adjustment_new(0,0,0,0,0,0);
	GtkAdjustment* v_adj = gtk_adjustment_new(0,0,0,0,0,0);
	GtkWidget* scrolled_window = gtk_scrolled_window_new(v_adj, h_adj);
	gtk_widget_set_size_request(scrolled_window, 200, 500);
	gtk_scrolled_window_set_policy(
			GTK_SCROLLED_WINDOW(scrolled_window), 
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	return scrolled_window;
}

void empty_box(GtkWidget* box){
	GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(box));
    for (iter = children; iter; iter = g_list_next(iter)) {
        gtk_container_remove(GTK_CONTAINER(box), GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}


// CSS methods
void add_class_to_widget(GtkWidget* widget, char* class){
	GtkStyleContext* context = gtk_widget_get_style_context(widget);
	gtk_style_context_add_class(context, class);
}
void remove_class_from_widget(GtkWidget* widget, char* class){
	GtkStyleContext* context = gtk_widget_get_style_context(widget);
	gtk_style_context_remove_class(context, class);
}
bool contains_class(GtkWidget* widget, char* class_name){
	GtkStyleContext* context = gtk_widget_get_style_context(widget);
	GList* classes = gtk_style_context_list_classes(context);
	for(GList* elem = classes; elem; elem = elem->next){
        if (g_strcmp0(elem->data, class_name) == 0) {
            return true;
        }
    }
	return false;
}

void apply_css(char* css_string){
	GtkCssProvider *provider = gtk_css_provider_new();    
    gtk_css_provider_load_from_data(provider, css_string, -1, NULL); 
    GdkScreen *screen = gtk_widget_get_screen(application_root);
    gtk_style_context_add_provider_for_screen(screen,
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider); 
}


// Detect and convert widget types
bool observed_type(GtkWidget* widget){
	return 
		GTK_IS_BUTTON(widget) ||
		GTK_IS_ENTRY(widget) ||
		GTK_IS_TEXT_BUFFER(widget) ||
		GTK_IS_CHECK_BUTTON(widget) ||
		GTK_IS_TOGGLE_BUTTON(widget) ||
		GTK_IS_SPIN_BUTTON(widget) ||
		GTK_IS_SCALE(widget) ||
		GTK_IS_COMBO_BOX(widget) ||
		GTK_IS_COMBO_BOX_TEXT(widget);
}

char* get_widget_type_string(GtkWidget* widget){
	if      (GTK_IS_CHECK_BUTTON(widget))  {return "check_button";} 
	else if (GTK_IS_TOGGLE_BUTTON(widget)) {return "toggle_button";} 
	else if (GTK_IS_SPIN_BUTTON(widget))   {return "spin_button";} 
	else if (GTK_IS_BUTTON(widget))        {return "button";}
	else if (GTK_IS_LABEL(widget))		   {return "label";}
	else if (GTK_IS_ENTRY(widget))         {return "entry";} 
	else if (GTK_IS_TEXT_BUFFER(widget))   {return "text_buffer";} 
	else if (GTK_IS_SCALE(widget))         {return "scale";} 
	else if (GTK_IS_COMBO_BOX_TEXT(widget)){return "combo_box_text";}
	else if (GTK_IS_COMBO_BOX(widget))     {return "combo_box";} 
	return "";
}

char* get_widget_label(GtkWidget* widget){
	char* label = NULL;
	if(GTK_IS_BUTTON(widget)){ 
		label = gtk_button_get_label(GTK_BUTTON(widget));	
	}
	if(GTK_IS_LABEL(widget)){
		label = gtk_label_get_label(GTK_LABEL(widget));
	}
	
	return label ? label : "";
}


// ----------------------------------------------------------------
// CATCHING ADDED WIDGETS
// In GTK3, you mostly add widgets by using gtk_container_add, so we get widgets we hooking into this function and picking them up
// Theres uhhh, a few other methods in GTK3 for adding widgets, so we hook into those two
// Hence the large block of code: It's just a bunch of addition functions we all bassicly hook into in the same way.
//
// In the previous version of Tuxrup, we tracked all added widgets and appended them to a hash map of all widgets that we are currently aware exist
// But that means we have more *state*, which is bad. Who knows, this hash map of known widgets might easily get out of sync with the real current status of the DOM!
// So we just track an element as "selected" by adding a class to it directly!
// Then, when we want to refresh and see the current status of which widgets we know, we just say "fuck performance" and run through all widgets on all windows, checking which widgets have this class "selected".
// Big gains: We don't need to keep track of widget deletion anymore, something we had completely forgotten about before

// Instead of tracking litteraly every widget in GTK, we simplify things by tracking only certain types of widgets
// You can view them in the function "observed_type()"
// -----------------------------------------------------------------

void on_widget_right_click(GtkWidget* widget){
	if(selected_widget != NULL){
		remove_class_from_widget(selected_widget, "selected");
	}
	selected_widget = widget;
	add_class_to_widget(selected_widget, "selected");
}
gboolean on_widget_click(GtkWidget* widget, GdkEventButton* event, gpointer user_data){
	// Check that it is actually a right click and not just any click
    if(!(event->type == GDK_BUTTON_PRESS && event->button == 3)){
        return false;
    }
	on_widget_right_click(widget);
}

// Make this widget customizable 
void make_widget_customizable(GtkWidget* widget){
	if(!observed_type(widget)){return;}

	add_class_to_widget(widget, "modifiable");

    gtk_widget_add_events(widget, GDK_BUTTON_PRESS_MASK);
    g_signal_connect_data(widget, "button-press-event", G_CALLBACK(on_widget_click), NULL, NULL, (GConnectFlags)0);
}

// ... make a typedef for the type of the function you want to override
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

// ... define a function storing the original function you are overriding
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

// ... then finally actually override the  function
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
//
// Tuxrup will have very bad human computer interaction: Every time some information is updated/changes, it wont automatically update on the tuxrup window. You have to manually press "refresh" for us to re-render the information
// This is of course bad for performance and a little annoying, but it is much easier to program this way.
// The two important functions to understand:
// build_tuxrup_window(): builds the window initially and lays out the structure. Only called once
// refresh_tuxrup_window(): fills out all the information we show in the tuxrup window. 
// ----------------------------------------
GtkWidget* refresh_button;

GtkWidget* widgets_overview;
GtkWidget* widget_types;
GtkWidget* widget_names;
GtkWidget* widget_labels;
GtkWidget* widget_pointers;
GtkWidget* widget_has_callbacks;
GtkWidget* widget_callback_names;
GtkWidget* widget_callback_function_names;
GtkWidget* widget_callback_function_pointers;

char* widget_to_string(GtkWidget* widget){
	char* pointer_name = g_strdup_printf("%p", widget);
	char* id_name = gtk_widget_get_name(widget); 
	char* label = get_widget_label(widget);  
	char* widget_type = get_widget_type_string(widget); 
	return g_strdup_printf("%s; %s; %s; %s", widget_type, id_name, label, pointer_name);
}

void find_all_modifiable_children(GtkWidget* widget, GList** widgets){
	if(!GTK_IS_WIDGET(widget)){return;}

	if(contains_class(widget, "modifiable")){
		*widgets = g_list_append(*widgets, widget);
	}
	
	if(!GTK_IS_CONTAINER(widget)){return;}

	GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
	for (GList *l = children; l; l = l->next){
		find_all_modifiable_children(GTK_WIDGET(l->data), widgets);
	}
	g_list_free(children);  
}

GList* find_all_modifiable_widgets(){
	GList* application_windows = gtk_application_get_windows(gtk_window_get_application(GTK_WINDOW(application_root)));
	GList* widgets = NULL; 
	for(GList* elem = application_windows; elem; elem = elem->next){
		GtkWidget* root_widget = GTK_WIDGET((GtkWindow*)elem->data);
		find_all_modifiable_children(root_widget, &widgets);
	}
	return widgets;
}

GtkWidget* create_overview_label(char* label_str){
	GtkWidget* label = gtk_label_new(label_str);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_widget_set_hexpand(label, false);
	return label;
}

void refresh_widgets_overview(){
	empty_box(widget_types);
    empty_box(widget_names);
    empty_box(widget_labels );
	empty_box(widget_pointers);

	GList* widgets = find_all_modifiable_widgets();
	for(GList* elem = widgets; elem; elem=elem->next){
		GtkWidget* widget = (GtkWidget*)elem->data;
		GtkWidget* label = create_overview_label(get_widget_type_string(widget));
		gtk_container_add_original(GTK_CONTAINER(widget_types),    label);
		/* gtk_container_add_original(GTK_CONTAINER(widget_types),    create_overview_label(get_widget_type_string(widget))); */
		gtk_container_add_original(GTK_CONTAINER(widget_names),    create_overview_label(gtk_widget_get_name(widget)));
		gtk_container_add_original(GTK_CONTAINER(widget_labels),   create_overview_label(get_widget_label(widget)));
		gtk_container_add_original(GTK_CONTAINER(widget_pointers), create_overview_label(g_strdup_printf("%p", widget)));
	}
}


void refresh_tuxrup_window(){
	refresh_widgets_overview();
	//other stuff here
	gtk_widget_show_all_original(tuxrup_root);
}

void build_tuxrup_window(){
	gtk_window_set_title(GTK_WINDOW(tuxrup_root), "Tuxrup");	

	GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	refresh_button = gtk_button_new_with_label("REFRESH");	
	g_signal_connect(refresh_button, "clicked", G_CALLBACK(refresh_tuxrup_window), NULL);

	gtk_container_add_original(GTK_CONTAINER(tuxrup_root), box);
	gtk_container_add_original(GTK_CONTAINER(box), refresh_button);

	GtkWidget* widgets_overview_scrolled_window = make_scrolled_window(500, 500); 
	gtk_container_add_original(GTK_CONTAINER(box), widgets_overview_scrolled_window);	
	widgets_overview = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	widget_types                      = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_names                      = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_labels                     = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_pointers                   = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	/* widget_has_callbacks              = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); */
	/* widget_callback_names             = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); */
	/* widget_callback_function_names    = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); */
	/* widget_callback_function_pointers = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); */
	gtk_container_add_original(GTK_CONTAINER(widgets_overview_scrolled_window), widgets_overview);
	gtk_container_add_original(GTK_CONTAINER(widgets_overview), widget_types   );
	gtk_container_add_original(GTK_CONTAINER(widgets_overview), widget_names   );
	gtk_container_add_original(GTK_CONTAINER(widgets_overview), widget_labels  );
	gtk_container_add_original(GTK_CONTAINER(widgets_overview), widget_pointers);
	gtk_widget_set_margin_right(widget_types,    10);
	gtk_widget_set_margin_right(widget_names,    10);
	gtk_widget_set_margin_right(widget_labels,   10);
	gtk_widget_set_margin_right(widget_pointers, 10);

	/* gtk_container_add_original(GTK_CONTAINER(widgets_overview), widget_has_callbacks             ); */
	/* gtk_container_add_original(GTK_CONTAINER(widgets_overview), widget_callback_names            ); */
	/* gtk_container_add_original(GTK_CONTAINER(widgets_overview), widget_callback_function_names   ); */
	/* gtk_container_add_original(GTK_CONTAINER(widgets_overview), widget_callback_function_pointers); */
}


// --------------------------------------------
// INITIALIZATION
// ---------------------------------------------
bool initialized = false;

// This function is called as the very first function, ie. before ANYTHING else. Can't rely on any kind of GTK context, since GTK hasn't even run it's application yet.
__attribute__((constructor))
void pre_init(){
}

// This function is run right BEFORE the application shows its first window
void init(){
	
}

// This function is called right AFTER the application shows its first window
void post_init(){
	apply_css(".selected{background: blue} \n .debug{background: red}");
}

void gtk_widget_show_all(GtkWidget *widget)
{
	if(!initialized){
		initialized = true;
		application_root = widget;
		GtkApplication* app = gtk_window_get_application(GTK_WINDOW(gtk_widget_get_toplevel(widget)));
		gtk_widget_show_all_original = (gtk_widget_show_all_t)get_original_function_pointer("gtk_widget_show_all");
		
		init();
		tuxrup_root = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		build_tuxrup_window();
		gtk_widget_show_all_original(tuxrup_root);
		gtk_widget_show_all_original(widget);	
		post_init();
	} else {
		gtk_widget_show_all_original(widget);	
	}
}


// ------------------------------------------------------------------
// TESTS
// The test functions are called *from* the testing GTK applications (currently only tests/test1/test1)
// We do this by having the test applictations call undefined functions like tuxrup_test1, then have "empty" implementations of this in a shared library.
// This way, when we run for example tests/test1/test1 with LD_PRELOAD, it takes the implementation of tuxrup_test1() from Tuxrup. 
// Kind of complicated and confusing but moral of story: If you wanna make a new test, have it be called from the application you are testing
// ----------------------------------------------------------------

// Test: "There are 6 elements in tests/test1 that will be added to the list of widgets"
bool exit_if_false(bool result, char* expected, char* got, int test_number){
	if(!result){
		g_print("test %d failed. Expected: %s, got %s\n", test_number, expected, got);	
		exit(1);
	}
	g_print("test %d PASSED.\n", test_number);
	return true;
}

bool tuxrup_test1(){
	GList* widgets = find_all_modifiable_widgets();
	int count = g_list_length(widgets);

	return exit_if_false(
		count == 6,
		"6",
		g_strdup_printf("%d", count),
	   1	
	);
}

// Test: "The 6 added elements have these specific names" 
bool tuxrup_test2(){
	char* names[6] = {
		"GtkButton",
		"GtkCheckButton",
		"buster",
		"GtkSpinButton",
		"GtkEntry",
		"super cool dropdown"
	};
	GList* widgets = find_all_modifiable_widgets();

	GList* elem = widgets;
	for(int i = 0; i < 6 && elem != NULL; i++, elem = elem->next){
		char* name = gtk_widget_get_name(elem->data); 
		if(strcmp(name, names[i]) != 0){
			g_print("test 2 failed. Expected: \"%s\", got \"%s\".\n", names[i], name);	
			exit(1);
		}
	}

	g_print("test 2 PASSED.\n");
	return true;
}
