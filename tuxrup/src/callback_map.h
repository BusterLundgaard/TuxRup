#ifndef CALLBACK_MAP_H
#define CALLBACK_MAP_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include "globals.h"

typedef void (*callback_type)(GtkWidget*, gpointer);

typedef struct {
    gpointer widget;
    enum gtk_callback_category callback;    
} callback_identifier;

typedef struct {
    callback_type original_function_pointer;
    gchar* function_name;
    void* dl_handle;
    void** identifier_pointers;
    int identifier_pointers_n;

    char* original_document_path;
    GString* original_function_code;
    GString* original_function_args;
    int original_function_code_location;
    GString* original_before_code;
    GString* original_before_code_removed_symbols;
    GString* original_after_code;
} callback_info;

void callback_map_add_original(GtkWidget* widget, enum gtk_callback_category callback_category, gpointer original_pointer, char* original_function_name);
void callback_map_add_new(GtkWidget* widget, enum gtk_callback_category callback_category);

callback_info* callback_map_get(GtkWidget* widget, enum gtk_callback_category callback_category);
bool callback_map_exists(GtkWidget* widget, enum gtk_callback_category callback_category);

guint callback_key_hash(gconstpointer key);
gboolean callback_key_equal(gconstpointer a, gconstpointer b);

void callback_key_free(gpointer data);
void callback_value_free(void* data);


#endif