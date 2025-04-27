#include "events_util.h"

GtkWidget* get_widget_from_connect_signal(gpointer instance){
	// This is going wrong probably because there's some GTK4 exlusive features here...
    if(GTK_IS_WIDGET(instance)){return instance;}

    #ifndef USE_GTK3
    if(GTK_IS_GESTURE_CLICK(instance)){
        GtkGesture* gesture = (GtkGesture*)instance;
        //gtk_event_controller_key_new()
        return gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
    }
    if(GTK_IS_EVENT_CONTROLLER_MOTION(instance)){
        GtkEventController* controller = (GtkEventController*)instance;
        return gtk_event_controller_get_widget(controller);
    }
    #endif

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
        (0 == strcmp(detailed_signal, "configure-event"))     { return GTK_CALLBACK_configure_event;    } else if
        (0 == strcmp(detailed_signal, "button-press-event"))     { return GTK_CALLBACK_left_click; }
    }

#ifndef USE_GTK3
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
#endif

    return GTK_CALLBACK_UNDEFINED;
}

bool widget_seen_before(GtkWidget* widget){return g_hash_table_contains(widget_hashes, widget);}
