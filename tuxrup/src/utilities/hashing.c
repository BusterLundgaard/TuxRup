#include "hashing.h"

#include "util.h"

guint compute_callback_hash(GtkWidget* widget, enum gtk_callback_category callback){
    guint widget_hash = widget_hashes_get(widget);
    return widget_hash ^ g_int_hash(&callback);
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

    guint parent_hash = widget_hashes_get(parent);
    return hash ^ parent_hash;
}

guint widget_hashes_get(GtkWidget* widget){
	if(g_hash_table_contains(computed_hashes, widget)){
		return *(guint*)g_hash_table_lookup(computed_hashes, widget);
	}
	guint* hash = malloc(sizeof(guint));
	*hash = compute_widget_hash(widget);
	g_hash_table_insert(computed_hashes, widget, hash);
	return *hash;
}
