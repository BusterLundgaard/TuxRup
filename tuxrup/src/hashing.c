#include "hashing.h"
#include "util.h"

guint compute_calback_hash(GtkWidget* widget, enum gtk_callback_category callback){
    gpointer widget_hash = g_hash_table_lookup(widget_hashes, widget);
    if(widget_hash==NULL){
        g_print("Could not find the widget whose hash of a callback we're trying to compute! Returning mischievious 0 value!\n");
        return 0;
    }
    return *(guint*)widget_hash ^ g_int_hash(&callback);
}

guint compute_widget_hash(GtkWidget* widget){
    const gchar* widget_type = g_type_name(G_OBJECT_TYPE(widget));
    guint hash = hash_string(widget_type);

    GtkWidget* parent = gtk_widget_get_parent(widget);
    if(!parent){
        return hash;
    }

    int child_number = get_child_number(widget);
    hash ^= hash_int(child_number);

    guint* parent_hash;
    if(g_hash_table_contains(widget_hashes, parent)){
        parent_hash = (guint*)g_hash_table_lookup(widget_hashes, parent);
    } else {
        parent_hash = (guint*)g_hash_table_lookup(widget_hashes, parent); 
        parent_hash = malloc(sizeof(guint));
        *parent_hash = compute_widget_hash(parent);
        g_hash_table_insert(widget_hashes, parent, parent_hash); 
    }

    return hash ^ *parent_hash;
}

