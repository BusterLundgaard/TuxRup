#ifndef CALLBACK_MAP_H
#define CALLBACK_MAP_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

typedef void (*callback_type)(GtkWidget*, gpointer);

typedef struct {
    gpointer widget;
    gchar* callback_name;
} callback_identifier;

typedef struct {
    callback_type original_function_pointer;
    gchar* function_name;
    void* dl_handle;
    void** identifier_pointers;
    int identifier_pointers_n;
} callback_info;

guint callback_key_hash(gconstpointer key);
gboolean callback_key_equal(gconstpointer a, gconstpointer b);

void callback_key_free(gpointer data);
void callback_value_free(void* data);

#endif