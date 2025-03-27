#ifndef GLOBALS_H
#define GLOBALS_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

enum gtk_callback_category {
    GTK_CALLBACK_UNDEFINED,

    // Mouse input
    GTK_CALLBACK_left_click,
    GTK_CALLBACK_right_click,
    GTK_CALLBACK_middle_click,
    
    // Mouse movement
    GTK_CALLBACK_motion,
    GTK_CALLBACK_enter,
    GTK_CALLBACK_leave,
    GTK_CALLBACK_motion_after,
    GTK_CALLBACK_end,

    // Keyboard input
    GTK_CALLBACK_key_pressed,
    GTK_CALLBACK_key_release,

    // General events
    GTK_CALLBACK_clicked,
    GTK_CALLBACK_activate,
    GTK_CALLBACK_toggled,
    GTK_CALLBACK_value_changed,
    GTK_CALLBACK_changed,
    GTK_CALLBACK_notify_property,
    GTK_CALLBACK_pressed,

    // Text events
    GTK_CALLBACK_insert_text,
    GTK_CALLBACK_delete_text,
    GTK_CALLBACK_backspace,

    // Resize, show, hide
    GTK_CALLBACK_size_allocate,
    GTK_CALLBACK_map,
    GTK_CALLBACK_unmap,

    // Drag and drop
    GTK_CALLBACK_drag_begin,
    GTK_CALLBACK_drag_drop,
    GTK_CALLBACK_drag_data_recieved,

    // Move or resize window
    GTK_CALLBACK_configure_event
};

enum widget_type_category {
    GTK_CATEGORY_ANY, 
    GTK_CATEGORY_UNDEFINED,

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

extern GtkWidget* application_root;

extern GHashTable *widget_callback_table;
extern GHashTable *widget_hashes;
extern GHashTable *widget_to_css_filepath_map;

extern const char* working_directory;
extern const char* executable_path;
extern const char* program_src_folder;

typedef struct {
    enum gtk_callback_category action_name; // A string with the name of the action
    enum widget_type_category valid_widget_types[5]; // The widgets you can apply this action to. No callback has more than 5 categories applying, so it's kind of just an arbitrary number to avoid a dynamically-sized array
} action;

#define MAPPABLE_ACTIONS_LEN 17
extern const action remapable_events[MAPPABLE_ACTIONS_LEN];

typedef gulong (*g_signal_connect_data_t)(gpointer instance,
                                          const gchar *detailed_signal,
                                          GCallback c_handler,
                                          gpointer data,
                                          GClosureNotify destroy_data,
                                          GConnectFlags connect_flags);
extern g_signal_connect_data_t normal_g_signal_connect_data;

typedef void(*right_click_callback_type)(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data);
typedef void(*menu_action_callback_type)(GSimpleAction* action, GVariant* parameter, gpointer user_data);
#endif