#include "util.h"

#include "../globals.h"
#include "../method_intercepting/hooks.h"

#include <clang-c/Index.h>
#include <sys/stat.h>

void set_gtk_version() {
	#ifdef USE_GTK3
        gtk_ver = GTK_VERSION_3;
    #else 
        gtk_ver = GTK_VERSION_4;
    #endif
}

GObjectClass* get_widget_class(GtkWidget* widget){
    GObject* object = G_OBJECT(widget);
    return G_OBJECT_GET_CLASS(G_OBJECT(widget));
}

GtkWidget* create_window(GtkWidget* any_widget_from_current_application, const char* title, guint width, guint height){
    // Get the parent window
    #ifdef USE_GTK3
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_toplevel(any_widget_from_current_application));  // Get the parent window
    #else
    GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_native(any_widget_from_current_application));  
    #endif
    
    // Get parent application
    GtkApplication *app = GTK_APPLICATION(gtk_window_get_application(parent_window)); 

    // Create a new window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    return window;
}

GtkWidget* add_widget_to_window(GtkWidget* window, GtkWidget* widget){
    #ifdef USE_GTK3 
    gtk_container_add(GTK_CONTAINER(window), widget);
    #else 
    gtk_window_set_child(GTK_WINDOW(window), widget);
    #endif
}

// Creates and returns a vbox in scrollable window with width=width, height=height
GtkWidget* create_and_add_scrollable_item_list(GtkWidget* window, guint width, guint height){
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);

    #ifdef USE_GTK3
    GtkAdjustment* v_adj = gtk_adjustment_new(0,0,0,0,0,0);
    GtkAdjustment* h_adj = gtk_adjustment_new(0,0,0,0,0,0);
    GtkWidget* scrolled_window = gtk_scrolled_window_new(v_adj, h_adj);
    #else 
    GtkWidget* scrolled_window = gtk_scrolled_window_new();
    #endif
    gtk_widget_set_size_request(scrolled_window, width, height);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), 
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    #ifdef USE_GTK3 
    gtk_container_add(GTK_CONTAINER(scrolled_window), vbox);
    #else 
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), vbox);
    #endif

    add_widget_to_window(window, scrolled_window);
    return vbox;
}

GtkWidget* add_widget_to_box(GtkWidget* box, GtkWidget* widget){
    #ifdef USE_GTK3 
    gtk_container_add(GTK_CONTAINER(box), widget);
    #else 
    gtk_box_append(GTK_BOX(box), widget);
    #endif
}

enum widget_type_category get_widget_type_category(GtkWidget* w){
    if(GTK_IS_BUTTON(w))        {return GTK_CATEGORY_Button;}
    if(GTK_IS_ENTRY(w))         {return GTK_CATEGORY_Entry;}
    if(GTK_IS_TEXT_BUFFER(w))   {return GTK_CATEGORY_TextBuffer;}
    if(GTK_IS_CHECK_BUTTON(w))  {return GTK_CATEGORY_CheckButton;}
    if(GTK_IS_TOGGLE_BUTTON(w)) {return GTK_CATEGORY_ToggleButton;}
    if(GTK_IS_SPIN_BUTTON(w))   {return GTK_CATEGORY_SpinButton;}
    if(GTK_IS_SCALE(w))         {return GTK_CATEGORY_Scale;}
    if(GTK_IS_COMBO_BOX_TEXT(w)){return GTK_CATEGORY_ComboBoxText;}
    #ifdef USE_GTK3
    if(GTK_IS_COMBO_BOX(w))     {return GTK_CATEGORY_DropDown;}
	if(GTK_IS_TOOL_BUTTON(w))   {g_print("It is a toolbutton!\n"); return GTK_CATEGORY_ToolButton;}
    #else
    if(GTK_IS_DROP_DOWN(w))     {return GTK_CATEGORY_DropDown;}
    #endif
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
    case GTK_CALLBACK_left_click: return "left-click";
    case GTK_CALLBACK_right_click: return "right-click";
    case GTK_CALLBACK_middle_click: return "middle-click";
    case GTK_CALLBACK_motion: return "motion";
    case GTK_CALLBACK_enter: return "enter";
    case GTK_CALLBACK_leave: return "leave";
    case GTK_CALLBACK_motion_after: return "motion-after";
    case GTK_CALLBACK_end: return "end";
    case GTK_CALLBACK_key_pressed: return "key-pressed";
    case GTK_CALLBACK_key_release: return "key-release";
    case GTK_CALLBACK_clicked: return "clicked";
    case GTK_CALLBACK_activate: return "activate";
    case GTK_CALLBACK_toggled: return "toggled";
    case GTK_CALLBACK_value_changed: return "value-changed";
    case GTK_CALLBACK_changed: return "changed";
    case GTK_CALLBACK_notify_property: return "notify-property";
    case GTK_CALLBACK_pressed: return "pressed";
    case GTK_CALLBACK_insert_text: return "insert-text";
    case GTK_CALLBACK_delete_text: return "delete-text";
    case GTK_CALLBACK_backspace: return "backspace";
    case GTK_CALLBACK_size_allocate: return "size-allocate";
    case GTK_CALLBACK_map: return "map";
    case GTK_CALLBACK_unmap: return "unmap";
    case GTK_CALLBACK_drag_begin: return "drag-begin";
    case GTK_CALLBACK_drag_drop: return "drag-drop";
    case GTK_CALLBACK_drag_data_recieved: return "drag-data-recieved";
    case GTK_CALLBACK_configure_event: return "configure-even";
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
        
