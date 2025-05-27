#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <gio/gio.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "util.h"
#include "css.h"
#include "globals.h"
#include "properties.h"
#include "io.h"
#include "symbols.h"
#include "callbacks.h"


// TODO: Pressing some of the buttons when you have no widget selected crashes the program

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ORIGINAL (NON-OVERRIDEN) FUNCTIONS: 
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void(*gtk_widget_show_all_t)(GtkWidget*);
// defining local var
gtk_widget_show_all_t gtk_widget_show_all_original;

typedef gboolean(*gtk_css_provider_load_from_file_t)(GtkCssProvider*, GFile*, GError**);
// defining local var with this return type
gtk_css_provider_load_from_file_t gtk_css_provider_load_from_file_original;



// LOCAL VARIABLES

GtkTextBuffer* css_buffer = NULL;
GFile* file_to_css = NULL;
gboolean hasloaded = FALSE;



//-------------------------------------------------------------------
// HELPER FUNCTIONS
//-------------------------------------------------------------------
void set_application_root(GtkWidget* candidate) {
	if (!application_root && GTK_IS_WINDOW(candidate)) {
		application_root = candidate;
		g_debug("application_root set to %p", candidate);
	} else if (application_root && application_root != candidate) {
		application_root = candidate;
		g_warning("overriding applicatoin root. Existing: %p, New: %p", application_root, candidate);
	}
}



gchar* read_gfile(GFile *file, gsize *length, GError **error_out) {
	GFileInputStream *stream = g_file_read(file,NULL,error_out);
	if (!stream) {
		return NULL;
	}
	GDataInputStream *data_stream = g_data_input_stream_new(G_INPUT_STREAM(stream));
	if (!data_stream) {
		return NULL;
	}
	
	GByteArray *buffer = g_byte_array_new();
	guint8 tempbuffer[4096];
	gssize bytes;

	GError *error = NULL;
	while ((bytes = g_input_stream_read(G_INPUT_STREAM(data_stream), tempbuffer, sizeof(tempbuffer), NULL, &error)) > 0) {
        g_byte_array_append(buffer, tempbuffer, bytes);
    }
	if (bytes<0) {
		g_warning("could not read the file with error %s",error->message);
		g_clear_error(&error);
		g_byte_array_free(buffer,TRUE);
		g_object_unref(data_stream);
		g_object_unref(stream);
		return NULL;
	}
	g_object_unref(data_stream);
	g_object_unref(stream);
	g_byte_array_append(buffer, (const guint8 *)"", 1);
    if (length) *length = buffer->len - 1;

    return (gchar *)g_byte_array_free(buffer, FALSE);
}
// could have used set text here
void append_to_gtk_buffer(GtkTextBuffer *buffer, const gchar *text) {
	GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, text, -1);
    gtk_text_buffer_insert(buffer, &end, "\n\n", -1);
}

