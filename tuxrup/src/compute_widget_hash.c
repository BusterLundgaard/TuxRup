#include "compute_widget_hash.h"
#include "util.h"

guint compute_widget_hash(GtkWidget* widget, GHashTable* widget_hashes_map){
    const gchar* widget_type = g_type_name(G_OBJECT_TYPE(widget));
    guint hash = hash_string(widget_type);

    GtkWidget* parent = gtk_widget_get_parent(widget);
    if(!parent)
    {return hash;}

    int child_number = get_child_number(widget);
    hash ^= hash_int(child_number);

    guint* parent_hash = 
        g_hash_table_contains(widget_hashes_map, parent) ? 
        g_hash_table_lookup(widget_hashes_map, parent) 
        : NULL;
    if(parent_hash)
    {hash ^= *parent_hash; 
     return hash;}
    
    parent_hash = malloc(sizeof(guint));
    *parent_hash = compute_widget_hash(parent, widget_hashes_map);
    g_hash_table_insert(widget_hashes_map, parent, parent_hash);
    
    hash ^= *parent_hash;
    return hash;
}