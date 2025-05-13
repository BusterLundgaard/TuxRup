#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H
#include <gtk/gtk.h>

static void on_image_click(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);
static void on_zoom_changed(GtkSpinButton* spinbutton, gpointer data);
GtkWidget* create_imageviewer();

extern GtkWidget* images[3];

#endif