void read_and_append_to_buffer(GFile *file, GtkTextBuffer *buffer) {
	GError *error = NULL;
	gsize len = 0;
	if(!G_IS_FILE(file)) {
		return;
	}
	gchar *text = read_gfile(file,&len,&error);
	if(text) {
		append_to_gtk_buffer(buffer,text);
		g_free(text);
	}
	else {
		g_warning("could not read file with error %s",error->message);
		g_clear_error(&error);
	}
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// FINDING ALL WIDGETS
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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



// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CREATING AND REFRESHING THE TUXRUP WINDOW
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


GtkWidget* refresh_button;
GtkWidget* widgets_overview;
GtkWidget* widget_types;
GtkWidget* widget_names;
GtkWidget* widget_labels;
GtkWidget* widget_pointers;
GtkWidget* widget_callback_names;
GtkWidget* widget_callback_function_names;
GtkWidget* widget_callback_function_pointers;
GtkWidget* property_editor;
GtkTextBuffer* css_text_buffer;
GtkTextBuffer* callbacks_text_buffer;
GtkWidget* program_src_folder_label;    
GtkWidget* working_directory_label;      
GtkWidget* executable_path_label;        
GtkWidget* executable_name_label;        
GtkWidget* executable_symbols_path_label;
GtkWidget* symbol_names;
GtkWidget* symbol_pointers;

GtkWidget* create_overview_label(char* label_str){
	GtkWidget* label = gtk_label_new(label_str);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	gtk_widget_set_hexpand(label, false);
	return label;
}

void make_widget_customizable(GtkWidget* widget);
void refresh_widgets_overview(){
	empty_box(widget_types);
    empty_box(widget_names);
    empty_box(widget_labels );
	empty_box(widget_pointers);
	empty_box(widget_callback_names);
	empty_box(widget_callback_function_names);
	empty_box(widget_callback_function_pointers);

	GList* widgets = find_all_modifiable_widgets();
	for(GList* elem = widgets; elem; elem=elem->next){
		GtkWidget* widget = (GtkWidget*)elem->data;
		GtkWidget* label_type = create_overview_label(get_widget_type_string(widget));
		if(widget == selected_widget){
			add_class_to_widget(label_type, "selected");
		}
		gtk_container_add(GTK_CONTAINER(widget_types),    label_type); 
		gtk_container_add(GTK_CONTAINER(widget_names),    create_overview_label(gtk_widget_get_name(widget)));
		gtk_container_add(GTK_CONTAINER(widget_pointers), create_overview_label(g_strdup_printf("%p", widget)));
		gtk_container_add(GTK_CONTAINER(widget_labels),   create_overview_label(get_widget_label(widget)));
		
		void* callback_pointer = g_object_get_data(G_OBJECT(widget), "callback_pointer");
		char* callback_name = g_object_get_data(G_OBJECT(widget), "callback_name");

		if(callback_pointer != NULL){
			gtk_container_add(GTK_CONTAINER(widget_callback_names), create_overview_label(callback_name));
			gtk_container_add(GTK_CONTAINER(widget_callback_function_pointers), create_overview_label(g_strdup_printf("%p", callback_pointer)));
			gtk_container_add(GTK_CONTAINER(widget_callback_function_names), create_overview_label(identifier_from_pointer(callback_pointer)));
		}

		if(!g_object_get_data(G_OBJECT(widget),"rightclickable")) {
			make_widget_customizable(widget);
			g_object_set_data(G_OBJECT(widget),"rightclickable", (gpointer)true);
		}

	}
}

void refresh_symbols_overview(){
	empty_box(symbol_names);
	empty_box(symbol_pointers);

	GHashTable* main_symbols = get_main_symbols();
	GHashTableIter iter;
	gpointer key, value;

	g_hash_table_iter_init(&iter, main_symbols);
	while(g_hash_table_iter_next(&iter, &key, &value)){
		char* symbol_name = (char*)key;
		void* symbol_pointer = (void*)value;
		gtk_container_add(GTK_CONTAINER(symbol_names), create_overview_label(symbol_name));  
		gtk_container_add(GTK_CONTAINER(symbol_pointers), create_overview_label(g_strdup_printf("%p", symbol_pointer)));  
	}
}

void read_and_append_to_buffer(GFile *file, GtkTextBuffer *buffer); 

void refreshallcss() {
	if(hasloaded == TRUE) {
		return;
	}
	hasloaded = TRUE;
	if(!G_IS_FILE(file_to_css)) {
		g_print("G_IS_FILE says its not a file");
		gtk_text_buffer_set_text(css_buffer, "\n\n\n\n\n\n\n\n\n\n\n\n", -1); //TODO: Make this less bad
	}
	else {
		read_and_append_to_buffer(file_to_css,css_buffer);
	}
}

void fix_application_root() {
	if (!GTK_IS_WINDOW(application_root)) {
		GList* toplevels = gtk_window_list_toplevels();
		for (GList* l = toplevels; l != NULL; l = l->next) {
			if (GTK_IS_WINDOW(l->data)) {
				application_root = GTK_WIDGET(l->data);
				break;
			}
		}
		g_list_free(toplevels);
	
		if (!GTK_IS_WINDOW(application_root)) {
			g_warning("Failed to set application_root in refresh_tuxrup_window.");
			return;
		}
	}
}

// this is the important part, we need to try and grab the application window when refresh is called every time, gotdamn i hate this.
void refresh_tuxrup_window(){
	fix_application_root();
	if(application_root) {refresh_widgets_overview();}

	refresh_symbols_overview();
	
	gtk_label_set_label(GTK_LABEL(program_src_folder_label),      g_strdup_printf("program_src_folder: %s",      get_program_src_folder()));
	gtk_label_set_label(GTK_LABEL(working_directory_label),       g_strdup_printf("working_directory: %s",       get_working_directory()));
	gtk_label_set_label(GTK_LABEL(executable_path_label),         g_strdup_printf("executable_path: %s",         get_executable_path()));
	gtk_label_set_label(GTK_LABEL(executable_name_label),         g_strdup_printf("executable_name: %s",         get_executable_name()));
	gtk_label_set_label(GTK_LABEL(executable_symbols_path_label), g_strdup_printf("executable_symbols_path: %s", get_executable_symbols_path()));

	css_reset(css_text_buffer);
	properties_reset(property_editor);
	callbacks_reset(callbacks_text_buffer);
	refreshallcss();

	gtk_widget_show_all_original(tuxrup_root);
}

void build_tuxrup_window(){
	gtk_window_set_title(GTK_WINDOW(tuxrup_root), "Tuxrup");	

	GtkWidget* columns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_container_add(GTK_CONTAINER(tuxrup_root), columns);

	// ------------------------------------------
	// data column
	GtkWidget* data_column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_hexpand(data_column, true);
	gtk_container_add(GTK_CONTAINER(columns), data_column);

	refresh_button = gtk_button_new_with_label("REFRESH");	
	g_signal_connect(refresh_button, "clicked", G_CALLBACK(refresh_tuxrup_window), NULL);

	gtk_container_add(GTK_CONTAINER(data_column), refresh_button);
	GtkWidget* io_paths_overview = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	program_src_folder_label      = create_overview_label("program_src_folder:");
	working_directory_label       = create_overview_label("working_directory:");
	executable_path_label         = create_overview_label("executable_path:");
	executable_name_label         = create_overview_label("executable_name:");
	executable_symbols_path_label = create_overview_label("executable_symbols_path:");
	gtk_container_add(GTK_CONTAINER(data_column), program_src_folder_label     );
	gtk_container_add(GTK_CONTAINER(data_column), working_directory_label      );
	gtk_container_add(GTK_CONTAINER(data_column), executable_path_label        );
	gtk_container_add(GTK_CONTAINER(data_column), executable_name_label        );
	gtk_container_add(GTK_CONTAINER(data_column), executable_symbols_path_label);
	gtk_container_add(GTK_CONTAINER(data_column), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

	GtkWidget* widgets_overview_scrolled_window = make_scrolled_window(20, 100); 
	gtk_box_pack_start(GTK_BOX(data_column), widgets_overview_scrolled_window, TRUE, TRUE, 0);
	widgets_overview = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	widget_types                      = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_names                      = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_labels                     = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_pointers                   = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_callback_names             = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_callback_function_names    = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	widget_callback_function_pointers = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(widgets_overview_scrolled_window), widgets_overview);
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_types   );
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_names   );
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_labels  );
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_pointers);
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_callback_names            );
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_callback_function_pointers);
	gtk_container_add(GTK_CONTAINER(widgets_overview), widget_callback_function_names   );
	gtk_widget_set_margin_right(widget_types,    10);
	gtk_widget_set_margin_right(widget_names,    10);
	gtk_widget_set_margin_right(widget_labels,   10);
	gtk_widget_set_margin_right(widget_pointers, 10);
	gtk_widget_set_margin_right(widget_callback_names, 10);
	gtk_widget_set_margin_right(widget_callback_function_pointers, 10);
	
	GtkWidget* symbol_label = gtk_label_new("symbols");	
	gtk_box_pack_start(GTK_BOX(data_column), symbol_label, TRUE, TRUE, 0);

	GtkWidget* symbol_overview_scrolled_window = make_scrolled_window(20, 100); 
	gtk_box_pack_start(GTK_BOX(data_column), symbol_overview_scrolled_window, TRUE, TRUE, 0);
	GtkWidget* symbol_overview = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	gtk_container_add(GTK_CONTAINER(symbol_overview_scrolled_window), symbol_overview);
	symbol_names    = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	symbol_pointers = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(symbol_overview), symbol_names   );
	gtk_container_add(GTK_CONTAINER(symbol_overview), symbol_pointers);
	gtk_widget_set_margin_right(symbol_pointers, 10);
	gtk_widget_set_margin_right(symbol_names,    10);

	//---------------------------------------------------------------------------------
	// Change widget properties
	GtkWidget* change_properties_column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	GtkWidget* change_widget_properties_button = gtk_button_new_with_label("Change widget properties");	
	gtk_container_add(GTK_CONTAINER(change_properties_column), change_widget_properties_button);

	GtkWidget* property_editor_scrolled_window = make_scrolled_window(100, 500); 
	gtk_container_add(GTK_CONTAINER(columns), change_properties_column);

	property_editor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(change_properties_column), property_editor_scrolled_window);
	gtk_container_add(GTK_CONTAINER(property_editor_scrolled_window), property_editor);
	g_signal_connect_data_original(change_widget_properties_button, "clicked", G_CALLBACK(on_edit_properties), property_editor, NULL, (GConnectFlags)0); 

	// ---------------------------------------------------------------------------------
	// Change CSS properties
	GtkWidget* change_css_column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(columns), change_css_column);

	GtkWidget* loadcssbutton = gtk_button_new_with_label("Load CSS");
	gtk_container_add(GTK_CONTAINER(change_css_column), loadcssbutton);

	GtkWidget* done = gtk_button_new_with_label("Done");
	gtk_container_add(GTK_CONTAINER(change_css_column), done);

	GtkWidget* css_edit_label = gtk_label_new("Add CSS for widget here");
	gtk_container_add(GTK_CONTAINER(change_css_column), css_edit_label);

	css_text_buffer = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(css_text_buffer, "\n\n\n\n\n\n\n\n\n\n\n\n", -1); //TODO: Make this less bad
	GtkWidget* css_editor = gtk_text_view_new_with_buffer(css_text_buffer);
	gtk_container_add(GTK_CONTAINER(change_css_column), css_editor);
	g_signal_connect(done, "clicked", G_CALLBACK(on_done_clicked), css_text_buffer);

	GtkWidget* css_all_label = gtk_label_new("View all CSS for application here");
	gtk_container_add(GTK_CONTAINER(change_css_column), css_all_label);

	GtkTextBuffer *textbuffer2 = gtk_text_buffer_new(NULL);
	css_buffer = textbuffer2;
	GtkWidget* css_viewer = gtk_text_view_new_with_buffer(textbuffer2);
	gtk_container_add(GTK_CONTAINER(change_css_column), css_viewer);
	g_signal_connect(loadcssbutton, "clicked", G_CALLBACK(on_load), css_text_buffer);


	// --------------------------------------------------------------------------------------
	// Change callback
	GtkWidget* change_callback_column = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_hexpand(change_callback_column, true);
	gtk_container_add(GTK_CONTAINER(columns), change_callback_column);

	GtkWidget* change_callback_button = gtk_button_new_with_label("Edit callback");
	gtk_container_add(GTK_CONTAINER(change_callback_column), change_callback_button);
		
	GtkWidget* callback_editor_scrolled_window = make_scrolled_window(20, 100); 
	gtk_box_pack_start(GTK_BOX(change_callback_column), callback_editor_scrolled_window, TRUE, TRUE, 0);

	callbacks_text_buffer = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(callbacks_text_buffer, "\n\n\n\n\n\n\n\n\n\n\n\n", -1); //TODO: Make this less bad
	GtkWidget* callback_editor = gtk_text_view_new_with_buffer(callbacks_text_buffer);
	gtk_container_add(GTK_CONTAINER(callback_editor_scrolled_window), callback_editor);

	GtkWidget* done_callback_button = gtk_button_new_with_label("Done editing callback");
	gtk_container_add(GTK_CONTAINER(change_callback_column), done_callback_button);

	g_signal_connect_data_original(change_callback_button, "clicked", G_CALLBACK(on_callback_edit), callbacks_text_buffer, NULL, (GConnectFlags)0);
	g_signal_connect_data_original(done_callback_button, "clicked", G_CALLBACK(on_callback_done), callbacks_text_buffer, NULL, (GConnectFlags)0);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// SELECTING WIDGETS WITH RIGHT CLICK
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void on_widget_right_click(GtkWidget* widget){
	
    if(selected_widget != NULL){
        remove_class_from_widget(selected_widget, "selected");
    }
	if(!widget) {
		g_print("right clicked widget not applicable");
		return;
	}
	selected_widget = widget;
    add_class_to_widget(selected_widget, "selected");
	refresh_tuxrup_window();
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
    g_signal_connect_data_original(widget, "button-press-event", G_CALLBACK(on_widget_click), NULL, NULL, (GConnectFlags)0);
}


