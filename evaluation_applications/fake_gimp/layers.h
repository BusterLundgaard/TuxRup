#ifndef LAYERS_H
#define LAYERS_H

#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

static void on_layer_select(GtkWidget* self, gpointer data);
GtkWidget* create_layerpicker();

#endif
