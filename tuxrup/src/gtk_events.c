#include "globals.h"
#include "callback_map.h"
#include "gtk_events.h"
#include "compute_widget_hash.h"
#include "util.h"
#include "pointer_name_conversion.h"
#include "modify_callback.h"

void on_right_click(GtkWidget* widget){
    callback_identifier cb_id = {widget, "clicked"};
    gpointer value = g_hash_table_lookup(widget_callback_table, &cb_id);
    if(value == NULL){
        g_print("Couldn't find the callback information for the widget you just right-clicked..\n");
        return;
    }
    callback_info* cb_info = (callback_info*)value;
    
    char* callback_name = "clicked"; // This is just hard-coded for now, can be generalized later
    callback_code_information* code_info = get_callback_code_information(cb_info->original_function_pointer, "clicked");
    
    create_code_editing_menu(widget, callback_name, code_info);
}

#ifdef USE_GTK3
void on_right_click_gtk3(GtkWidget *widget, GdkEventButton *event, gpointer user_data){
    on_right_click(widget);
}
#else
void on_right_click_gtk4(GtkGestureClick *gesture,int, double x, double y, gpointer* self){
    on_right_click(GTK_WIDGET(self));
}
#endif

void on_added_to_dom(GtkWidget* widget, gpointer data){
    guint* hash = malloc(sizeof(guint));  
    *hash = compute_widget_hash(widget, widget_hashes);
    const gchar* widget_type = g_type_name(G_OBJECT_TYPE(widget));    
    g_hash_table_insert(widget_hashes, (gpointer)widget, (gpointer)hash);
}


GtkWidget* get_widget_from_connect_signal(gpointer instance){
    if(GTK_IS_WIDGET(instance)){return instance;}

    if(GTK_IS_GESTURE_CLICK(instance)){
        GtkGesture* gesture = (GtkGesture*)instance;
        //gtk_event_controller_key_new()
        return gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    }
    if(GTK_IS_EVENT_CONTROLLER_MOTION(instance)){
        GtkEventController* controller = (GtkEventController*)instance;
        return gtk_event_controller_get_widget(controller);
    }

    return NULL;
}

enum gtk_callback_category get_callback_category_from_connect_signal(gpointer instance, const gchar* detailed_signal){
    if(GTK_IS_WIDGET(instance)){
      if(0 == strcmp(detailed_signal, "clicked"))             { return GTK_CALLBACK_clicked;            } else if 
        (0 == strcmp(detailed_signal, "activate"))            { return GTK_CALLBACK_activate;           } else if
        (0 == strcmp(detailed_signal, "toggled"))             { return GTK_CALLBACK_toggled;            } else if
        (0 == strcmp(detailed_signal, "value-changed"))       { return GTK_CALLBACK_value_changed;      } else if
        (0 == strcmp(detailed_signal, "notify::property"))    { return GTK_CALLBACK_notify_property;    } else if
        (0 == strcmp(detailed_signal, "insert-text"))         { return GTK_CALLBACK_insert_text;        } else if
        (0 == strcmp(detailed_signal, "delete-text"))         { return GTK_CALLBACK_delete_text;        } else if
        (0 == strcmp(detailed_signal, "backspace"))           { return GTK_CALLBACK_backspace;          } else if
        (0 == strcmp(detailed_signal, "size-allocate"))       { return GTK_CALLBACK_size_allocate;      } else if
        (0 == strcmp(detailed_signal, "map"))                 { return GTK_CALLBACK_map;                } else if
        (0 == strcmp(detailed_signal, "unmap"))               { return GTK_CALLBACK_unmap;              } else if
        (0 == strcmp(detailed_signal, "drag-begin"))          { return GTK_CALLBACK_drag_begin;         } else if
        (0 == strcmp(detailed_signal, "drag-drop"))           { return GTK_CALLBACK_drag_drop;          } else if
        (0 == strcmp(detailed_signal, "drag-recieved"))       { return GTK_CALLBACK_drag_data_recieved; } else if
        (0 == strcmp(detailed_signal, "configure-event"))     { return GTK_CALLBACK_configure_event;    } 
    }

