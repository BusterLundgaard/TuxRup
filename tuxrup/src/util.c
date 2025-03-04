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

char* get_working_directory(){
    FILE* pipe = popen("pwd", "r");
    char buffer[1024];
    if(fgets(buffer, sizeof(buffer), pipe) == NULL){
        printf("Error finding working directory\n");
        return NULL;
    }
    return g_strdup(buffer);
}

unsigned int hash_int(int key) {
    return (unsigned int)key * 2654435761U;  // Knuth's multiplicative hash
}

unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash;
}

char* get_text_view_text(GtkWidget* text_view){
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);  
}

   