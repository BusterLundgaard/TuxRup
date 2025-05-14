#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H
// #include <gtk/gtk.h>
#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else
    #include <gtk-4.0/gtk/gtk.h>
#endif

static void on_image_click(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);
static void on_zoom_changed(GtkSpinButton* spinbutton, gpointer data);
GtkWidget* create_imageviewer();

extern GtkWidget* images[3];

#endif
