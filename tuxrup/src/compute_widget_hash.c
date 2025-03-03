#include "compute_widget_hash.h"
#include "util.h"

guint compute_widget_hash(GtkWidget* widget, GHashTable* widget_hashes_map){
    GtkWidget* parent = gtk_widget_get_parent(widget);
    GType type = G_OBJECT_TYPE(widget);
    int child_number = get_child_number(widget);

    guint hash = g_int_hash(&type);
    if(parent){
        hash = g_int_hash(&type);
        hash ^= g_int_hash(&child_number);

        // this is wrong. If the parent hasn't been added yet, you should add the parent to the hash map!
        // two ways things can go wrong: either it is added, but has no hash, or it is not added at all
        if(!g_hash_table_contains(widget_hashes_map, (gpointer)parent)){
            //set_widget_hash(parent);
        }

        guint parent_hash = *((guint*)(g_hash_table_lookup(widget_hashes_map, (gpointer)parent)));
        if(parent_hash == 0){
            parent_hash = compute_widget_hash(parent, widget_hashes_map);
            g_hash_table_insert(widget_hashes_map, (gpointer)widget, (gpointer)(&hash));
        }
        hash ^= parent_hash;

        g_hash_table_insert(widget_hashes_map, (gpointer)widget, (gpointer)(&hash));
    } 
    return hash;
}