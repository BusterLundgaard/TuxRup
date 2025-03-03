#include "util.h"

int get_child_number(GtkWidget* widget){
    GtkWidget *parent = gtk_widget_get_parent(widget);
    if (parent) {
        GtkWidget *child = gtk_widget_get_first_child(parent);
        int index = 0;
        for(GtkWidget *child = gtk_widget_get_first_child(parent); child != NULL; child=gtk_widget_get_next_sibling(child), index++){
            if(child == widget){
                return index;
            }
        }
        g_print("Could not find widget child number for some reason\n");
        return -1;
    } else {
        g_print("Widget has no parent.\n");
        return -1;
    }
}

char* get_basename_without_extension(const char *filepath) {
    char *dot = strrchr(filepath, '.');  // Find the last dot (.)
    size_t len = dot - filepath;  // Calculate length up to the dot
    char *basename = (char*)malloc(len + 1);
    strncpy(basename, filepath, len);
    basename[len] = '\0';  // Null-terminate the new string
    return basename;
}

void remove_signal (GtkWidget* w, char* signal_name){
    g_signal_handlers_disconnect_matched(
        w, 
        G_SIGNAL_MATCH_ID, 
        g_signal_lookup(signal_name, G_OBJECT_TYPE(w)), 
        0, 
        NULL, 
        NULL, 
        NULL);  
}

bool is_mappable_action(const gchar* event_name){
    for(int i = 0; i < MAPPABLE_ACTIONS_LEN; i++){
        if(strcmp(event_name, remapable_events[i]) == 0){
            return true;
        }
    }
    return false; 
}