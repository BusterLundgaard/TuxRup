#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H
#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

static void on_image_click(GtkWidget* widget, GdkEventButton* event, gpointer user_data);
static void on_zoom_changed(GtkSpinButton* spinbutton, gpointer data);
GtkWidget* create_imageviewer();

void on_move_image_left(GtkWidget* widget, gpointer user_data);
void on_move_image_right(GtkWidget* widget, gpointer user_data);

extern GtkWidget* images[3];

#endif
