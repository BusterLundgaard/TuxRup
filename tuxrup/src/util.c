#include "util.h"
#include <clang-c/Index.h>

int get_child_number(GtkWidget* widget){
    
}

#ifdef USE_GTK3
int get_child_number_gtk3(GtkWidget* widget){
    GtkWidget *parent = gtk_widget_get_parent(widget);
    if (parent && GTK_IS_CONTAINER(parent)) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
        
        int index = 0;
        for (GList *l = children; l != NULL; l = l->next, index++) {
            GtkWidget *child = GTK_WIDGET(l->data);
            if (child == widget) {
                g_list_free(children);  
                return index;
            }
        }
        
        g_print("Widget not found among parent's children.\n");
        g_list_free(children);  
        return -1;
    } else {
        g_print("Widget has no parent, or at least no parent that is a container.\n");
        return -1;
    }
}
#else 
int get_child_number_gtk4(GtkWidget* widget){
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
#endif

char* get_basename_without_extension(const char *filepath) {
    char *dot = strrchr(filepath, '.');  // Find the last dot (.)
    size_t len = dot - filepath;  // Calculate length up to the dot
    char *basename = (char*)malloc(len + 1);
    strncpy(basename, filepath, len);
    basename[len] = '\0';  // Null-terminate the new string
    return basename;
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

void remove_callback (GtkWidget* w, char* callback_name){
    g_signal_handlers_disconnect_matched(
        w, 
        G_SIGNAL_MATCH_ID, 
        g_signal_lookup(callback_name, G_OBJECT_TYPE(w)), 
        0, 
        NULL, 
        NULL, 
        NULL);  
}

char* get_document_path(char* function_name){
    FILE* fp = popen(g_strdup_printf("ctags -R -o - %s | grep %s", program_src_folder, function_name), "r");

    char buffer[1024];
    if(fgets(buffer, sizeof(buffer), fp) == NULL){
        return NULL;
    }

    char document_path[1024];
    document_path[0]='\0';
    sscanf(buffer, "%*s %1023s", document_path);

    return g_strdup(document_path);
}

GtkWidget* create_textview(const char* text, bool editable){
    GtkWidget *text_view = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, text, -1);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), editable);
    return text_view;
}

char* get_textview_text(GtkWidget* text_view){
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    return g_strdup(gtk_text_buffer_get_text(buffer, &start, &end, FALSE));  
}

void print_cursor_location(CXSourceLocation loc){
    CXFile file;
    unsigned line, column;
    clang_getSpellingLocation(loc, &file, &line, &column, NULL);
    g_print("The modified function location is at line %u, column %u\n", line, column);
}

