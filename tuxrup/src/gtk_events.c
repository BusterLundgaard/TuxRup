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

gulong
on_g_signal_connect_data(
gpointer instance,
const gchar *detailed_signal,
GCallback c_handler,
gpointer data,
GClosureNotify destroy_data,
GConnectFlags connect_flags){

    if(widget_hashes == NULL)
    {return 0;}

    if(is_mappable_action(detailed_signal)){
        callback_identifier* cb_key = malloc(sizeof(callback_identifier));
        callback_info* cb_info = malloc(sizeof(callback_info));
        
        cb_key->widget = instance;
        cb_key->callback_name = g_strdup(detailed_signal);

        cb_info->original_function_pointer = (callback_type)c_handler;
        cb_info->function_name = get_identifier_from_pointer(c_handler);
        cb_info->dl_handle = NULL;
        cb_info->identifier_pointers = NULL;
        cb_info->identifier_pointers_n = 0;

        g_hash_table_insert(widget_callback_table, cb_key, cb_info);
    }

    if(!g_hash_table_contains(widget_hashes, instance)){
        g_hash_table_insert(widget_hashes, instance, NULL);
        normal_g_signal_connect_data(instance, "notify::root", G_CALLBACK(on_added_to_dom), NULL, ((void*)0), (GConnectFlags)0);

        if(is_mappable_action(detailed_signal)){
            #ifdef USE_GTK3
            g_signal_connect(instance, "button-press-event", G_CALLBACK(on_right_click_gtk3), NULL);
            #else 
            GtkGesture* gesture = gtk_gesture_click_new();
            gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3);
            gtk_widget_add_controller(GTK_WIDGET(instance), GTK_EVENT_CONTROLLER(gesture));
            normal_g_signal_connect_data(gesture, "pressed", G_CALLBACK(on_right_click_gtk4), instance, NULL, (GConnectFlags)0);
            #endif
        }
    }

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
        #elif defined(USE_GTK4)
            printf("Hello from GTK 4\n");
        #else
            printf("Unknown GTK version!\n");
        #endif

        printf("I was initialized yay! What's up with you though?\n");
        working_directory = get_working_directory();
        widget_hashes = g_hash_table_new(g_direct_hash, g_direct_equal);
        widget_callback_table = g_hash_table_new_full(callback_key_hash, callback_key_equal, callback_key_free, callback_value_free);
    }
}
