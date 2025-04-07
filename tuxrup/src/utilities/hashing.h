#ifndef COMPUTE_WIDGET_HASH_H
#define COMPUTE_WIDGET_HASH_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include "../globals.h"

guint compute_widget_hash(GtkWidget* widget);

guint compute_callback_hash(GtkWidget* widget, enum gtk_callback_category callback);

#endif