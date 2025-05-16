

#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "util.h"



// void on_done_clicked(GtkWidget *widget, gpointer user_data)
// {   
//     gchar* text = get_textview_text(data->text_view);

//     static guint file_counter = 0;
    
//     // Apply the css
//     apply_css_to_widget(modified_widget, text);

//     // Make changes permanent by saving to a file
//     gchar *filename;
//     gchar *stored_filename = g_hash_table_lookup(widget_to_css_filepath_map, modified_widget);
//     if (stored_filename) {
//         filename = stored_filename;
//     } else {
//         filename = g_strdup_printf("./css_output_%u.css", file_counter++);
//         g_hash_table_insert(widget_to_css_filepath_map, modified_widget, filename);
//     }

//     GError *error = NULL;
//     if (!g_file_set_contents(filename, text, -1, &error)) {
//         g_print("Error writing to file %s: %s\n", filename, error->message);
//         g_error_free(error);
//         if (!stored_filename) {
//             g_free(filename);
//             g_hash_table_remove(widget_to_css_filepath_map, modified_widget);
//         }
//         g_free(text);
//         #ifdef USE_GTK3
//         gtk_widget_destroy(data->editor_window);
//         #else
//         gtk_window_destroy(GTK_WINDOW(data->editor_window));
//         #endif
//         g_free(data);
//         return;
//     } 

//     // Clean up and close the editor window
//     g_free(text);
//     #ifdef USE_GTK3
//     gtk_widget_destroy(data->editor_window);
//     #else
//     gtk_window_destroy(GTK_WINDOW(data->editor_window));
//     #endif
//     g_free(data);
// }