// ---------------------------------------------
// CALLBACKS
// ---------------------------------------------
gulong g_signal_connect_data(gpointer instance,
                             const gchar *detailed_signal,
                             GCallback c_handler,
                             gpointer data,
                             GClosureNotify destroy_data,
 							 GConnectFlags connect_flags){

	if(!observed_type(instance))
	{goto signal_connect_end;}

	if(!(
		strcmp(detailed_signal, "clicked") ==0 || 
		strcmp(detailed_signal, "activate")==0)
	)
	{goto signal_connect_end;}

	g_print("widget %p passed. Has detailed_signal %s, and name %s.\n", instance, detailed_signal, gtk_widget_get_name(GTK_WIDGET(instance)));
	g_object_set_data(G_OBJECT(instance), "callback_name", detailed_signal); 
	g_object_set_data(G_OBJECT(instance), "callback_pointer", (gpointer)c_handler);
	g_object_set_data(G_OBJECT(instance), "callback_data", data); 

	signal_connect_end:
	g_signal_connect_data_original = (g_signal_connect_data_original != NULL) ? g_signal_connect_data_original : (g_signal_connect_data_t)get_original_function_pointer("g_signal_connect_data");
	return g_signal_connect_data_original(instance, detailed_signal, c_handler, data, destroy_data, connect_flags);
}

