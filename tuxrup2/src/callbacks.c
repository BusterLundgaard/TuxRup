#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "util.h"
#include "io.h"
#include "symbols.h"
#include "globals.h"

static GtkWidget* active_widget = NULL;

typedef void(*callback_function_t)(GtkWidget*, gpointer);

void remove_callback (GtkWidget* w, char* callback_name){
    g_signal_handlers_disconnect_matched(
        w, 
        G_SIGNAL_MATCH_ID, 
        g_signal_lookup(callback_name, G_OBJECT_TYPE(w)), 
        0, 
        NULL, 
        NULL, 
        NULL);  
}

char* get_document_path(char* function_name){
    FILE* fp = popen(g_strdup_printf("ctags -R -o - %s | grep %s", get_program_src_folder(), function_name), "r");

    char buffer[1024];
    if(fgets(buffer, sizeof(buffer), fp) == NULL){
        return NULL;
    }

    char document_path[1024];
    document_path[0]='\0';
    sscanf(buffer, "%*s %1023s", document_path);
	g_print("document_path is %s\n", document_path);

    return g_strdup(document_path);
}

char* shared_lib_path(GtkWidget* widget){
	 return g_strdup_printf("runtime_generated_code/%lu.so", (unsigned long*)widget);
}

void function_dispatcher(GtkWidget* widget, gpointer data){
	void* shared_lib_pointer        = g_object_get_data(G_OBJECT(widget), "shared_lib_pointer");
	void* user_data                 = g_object_get_data(G_OBJECT(widget), "callback_data");
	void* original_function_pointer = g_object_get_data(G_OBJECT(widget), "callback_pointer");
	char* original_function_name    = identifier_from_pointer(original_function_pointer);

	if(shared_lib_pointer == NULL){g_print("shared_lib_pointer was NULL. Cant continue callback\n"); return;}              
	if(original_function_pointer == NULL){g_print("original_function_pointer was NULL. Cant continue callback\n"); return;}
	if(original_function_name == NULL){g_print("original_function_name was NULL. Cant continue callback\n"); return;}      

	if(user_data == NULL){g_print("user_data was NULL. But this may be intentional. Just a warning!\n");}                                

	callback_function_t custom_function_pointer = (callback_function_t)dlsym(shared_lib_pointer, original_function_name);
	if(custom_function_pointer == NULL){g_print("custom_function_pointer was NULL. Cant continue callback\n"); return;}

	sync_variables(shared_lib_path(widget), shared_lib_pointer, true); 
	custom_function_pointer(widget, user_data);
	sync_variables(shared_lib_path(widget), shared_lib_pointer, false); 
}

char* read_file(char* filepath){
	FILE* filePointer;
	char buffer[2012]; 
	FILE* fp = fopen(filepath, "r");

	GString* contents = g_string_new("");
	while(fgets(buffer, 2012, fp)) {
		g_string_append(contents, buffer);
	}
	fclose(filePointer);
	return contents->str;		
}

void on_callback_edit(GtkWidget* widget, GtkTextBuffer* buffer){
	active_widget = selected_widget;
	if(active_widget == NULL){
		g_print("Selected widget was null. Not continuing with editing callback\n");
		return;
	}

	char* original_document = get_document_path(identifier_from_pointer(g_object_get_data(G_OBJECT(active_widget), "callback_pointer"))); 
	char* contents = read_file(original_document); 
	gtk_text_buffer_set_text(buffer, contents, -1); 

	g_free(contents);
	g_free(original_document);
}


void on_callback_done(GtkWidget* widget, GtkTextBuffer* buffer){
	if(active_widget == NULL){
		g_print("Selected widget was null. Not continuing with editing callback\n");
		return;
	}

	// Compile code in the text buffer
	char* code = get_text_from_buffer(buffer);		
	
	g_file_set_contents("temp.c", code, strlen(code), NULL);

	char* compile_path = shared_lib_path(active_widget);
	char* gcc_command = g_strdup_printf(
			"gcc temp.c -w -g -shared -fPIC $(pkg-config --cflags --libs gtk+-3.0) -o %s", compile_path);
	g_print("%s\n", gcc_command);
	int res = system(gcc_command);

	// Check for potential issues
	if(res != 0){
		g_print("Compilation failed! Won't overwrite callback.\n");
		return;
	}

	char* callback_name = g_object_get_data(G_OBJECT(active_widget), "callback_name");
	if(callback_name == NULL){
		g_print("Couldn't find any callback_name field on widget %p. Won't overwrite callback.\n", active_widget);
		return;
	}

	void* shared_lib_pointer = dlopen(compile_path, RTLD_LAZY);
	if(shared_lib_pointer == NULL){
		g_print("Failed to open shared lib pointer to %s. Won't overwrite callback.\n", compile_path);
		return;
	}

	void* previous_shared_lib_pointer = g_object_get_data(G_OBJECT(active_widget), "shared_lib_pointer");
	if(previous_shared_lib_pointer != NULL){
		g_print("There was a previously opened shared_lib_pointer. Closing it and proceeding to set a new one.", compile_path);
		dlclose(previous_shared_lib_pointer);
	}
	
	// Actually change the callback	
	remove_callback(active_widget, callback_name);
	g_object_set_data(G_OBJECT(active_widget), "shared_lib_pointer", shared_lib_pointer);
	g_signal_connect_data_original(active_widget, callback_name, G_CALLBACK(&function_dispatcher), NULL, NULL, (GConnectFlags)0);
}

void callbacks_reset(GtkTextBuffer* buffer){
	active_widget = NULL;
	gtk_text_buffer_set_text(buffer, "", -1);
}
