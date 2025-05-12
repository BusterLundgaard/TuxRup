#include <string.h>
#include "gtk_events.h"
#include "hooks.h"
#include "events_util.h"

#include "../globals.h"
#include "../callback_map.h"
#include "../utilities/hashing.h" 
#include "../utilities/util.h"
#include "../utilities/pointer_name_conversion.h"
#include "../right_click/context_menu.h"
#include "../editing_callbacks/edit_callbacks_window.h"
#include "../function_dispatcher.h"

#include <sys/stat.h>

void set_program_name(){
	char* executable_path_copy = g_strdup(executable_path);
    program_name = g_basename(executable_path_copy);
}
void set_program_src_code(){
	for(int i = 0; i < source_code_paths_LEN; i++){
		if(source_code_paths[i].executable_name == NULL){break;}
		if(strcmp(source_code_paths[i].executable_name, program_name) == 0){
			program_src_folder = source_code_paths[i].source_code_path;
			g_print("set program_src_folder to %s\n", program_src_folder);
		}
	}
}

static bool initialized = false;
void on_init(){
    if(!initialized){
        initialized = true;

        #ifdef USE_GTK3
            printf("Hello from GTK 3\n");
        #else
            printf("Hello from GTK 4\n");
        #endif

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
    }
}

void test_function_overwriting(){
	GtkWidget* button = gtk_button_new_with_label("Test switchAmPm");
	void* incHourPointer = get_pointer_from_identifier("switchAmPm");
	gtk_window_set_child(GTK_WINDOW(application_root), button);
	g_signal_connect_data(button, "clicked", G_CALLBACK(incHourPointer), NULL, NULL, (GConnectFlags)0);
	on_added_to_dom(button, NULL);

	callback_identifier id = {button, GTK_CALLBACK_clicked};
	on_edit_callback_button(button, &(id.callback)); 
	on_edit_callback_done_button(button, &(id.callback));
	function_dispatcher(button, &(id.callback));
}

void change_button_A(GtkWidget* widget, gpointer data){
	GtkWidget* button = (GtkWidget*)data;
	enum gtk_callback_category category = GTK_CALLBACK_clicked;	
	on_edit_callback_button(button, &category);
	on_edit_callback_done_button(button, &category);
}

void test_function_overwriting2(){
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);	
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);

	GtkWidget* button1 = gtk_button_new_with_label("A");
	GtkWidget* button2 = gtk_button_new_with_label("B");
	GtkWidget* button_change = gtk_button_new_with_label("Change A callback");
	
	gtk_box_append(GTK_BOX(vbox), button1);
	gtk_box_append(GTK_BOX(vbox), button2);
	gtk_box_append(GTK_BOX(vbox), button_change);
	gtk_window_set_child(GTK_WINDOW(application_root), vbox);
	on_added_to_dom(button1, NULL);	
	on_added_to_dom(button2, NULL);	
	on_added_to_dom(button_change, NULL);	

	void* show_fun = get_pointer_from_identifier("show");
	char* show_name = get_identifier_from_pointer(show_fun); 
	void* increment_fun = get_pointer_from_identifier("increment");
	char* increment_name = get_identifier_from_pointer(increment_fun);
	g_print("the name of the pointer %p is %s\n", increment_fun, increment_name);
	g_signal_connect_data(button1, "clicked", G_CALLBACK(increment_fun), NULL, NULL, (GConnectFlags)0);
	g_signal_connect_data(button2, "clicked", G_CALLBACK(show_fun), NULL, NULL, (GConnectFlags)0);
	g_signal_connect_data(button_change, "clicked", G_CALLBACK(change_button_A), button1, NULL, (GConnectFlags)0);
}