    if(GTK_IS_GESTURE_CLICK(instance)){
        GtkGestureSingle* gesture = (GtkGestureSingle*)instance;
        guint clicked_mouse_button = gtk_gesture_single_get_button(gesture);
        switch(clicked_mouse_button){
            case 1: return GTK_CALLBACK_left_click;
            case 2: return GTK_CALLBACK_middle_click;
            case 3: return GTK_CALLBACK_right_click;
        }
    }

    if(GTK_IS_EVENT_CONTROLLER_MOTION(instance)){
        if(0 == strcmp(detailed_signal, "motion"))            { return GTK_CALLBACK_motion; }
        if(0 == strcmp(detailed_signal, "enter"))             { return GTK_CALLBACK_enter; }
        if(0 == strcmp(detailed_signal, "leave"))             { return GTK_CALLBACK_leave; }
        if(0 == strcmp(detailed_signal, "motion-after"))      { return GTK_CALLBACK_motion_after; }
        if(0 == strcmp(detailed_signal, "end"))               { return GTK_CALLBACK_end; }
    }

    if(GTK_IS_EVENT_CONTROLLER_KEY(instance)){
        if(0 == strcmp(detailed_signal, "key-press-event"))   { return GTK_CALLBACK_key_pressed;}
        if(0 == strcmp(detailed_signal, "key-release-event")) { return GTK_CALLBACK_key_release;}
    }

    return GTK_CALLBACK_UNDEFINED;
}

bool widget_seen_before(GtkWidget* widget){return g_hash_table_contains(widget_hashes, widget);}

gulong
on_g_signal_connect_data(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags){

    if(instance == NULL || widget_hashes == NULL)
    {return 0;}

    GtkWidget* widget = get_widget_from_connect_signal(instance);
    if(widget == NULL)
    {return 0;}
    
    // // If this is the first time we see this widget, add it to the map of widget hashes, and add a "on_added_to_dom" signal for it
    // // We have to do this because there is no general "add_to_dom" function from a shared library we can overwrite
    if(!widget_seen_before(widget)){
        g_hash_table_insert(widget_hashes, widget, NULL);
        normal_g_signal_connect_data(widget, "notify::root", G_CALLBACK(on_added_to_dom), NULL, ((void*)0), (GConnectFlags)0);
    }

    enum widget_type_category widget_category = get_widget_type_category(widget);
    if(widget_category == GTK_CATEGORY_UNDEFINED)
    {return 0;}

    printf("%s\n", get_widget_type_category_str(widget_category));
    
    if(strcmp(detailed_signal, "clicked") == 0){
        printf("Adding callback with detailed_signal = %s\n", detailed_signal);
        char* function_name = get_identifier_from_pointer(c_handler);
        add_callback_to_table(widget, detailed_signal, c_handler, function_name);

        GtkGesture* gesture = gtk_gesture_click_new();
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3); 
        gtk_widget_add_controller(GTK_WIDGET(widget), GTK_EVENT_CONTROLLER(gesture));
        g_signal_connect(gesture, "pressed", G_CALLBACK(on_right_click_gtk4), widget);
    }

    // enum gtk_callback_category callback_category = get_callback_category_from_connect_signal(instance, detailed_signal);
    // if(callback_category == GTK_CALLBACK_UNDEFINED)
    // {return 0;}

    // printf("%s\n", get_callback_type_category_str(callback_category));
    
    // if(is_callback_remapable(widget_category, callback_category)){
    //     printf("Adding callback with detailed_signal = %s\n", detailed_signal);
    //     char* function_name = get_identifier_from_pointer(c_handler);
    //     add_callback_to_table(widget, detailed_signal, c_handler, function_name);
    // }

    return 0;
}


void on_gtk_window_present(GtkWindow *window)
{
    //clang_tests("../program_src/hello_world.c");
}

static bool initialized = false;
void on_init(){
    if(!initialized){
        initialized = true;

        #ifdef USE_GTK3
            printf("Hello from GTK 3\n");
        #else
            printf("Hello from GTK 4\n");
        #endif

        printf("I was initialized yay! What's up with you though?\n");
        working_directory = get_working_directory();
        widget_hashes = g_hash_table_new(g_direct_hash, g_direct_equal);
        widget_callback_table = g_hash_table_new_full(callback_key_hash, callback_key_equal, callback_key_free, callback_value_free);
    }
}
