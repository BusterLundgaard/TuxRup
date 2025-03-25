#include "util.h"
#include <clang-c/Index.h>

enum widget_type_category get_widget_type_category(GtkWidget* w){
    if(GTK_IS_BUTTON(w))        {return GTK_CATEGORY_Button;}
    if(GTK_IS_ENTRY(w))         {return GTK_CATEGORY_Entry;}
    if(GTK_IS_TEXT_BUFFER(w))   {return GTK_CATEGORY_TextBuffer;}
    if(GTK_IS_CHECK_BUTTON(w))  {return GTK_CATEGORY_CheckButton;}
    if(GTK_IS_TOGGLE_BUTTON(w)) {return GTK_CATEGORY_ToggleButton;}
    if(GTK_IS_SPIN_BUTTON(w))   {return GTK_CATEGORY_SpinButton;}
    if(GTK_IS_SCALE(w))         {return GTK_CATEGORY_Scale;}
    if(GTK_IS_COMBO_BOX_TEXT(w)){return GTK_CATEGORY_ComboBoxText;}
    if(GTK_IS_DROP_DOWN(w))     {return GTK_CATEGORY_DropDown;}
    if(GTK_IS_WINDOW(w))        {return GTK_CATEGORY_Window;}
    return GTK_CATEGORY_UNDEFINED;
}
char* get_widget_type_category_str(enum widget_type_category c){
    switch(c){
        case GTK_CATEGORY_Button: return "GTK_CATEGORY_Button";
        case GTK_CATEGORY_Entry: return "GTK_CATEGORY_Entry";
        case GTK_CATEGORY_TextBuffer: return "GTK_CATEGORY_TextBuffer";
        case GTK_CATEGORY_CheckButton: return "GTK_CATEGORY_CheckButton";
        case GTK_CATEGORY_ToggleButton: return "GTK_CATEGORY_ToggleButton";
        case GTK_CATEGORY_SpinButton: return "GTK_CATEGORY_SpinButton";
        case GTK_CATEGORY_Scale: return "GTK_CATEGORY_Scale";
        case GTK_CATEGORY_ComboBoxText: return "GTK_CATEGORY_ComboBoxText";
        case GTK_CATEGORY_DropDown: return "GTK_CATEGORY_DropDown";
        case GTK_CATEGORY_Window: return "GTK_CATEGORY_Window";
        case GTK_CATEGORY_UNDEFINED: return "GTK_CATEGORY_UNDEFINED";
    }
    printf("Hey ya dumbass! Looks like you added a new widget_type_category, but forgot to update get_widget_type_category_str, so now it's just outputting empty\n");
    return "";
}
char* get_callback_type_category_str(enum gtk_callback_category c){
    switch(c) {
    case GTK_CALLBACK_left_click: return "GTK_CALLBACK_left_click";
    case GTK_CALLBACK_right_click: return "GTK_CALLBACK_right_click";
    case GTK_CALLBACK_middle_click: return "GTK_CALLBACK_middle_click";
    case GTK_CALLBACK_motion: return "GTK_CALLBACK_motion";
    case GTK_CALLBACK_enter: return "GTK_CALLBACK_enter";
    case GTK_CALLBACK_leave: return "GTK_CALLBACK_leave";
    case GTK_CALLBACK_motion_after: return "GTK_CALLBACK_motion_after";
    case GTK_CALLBACK_end: return "GTK_CALLBACK_end";
    case GTK_CALLBACK_key_pressed: return "GTK_CALLBACK_key_pressed";
    case GTK_CALLBACK_key_release: return "GTK_CALLBACK_key_release";
    case GTK_CALLBACK_clicked: return "GTK_CALLBACK_clicked";
    case GTK_CALLBACK_activate: return "GTK_CALLBACK_activate";
    case GTK_CALLBACK_toggled: return "GTK_CALLBACK_toggled";
    case GTK_CALLBACK_value_changed: return "GTK_CALLBACK_value_changed";
    case GTK_CALLBACK_changed: return "GTK_CALLBACK_changed";
    case GTK_CALLBACK_notify_property: return "GTK_CALLBACK_notify_property";
    case GTK_CALLBACK_pressed: return "GTK_CALLBACK_pressed";
    case GTK_CALLBACK_insert_text: return "GTK_CALLBACK_insert_text";
    case GTK_CALLBACK_delete_text: return "GTK_CALLBACK_delete_text";
    case GTK_CALLBACK_backspace: return "GTK_CALLBACK_backspace";
    case GTK_CALLBACK_size_allocate: return "GTK_CALLBACK_size_allocate";
    case GTK_CALLBACK_map: return "GTK_CALLBACK_map";
    case GTK_CALLBACK_unmap: return "GTK_CALLBACK_unmap";
    case GTK_CALLBACK_drag_begin: return "GTK_CALLBACK_drag_begin";
    case GTK_CALLBACK_drag_drop: return "GTK_CALLBACK_drag_drop";
    case GTK_CALLBACK_drag_data_recieved: return "GTK_CALLBACK_drag_data_recieved";
    case GTK_CALLBACK_configure_event: return "GTK_CALLBACK_configure_even";
    case GTK_CALLBACK_UNDEFINED: return "UNDEFINED";
    }
}

bool is_callback_remapable(enum widget_type_category widget_c, enum gtk_callback_category callback_c){
    for(int i = 0; i < MAPPABLE_ACTIONS_LEN; i++){
        if(callback_c == (&remapable_events[i])->action_name){
            if((&remapable_events[i])->valid_widget_types[0] == GTK_CATEGORY_ANY)
            {return true;}

            int j = 0;
            for(int j = 0; (&remapable_events[i])->valid_widget_types[j] != 0; j++){
                if((&remapable_events[i])->valid_widget_types[j] == widget_c)
                {return true;}
            }
        }
    }
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

int get_child_number(GtkWidget* widget){
    #ifdef USE_GTK3
    get_child_number_gtk3(widget);
    #else 
    get_child_number_gtk4(widget);
    #endif    
}

char* get_basename_without_extension(const char *filepath) {
    char *dot = strrchr(filepath, '.');  // Find the last dot (.)
    size_t len = dot - filepath;  // Calculate length up to the dot
    char *basename = (char*)malloc(len + 1);
    strncpy(basename, filepath, len);
    basename[len] = '\0';  // Null-terminate the new string
    return basename;
}

bool is_mappable_action(const gchar* event_name){
    // for(int i = 0; i < MAPPABLE_ACTIONS_LEN; i++){
    //     if(strcmp(event_name, remapable_events[i]) == 0){
    //         return true;
    //     }
    // }
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

void add_right_click_action(GtkWidget* widget, right_click_callback_type callback, gpointer user_data) {
    GtkGesture* gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3); 
    gtk_widget_add_controller(GTK_WIDGET(widget), GTK_EVENT_CONTROLLER(gesture));
    normal_g_signal_connect_data(gesture, "pressed", G_CALLBACK(callback), user_data, NULL, (GConnectFlags)0);
}
