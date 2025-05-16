/* Converted from GTK 4 to GTK 3 */
#include <gtk/gtk.h>

static GtkWidget *image_view;

void set_image(const gchar *filename) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    if (!pixbuf) {
        g_printerr("Failed to load image: %s\n", filename);
        return;
    }

    gtk_image_set_from_pixbuf(GTK_IMAGE(image_view), pixbuf);
    g_object_unref(pixbuf);
}

GtkWidget* create_image_viewer(void) {
    GtkWidget *scrolled;

    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    image_view = gtk_image_new();
    gtk_container_add(GTK_CONTAINER(scrolled), image_view);

    return scrolled;
}