// we only want to read the file, and bail out fast.
gboolean gtk_css_provider_load_from_file(GtkCssProvider *css_provider, GFile *file, GError **error){
	gtk_css_provider_load_from_file_original = (gtk_css_provider_load_from_file_original != NULL) ? gtk_css_provider_load_from_file_original : (gtk_css_provider_load_from_file_t)get_original_function_pointer("gtk_css_provider_load_from_file");
	if (G_IS_FILE(file)) {
	if(file_to_css) g_object_unref(file_to_css);
	file_to_css = g_object_ref(file);
	g_print("Updated file to %s",g_file_get_path(file_to_css));
	return gtk_css_provider_load_from_file_original(css_provider,file,error);
	}
	return gtk_css_provider_load_from_file_original(css_provider,file,error);
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
// we should try to get the application root here yes?
//:thinking:;______________________________;
void post_init(){
	char* selected_css = 
	".selected{\
		 background-color:rgb(247, 190, 4);\
   		 color: black;\
   		 font-weight: bold;\
   		 box-shadow: 0 2px 4px rgba(0, 0, 0, 0.4);\
    }";
	apply_css(selected_css, application_root);
	apply_css(selected_css, tuxrup_root);
}

void gtk_widget_show_all(GtkWidget *widget)
{
	if(!initialized){
		initialized = true;
		if(!GTK_IS_WINDOW(widget)) {goto showall;}
		application_root = widget;
		fix_application_root();
		GtkWindow* window = GTK_WINDOW(gtk_widget_get_toplevel(widget));
		if(!window) {goto showall;}
		GtkApplication* app = gtk_window_get_application(window); // when do we use this? This is defined locally in the function but not used here ;____;


		showall:
		gtk_widget_show_all_original = (gtk_widget_show_all_t)get_original_function_pointer("gtk_widget_show_all");
		
		init();
		tuxrup_root = gtk_window_new(GTK_WINDOW_TOPLEVEL); // why is this not just in init ;________;
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

bool tuxrup_test_pointer_name_conversion(){
	int x = *((int*)pointer_from_identifier("x"));
	if(x != 23){
		g_print("test 4 failed. Failed to properly resolve pointer with name \"x\". Expected: 23, got: %d\n", x);	
		exit(1);
	}		

	typedef void(*on_new_clicked_t)(GtkWidget*, gpointer);
	((on_new_clicked_t)pointer_from_identifier("on_new_clicked"))(NULL, NULL);

	g_print("test 4 PASSED.\n");
}
