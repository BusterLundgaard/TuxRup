#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "util.h"

//------------------------------------
// GlOBALS
// -----------------------------------
GtkWidget* application_root = NULL; // the first window from the application we are modifying. Should remain constant.
GtkWidget* tuxrup_root = NULL; // the tuxrup window. Should remain constant.   
GtkWidget* selected_widget = NULL; //the currently selected widget.

// -----------------------------------
// ORIGINAL (NON-OVERRIDEN) FUNCTIONS: 
// -----------------------------------
typedef void(*gtk_widget_show_all_t)(GtkWidget*);
gtk_widget_show_all_t gtk_widget_show_all_original;

// ------------------------------------
// UTIL
// ------------------------------------
// idget methods:
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

	if(observed_type(widget)){
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
		gtk_container_add(GTK_CONTAINER(widget_types),    label);
		/* gtk_container_add(GTK_CONTAINER(widget_types),    create_overview_label(get_widget_type_string(widget))); */
		gtk_container_add(GTK_CONTAINER(widget_names),    create_overview_label(gtk_widget_get_name(widget)));
		gtk_container_add(GTK_CONTAINER(widget_labels),   create_overview_label(get_widget_label(widget)));
		gtk_container_add(GTK_CONTAINER(widget_pointers), create_overview_label(g_strdup_printf("%p", widget)));
	}
}


void refresh_tuxrup_window(){
	refresh_widgets_overview();
	//other stuff here
	gtk_widget_show_all_original(tuxrup_root);
}

void build_tuxrup_window(){
	gtk_window_set_title(GTK_WINDOW(tuxrup_root), "Tuxrup");	

	GtkWidget* columns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(tuxrup_root), columns);

	// ------------------------------------------
	// data colum
	GtkWidget* data_column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_hexpand(data_column, true);
	gtk_container_add(GTK_CONTAINER(columns), data_column);

	refresh_button = gtk_button_new_with_label("REFRESH");	
	g_signal_connect(refresh_button, "clicked", G_CALLBACK(refresh_tuxrup_window), NULL);

	gtk_container_add(GTK_CONTAINER(data_column), refresh_button);

	// TODO: Does not respect the size right now
	GtkWidget* widgets_overview_scrolled_window = make_scrolled_window(20, 100); 
	gtk_box_pack_start(GTK_BOX(data_column), widgets_overview_scrolled_window, TRUE, TRUE, 0);
	widgets_overview = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	widget_types                      = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_names                      = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_labels                     = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_pointers                   = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	/* widget_has_callbacks              = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); */
	/* widget_callback_names             = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); */
	/* widget_callback_function_names    = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); */
	/* widget_callback_function_pointers = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); */
	gtk_container_add(GTK_CONTAINER(widgets_overview_scrolled_window), widgets_overview);
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_types   );
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_names   );
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_labels  );
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_pointers);
	gtk_widget_set_margin_right(widget_types,    10);
	gtk_widget_set_margin_right(widget_names,    10);
	gtk_widget_set_margin_right(widget_labels,   10);
	gtk_widget_set_margin_right(widget_pointers, 10);
	/* gtk_container_add(GTK_CONTAINER(widgets_overview), widget_has_callbacks             ); */
	/* gtk_container_add(GTK_CONTAINER(widgets_overview), widget_callback_names            ); */
	/* gtk_container_add(GTK_CONTAINER(widgets_overview), widget_callback_function_names   ); */
	/* gtk_container_add(GTK_CONTAINER(widgets_overview), widget_callback_function_pointers); */
	
	//---------------------------------------------------------------------------------
	// Change widget properties
	GtkWidget* change_properties_column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_hexpand(change_properties_column, true);

	refresh_button = gtk_button_new_with_label("Change widget properties");	
	gtk_container_add(GTK_CONTAINER(change_properties_column), refresh_button);

	GtkWidget* property_editor_scrolled_window = make_scrolled_window(100, 500); 
	gtk_container_add(GTK_CONTAINER(columns), change_properties_column);

	GtkWidget* property_editor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(change_properties_column), property_editor_scrolled_window);
	gtk_container_add(GTK_CONTAINER(property_editor_scrolled_window), property_editor);
	
	for(int i = 0; i < 100; i++){
		GtkWidget* label = gtk_label_new("wow i am a label");
		gtk_container_add(GTK_CONTAINER(property_editor), label);
	}

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
	apply_css(".selected{background: blue} \n .debug{background: red}", application_root);
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

// TODO: might be worth trying to change the tests to keep on running even if one fails;

// Test: "There are 8 elements in tests/test1 that will be added to the list of widgets"
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
		count == 8,
		"8",
		g_strdup_printf("%d", count),
	   1	
	);
}

// Test: "The 8 added elements have these specific names" 
bool tuxrup_test2(){
	char* names[8] = {
		"GtkButton",
		"GtkCheckButton",
		"buster",
		"GtkSpinButton",
		"GtkEntry",
		"super cool dropdown",
		"GtkButton",
		"GtkButton"
	};
	GList* widgets = find_all_modifiable_widgets();

	GList* elem = widgets;
	for(int i = 0; i < 8 && elem != NULL; i++, elem = elem->next){
		char* name = gtk_widget_get_name(elem->data); 
		if(strcmp(name, names[i]) != 0){
			g_print("test 2 failed. Expected: \"%s\", got \"%s\".\n", names[i], name);	
			exit(1);
		}
	}

	g_print("test 2 PASSED.\n");
	return true;
}


bool tuxrup_testBangladeshLable() {
	GList* widgets = find_all_modifiable_widgets();

	GList* elem = widgets;
	for(int i = 0; i < g_list_length(widgets); i++, elem = elem->next) {
		if(strcmp(get_widget_label(elem->data),"Bangladesh") == 0) {
			g_print("test 3 PASSED\n");
			return true;
		}
	}
	g_print("test 3 failed. Expected: \"%s\", got \"%s\".\n", "Bangladesh", "None");	
	exit(1);
	return false;
}
