#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * DEFINE the global here exactly once. 
 * That’s the actual variable stored in memory. 
 */

GHashTable *widget_to_filepath_map = NULL;

/* "Done" button callback */
void on_done_clicked(GtkWidget *widget, gpointer user_data)
{   
    static guint file_counter = 0;
    EditorData *data = (EditorData *)user_data;
    GtkWidget *clickedWidget = data->target_button;

    g_print("on_done_clicked triggered\n");

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    /* Look up any existing filename in our global hash table. */
    gchar *stored_filename = g_hash_table_lookup(widget_to_filepath_map, clickedWidget);
    gchar *filename = NULL;

    if (stored_filename) {
        g_print("Widget in hashmap with filename: %s (overwriting)\n", stored_filename);
        filename = stored_filename;
    } else {
        filename = g_strdup_printf("./css_output_%u.css", file_counter++);
        g_hash_table_insert(widget_to_filepath_map, clickedWidget, filename);
    }

    g_print("Writing to file: %s\n", filename);
    GError *error = NULL;
    if (!g_file_set_contents(filename, text, -1, &error)) {
        g_print("Error writing to file %s: %s\n", filename, error->message);
        g_error_free(error);
        if (!stored_filename) {
            g_free(filename);
            g_hash_table_remove(widget_to_filepath_map, clickedWidget);
        }
        g_free(text);
        gtk_window_destroy(GTK_WINDOW(data->editor_window));
        g_free(data);
        return;
    } else {
        g_print("Successfully wrote CSS to file: %s\n", filename);
    }

    g_print("Applying CSS:\n%s\n", text);
    changeCss(clickedWidget, text); /* from css_utils.h */

    g_free(text);
    gtk_window_destroy(GTK_WINDOW(data->editor_window));
    g_free(data);
}

/* Opens the text editor window. */
void open_text_editor(GtkWidget *widget, gpointer user_data)
{
    GtkApplication *app = GTK_APPLICATION(user_data);

    GtkWidget *editor_window = gtk_application_window_new(app); // can change.
    gtk_window_set_title(GTK_WINDOW(editor_window), "Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(editor_window), 600, 400);

    GtkWidget *vbox     = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_widget_set_vexpand(text_view, TRUE);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    CssProps result = listCssPropertiesOfGobject(G_OBJECT(widget));
    gchar *stored_filename = g_hash_table_lookup(widget_to_filepath_map, widget);

    if (stored_filename) {
        g_print("File found for widget: %s\n", stored_filename);
        gchar *file_contents = NULL;
        gsize length = 0;
        GError *error = NULL;
        if (g_file_get_contents(stored_filename, &file_contents, &length, &error)) {
            gtk_text_buffer_set_text(buffer, file_contents, -1);
            g_print("Loaded CSS contents:\n%s\n", file_contents);
            g_free(file_contents);
        } else {
            g_print("Error reading file %s: %s\n", stored_filename, error->message);
            g_error_free(error);
        }
    } else {
        g_print("generating dynamically\n");
        char *contents = getCss("/home/wowsuchdoge/coding/style.css",
                                result.css_name,
                                (const char **)result.css_classes,
                                result.num_classes);
        if (contents) {
            gtk_text_buffer_set_text(buffer, contents, -1);
            g_print("Dynamically loaded CSS contents:\n%s\n", contents);
        }
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), text_view);

    GtkWidget *done_button = gtk_button_new_with_label("Done");
    EditorData *data       = g_malloc(sizeof(EditorData));
    data->text_view        = text_view;
    data->editor_window    = editor_window;
    data->target_button    = widget;

    g_signal_connect(done_button, "clicked", G_CALLBACK(on_done_clicked), data);

    gtk_box_append(GTK_BOX(vbox), scrolled);
    gtk_box_append(GTK_BOX(vbox), done_button);
    gtk_window_set_child(GTK_WINDOW(editor_window), vbox);

    gtk_window_present(GTK_WINDOW(editor_window));
}

/* The existing right-click handler that calls open_text_editor. */
void on_right_click(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
    guint button = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    if (button == GDK_BUTTON_SECONDARY) {
        GtkWidget *clicked_widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
        open_text_editor(clicked_widget, user_data);
    }
}

