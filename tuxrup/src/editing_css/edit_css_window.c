#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utilities/util.h"

#include "edit_css_window.h"

/* "Done" button callback */
void on_done_clicked(GtkWidget *widget, gpointer user_data)
{   
    EditorData *data = (EditorData *)user_data;
    GtkWidget *modified_widget = data->target_button;
    gchar* text = get_textview_text(data->text_view);

    static guint file_counter = 0;
    
    // Apply the css
    apply_css_to_widget(modified_widget, text);

    // Make changes permanent by saving to a file
    gchar *filename;
    gchar *stored_filename = g_hash_table_lookup(widget_to_css_filepath_map, modified_widget);
    if (stored_filename) {
        filename = stored_filename;
    } else {
        filename = g_strdup_printf("./css_output_%u.css", file_counter++);
        g_hash_table_insert(widget_to_css_filepath_map, modified_widget, filename);
    }

    GError *error = NULL;
    if (!g_file_set_contents(filename, text, -1, &error)) {
        g_print("Error writing to file %s: %s\n", filename, error->message);
        g_error_free(error);
        if (!stored_filename) {
            g_free(filename);
            g_hash_table_remove(widget_to_css_filepath_map, modified_widget);
        }
        g_free(text);
        gtk_window_destroy(GTK_WINDOW(data->editor_window));
        g_free(data);
        return;
    } 

    // Clean up and close the editor window
    g_free(text);
    gtk_window_destroy(GTK_WINDOW(data->editor_window));
    g_free(data);
}

EditorData* build_css_editor_window(GtkWidget* widget){
    GtkWidget* window = create_window(widget, "Edit widget styling", 400, 800);

    GtkWidget *vbox     = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_widget_set_vexpand(text_view, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), text_view);

    GtkWidget *done_button = gtk_button_new_with_label("Done");

    gtk_box_append(GTK_BOX(vbox), scrolled);
    gtk_box_append(GTK_BOX(vbox), done_button);
    gtk_window_set_child(GTK_WINDOW(window), vbox);
    
    EditorData *data       = g_malloc(sizeof(EditorData));
    data->text_view        = text_view;
    data->done_button      = done_button;
    data->editor_window    = window;
    data->target_button    = widget;

    return data;
}

/* Opens the text editor window. */
void open_css_editor(GtkWidget *widget)
{
    EditorData* editor_data = build_css_editor_window(widget);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor_data->text_view));
    
    CssProps result = get_css_class_information_of_widget(widget);
    gchar *stored_filename = g_hash_table_lookup(widget_to_css_filepath_map, widget);

    if (stored_filename) {
        gchar *file_contents = NULL;
        gsize length = 0;
        GError *error = NULL;
        if (g_file_get_contents(stored_filename, &file_contents, &length, &error)) {
            gtk_text_buffer_set_text(buffer, file_contents, -1);
            g_free(file_contents);
        } else {
            g_print("Error reading file %s: %s\n", stored_filename, error->message);
            g_error_free(error);
        }
    } else {
        char *contents = parse_applicable_css_rules("./all_css.css", &result);
        if (contents) {
            gtk_text_buffer_set_text(buffer, contents, -1);
        }
    }

    g_signal_connect(editor_data->done_button, "clicked", G_CALLBACK(on_done_clicked), editor_data);

    gtk_window_present(GTK_WINDOW(editor_data->editor_window));
}

