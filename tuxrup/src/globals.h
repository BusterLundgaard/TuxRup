#ifndef GLOBALS_H
#define GLOBALS_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

enum gtk_version {
    UNKNOWN = 0,
    GTK_VERSION_4,
    GTK_VERSION_3,
    GTK_VERSION_2
};

enum gtk_callback_category {
    GTK_CALLBACK_UNDEFINED=0,

    // Mouse input
    GTK_CALLBACK_left_click=1,
    GTK_CALLBACK_right_click=2,
    GTK_CALLBACK_middle_click=3,
    
    // Mouse movement
    GTK_CALLBACK_motion=4,
    GTK_CALLBACK_enter=5,
    GTK_CALLBACK_leave=6,
    GTK_CALLBACK_motion_after=7,
    GTK_CALLBACK_end=8,

    // Keyboard input
    GTK_CALLBACK_key_pressed=9,
    GTK_CALLBACK_key_release=10,

    // General events
    GTK_CALLBACK_clicked=11,
    GTK_CALLBACK_activate=12,
    GTK_CALLBACK_toggled=13,
    GTK_CALLBACK_value_changed=14,
    GTK_CALLBACK_changed=15,
    GTK_CALLBACK_notify_property=16,
    GTK_CALLBACK_pressed=17,

    // Text events
    GTK_CALLBACK_insert_text=18,
    GTK_CALLBACK_delete_text=19,
    GTK_CALLBACK_backspace=20,

    // Resize, show, hide
    GTK_CALLBACK_size_allocate=21,
    GTK_CALLBACK_map=22,
    GTK_CALLBACK_unmap=23,

    // Drag and drop
    GTK_CALLBACK_drag_begin=24,
    GTK_CALLBACK_drag_drop=25,
    GTK_CALLBACK_drag_data_recieved=26,

    // Move or resize window
    GTK_CALLBACK_configure_event=27
};

enum widget_type_category {
    GTK_CATEGORY_UNDEFINED = 0,
    GTK_CATEGORY_ANY, 

    GTK_CATEGORY_Button, 
    
    GTK_CATEGORY_Entry, 
    GTK_CATEGORY_TextBuffer,
    
    GTK_CATEGORY_CheckButton, 
    GTK_CATEGORY_ToggleButton,
    
    GTK_CATEGORY_SpinButton, 
    GTK_CATEGORY_Scale, 
    
    GTK_CATEGORY_ComboBoxText, 
    GTK_CATEGORY_DropDown, 
    
    GTK_CATEGORY_Window
};

extern enum gtk_version gtk_ver;

extern GtkWidget* application_root;

extern GHashTable *widget_callback_table;
extern GHashTable *widget_hashes;
extern GHashTable *widget_to_css_filepath_map;

extern char* working_directory;
extern char* executable_path;
extern char* program_src_folder;
extern char* debug_symbols_path;

typedef struct {
    enum gtk_callback_category action_name; // A string with the name of the action
    enum widget_type_category valid_widget_types[5]; // The widgets you can apply this action to. No callback has more than 5 categories applying, so it's kind of just an arbitrary number to avoid a dynamically-sized array
} action;

#define MAPPABLE_ACTIONS_LEN 17
extern const action remapable_events[MAPPABLE_ACTIONS_LEN];

typedef void(*right_click_callback_type)(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);
typedef void(*menu_action_callback_type)(GSimpleAction* action, GVariant* parameter, gpointer user_data);
#endif