        g_print("get_child_number_gtk3: Widget not found among parent's children.\n");
        g_list_free(children);  
        return -1;
    } else {
        g_print("get_child_number_gtk3: Widget has no parent, or at least no parent that is a container.\n");
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
        g_print("get_child_number_gtk4: Could not find widget child number for some reason\n");
        return -1;
    } else {
        g_print("get_child_number_gtk4: Widget has no parent.\n");
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

void copy_file(const char* file_path, const char* copied_path){
    FILE *copied_file = fopen(file_path, "r");
    if (!copied_file) {
        g_warning("Failed to open file to copy");
        g_free(file_path);
        return;
    }

    // Create the new file
    FILE *new_file = fopen(copied_path, "w");
    if (!new_file) {
        g_warning("Failed to create new file we are copying: %s", copied_path);
        fclose(copied_file);
        g_free(file_path);
        return;
    }

    // Copy contents of the template file to the new file
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), copied_file)) > 0) {
        fwrite(buffer, 1, bytes_read, new_file);
    }

    // Close the files
    fclose(copied_file);
    fclose(new_file);

    g_print("Created callback file: %s\n", file_path);
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

char* get_executable_directory() {
    char* buffer = malloc(sizeof(char)*1024);
    ssize_t len = readlink("/proc/self/exe", buffer, 1024 - 1);
    if(len == -1){
        return NULL;
    }

    buffer[len] = '\0';
	g_print("get_executable_directory returned: %s\n", buffer);
    return g_strdup(buffer);
}

gboolean file_exists(const gchar *filepath) {
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}

char* fix_broken_fucking_css_path(char* path){
    if(file_exists(path))
    {return path;}

    gchar *executable_dir = g_path_get_dirname(executable_path); // Get just the directory of the executable
    gchar *relative_path = NULL;

    relative_path = g_strdup(path + strlen(working_directory)); // Extract the relative part
    if (*relative_path == '/') relative_path++; // Remove leading slash if necessary
    
    gchar* fixed_path = g_build_filename(executable_dir, relative_path, NULL);    
    g_free(relative_path);
    g_free(executable_dir);
    
    return fixed_path;
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

void remove_callback (GtkWidget* w, enum gtk_callback_category callback){
    g_signal_handlers_disconnect_matched(
        w, 
        G_SIGNAL_MATCH_ID, 
        g_signal_lookup(get_callback_type_category_str(callback), G_OBJECT_TYPE(w)), 
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
	g_print("document_path is %s\n", document_path);

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
    #ifdef USE_GTK3
    gtk_widget_add_events(widget, GDK_BUTTON_PRESS_MASK);
    g_signal_connect_data_ORIGINAL(widget, "button-press-event", G_CALLBACK(callback), user_data, NULL, (GConnectFlags)0);
    #else
    GtkGesture* gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3); 
    gtk_widget_add_controller(GTK_WIDGET(widget), GTK_EVENT_CONTROLLER(gesture));
    g_signal_connect_data_ORIGINAL(gesture, "pressed", G_CALLBACK(callback), user_data, NULL, (GConnectFlags)0);
    #endif
}

char* execute_command_and_get_result(char* command){
    FILE *fp = popen(command, "r");
    if (!fp) {
        perror("Failed to run command");
        return NULL;
    }

    size_t buffer_size = 1024;
    size_t length = 0;
    char *output = malloc(buffer_size);
    if (!output) {
        perror("Memory allocation failed");
        pclose(fp);
        return NULL;
    }
    output[0] = '\0';

    char temp_buffer[1024];
    while (fgets(temp_buffer, sizeof(temp_buffer), fp) != NULL) {
        size_t temp_length = strlen(temp_buffer);
        if (length + temp_length + 1 > buffer_size) {  // +1 for null terminator
            buffer_size *= 2;
            char *new_output = realloc(output, buffer_size);
            if (!new_output) {
                perror("Memory reallocation failed");
                free(output);
                pclose(fp);
                return NULL;
            }
            output = new_output;
        }
        strcat(output, temp_buffer);
        length += temp_length;
    }

    pclose(fp);
    return output;
}

char* read_file_contents(char* filepath){
    gchar *content = NULL;
    gsize length = 0;
    GError *error = NULL;
    GFile* file = g_file_new_for_path(filepath);

    if (!g_file_load_contents(file, NULL, &content, &length, NULL, &error)) {
        fprintf(stderr, "Error loading file: %s\n", error->message);
        g_error_free(error);
        g_free(filepath);
        return NULL;
    }

    return content;
}

void append_text_to_file(char* filepath, char* text){
    FILE *output_file = fopen(filepath, "a");
    if (!output_file) {
        fprintf(stderr, "Error: Unable to open all_css.css for writing\n");
        return;
    }

    fwrite(text, 1, strlen(text), output_file);
    fprintf(output_file, "\n"); 
    fclose(output_file);
}

//[MEMLEAK]
void iterate_hash_table(GHashTable* table, hash_table_iter_function iter_function, gpointer user_data){
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, table);
    gpointer key, value;
    int i = 0;
    for(; g_hash_table_iter_next(&iter, &key, &value); i++){
        iter_function(key, value, i, user_data);
    }
}

void set_program_name(){
	char* executable_path_copy = g_strdup(executable_path);
    program_name = g_basename(executable_path_copy);
}
void set_program_src_code(){
	for(int i = 0; i < source_code_paths_LEN; i++){
		if(source_code_paths[i].executable_name == NULL){break;}
		if(strcmp(source_code_paths[i].executable_name, program_name) == 0){
			program_src_folder = source_code_paths[i].source_code_path;
			g_print("set program_src_folder to %s\n", program_src_folder);
		}
	}
}

