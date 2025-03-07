#ifndef COMPUTE_WIDGET_HASH_H
#define COMPUTE_WIDGET_HASH_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

guint compute_widget_hash(GtkWidget* widget, GHashTable* widget_hashes_map);

#endif