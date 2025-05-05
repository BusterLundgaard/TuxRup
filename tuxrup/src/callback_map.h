#ifndef CALLBACK_MAP_H
#define CALLBACK_MAP_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif
#include <stdbool.h>

#include "globals.h"

typedef struct {
	char* name;
	char* type;
} function_argument;

typedef struct {
	function_argument* args;
	int n;
} function_arguments;

typedef void (*callback_type)(GtkWidget*, gpointer);

typedef struct {
    gpointer widget;
    enum gtk_callback_category callback;    
} callback_identifier;

typedef struct {
    callback_identifier* id;

    callback_type original_function_pointer;
	gpointer original_user_data;
    gchar* function_name;
    void* dl_handle;
    void** identifier_pointers;
    int identifier_pointers_n;

    guint hash;
    char* modified_code_path; // path to the C file containing the modified code
    char* shared_library_path; // path to the shared library with the compiled new callback
    char* original_document_path;
    GString* original_function_code;
    function_arguments original_function_args;
    int original_function_location;
    GString* original_before_code;
    GString* original_definitions_code;
    GString* original_after_code;
} callback_info;

callback_info* callback_map_add_original(GtkWidget* widget, enum gtk_callback_category callback_category, gpointer original_pointer, gpointer original_user_data);
callback_info* callback_map_add_new(GtkWidget* widget, enum gtk_callback_category callback_category);

callback_info* callback_map_get(GtkWidget* widget, enum gtk_callback_category callback_category);
bool callback_map_exists(GtkWidget* widget, enum gtk_callback_category callback_category);

guint callback_key_hash(gconstpointer key);
gboolean callback_key_equal(gconstpointer a, gconstpointer b);

void callback_key_free(gpointer data);
void callback_value_free(void* data);


#endif
