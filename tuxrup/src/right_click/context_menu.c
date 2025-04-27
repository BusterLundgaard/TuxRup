#include "context_menu.h"

#include "../method_intercepting/hooks.h"
#include "../globals.h"
#include "../editing_properties/edit_properties_window.h"
#include "../editing_css/edit_css_window.h"
#include "../editing_callbacks/edit_callbacks_window.h"
#include "../utilities/util.h"

static GtkWidget* context_menu_popover = NULL;
static GtkWidget* active_widget = NULL;
static bool window_open = false;

static void on_open_edit_properties_window(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    if(!active_widget){return;}
	g_print("active_widget = %p\n", active_widget);
    open_edit_properties_window(active_widget);
}

static void on_open_edit_styling_window(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    if(!active_widget){return;}
	g_print("active_widget = %p\n", active_widget);
    open_css_editor(active_widget);
}

static void on_open_edit_callbacks_window(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    if(!active_widget){return;}
    open_edit_callbacks_window(active_widget);
}

static void on_remove_widget(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    if(!active_widget){return;}
    GtkWidget* parent = gtk_widget_get_parent(active_widget);
    if(parent){
#ifdef USE_GTK3
		gtk_container_remove(GTK_CONTAINER(parent), active_widget);
#else
        gtk_widget_unparent(active_widget);
#endif
	}
}

// void initialize_right_click_context_menu(){
//     GMenu* menu = g_menu_new();
//     context_menu_popover = gtk_popover_menu_new_from_model(G_MENU_MODEL(menu));
    
//     GMenuItem* edit_properties = g_menu_item_new("Edit properties", "tuxrup.editproperties");
//     GMenuItem* edit_styling = g_menu_item_new("Edit styling", "tuxrup.editstyling");
//     GMenuItem* edit_callbacks = g_menu_item_new("Edit callbacks", "tuxrup.editcallbacks");
//     GMenuItem* delete_widget_item = g_menu_item_new("Delete widget", "tuxrup.deletewidget");
    
//     g_menu_append_item(menu, edit_properties);
//     g_menu_append_item(menu, edit_styling);
//     g_menu_append_item(menu, edit_callbacks);
//     g_menu_append_item(menu, delete_widget_item);

//     GSimpleActionGroup* action_group = g_simple_action_group_new();
//     gtk_widget_insert_action_group(application_root, "tuxrup", G_ACTION_GROUP(action_group));

//     GSimpleAction* edit_properties_action = g_simple_action_new("editproperties", NULL);
//     GSimpleAction* edit_styling_action = g_simple_action_new("editstyling", NULL);
//     GSimpleAction* edit_callbacks_action = g_simple_action_new("editcallbacks", NULL);
//     GSimpleAction* delete_widget_action = g_simple_action_new("deletewidget", NULL);
//     g_signal_connect_data_ORIGINAL(edit_properties_action, "activate", G_CALLBACK(on_open_edit_properties_window), NULL, NULL, (GConnectFlags)0);
//     g_signal_connect_data_ORIGINAL(edit_styling_action, "activate", G_CALLBACK(on_open_edit_styling_window), NULL, NULL, (GConnectFlags)0);
//     g_signal_connect_data_ORIGINAL(edit_callbacks_action, "activate", G_CALLBACK(on_open_edit_callbacks_window), NULL, NULL, (GConnectFlags)0);
//     g_signal_connect_data_ORIGINAL(delete_widget_action, "activate", G_CALLBACK(on_remove_widget), NULL, NULL, (GConnectFlags)0);
//     g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_properties_action));
//     g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_styling_action));
//     g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_callbacks_action));
//     g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(delete_widget_action));
// }

void close_right_click_context_menu(GtkWidget* widget, gpointer user_data){
	window_open = false;
	active_widget = NULL;
}

//Callback that is called when right-clicking a widget
#ifdef USE_GTK3
gboolean open_right_click_context_menu(GtkWidget* widget, GdkEventButton *event, gpointer user_data) {
    if(!(event->type == GDK_BUTTON_PRESS && event->button == 3)){
        return false;
    }
#else
gboolean open_right_click_context_menu(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data){
#endif
    if(GTK_IS_WINDOW(user_data)){return true;}

	g_print("user_data = %p\n", user_data);
    #ifndef USE_GTK3
    GtkWidget* widget = (GtkWidget*)user_data;
    #endif
    active_widget = widget;

	/* if(window_open){return;} */

	GtkWidget* window = create_window(widget, "Tuxrup actions", 200, 200);
	GtkWidget* vbox = create_and_add_scrollable_item_list(window, 200,200);
	g_signal_connect_data_ORIGINAL(window, "destroy", G_CALLBACK(close_right_click_context_menu), NULL, NULL, (GConnectFlags)0);

	GtkWidget* props_button = gtk_button_new_with_label("Edit properties");
	GtkWidget* styling_button = gtk_button_new_with_label("Edit styling");
	GtkWidget* callbacks_button = gtk_button_new_with_label("Edit callbacks");
	GtkWidget* delete_button = gtk_button_new_with_label("Delete widget");

	g_signal_connect_data_ORIGINAL(props_button, "clicked", G_CALLBACK(on_open_edit_properties_window), NULL, NULL, (GConnectFlags)0);
	g_signal_connect_data_ORIGINAL(styling_button, "clicked", G_CALLBACK(on_open_edit_styling_window), NULL, NULL, (GConnectFlags)0);
	g_signal_connect_data_ORIGINAL(callbacks_button, "clicked", G_CALLBACK(on_open_edit_callbacks_window), NULL, NULL, (GConnectFlags)0);
	g_signal_connect_data_ORIGINAL(delete_button, "clicked", G_CALLBACK(on_remove_widget), NULL, NULL, (GConnectFlags)0);

	add_widget_to_box(vbox, props_button);
	add_widget_to_box(vbox, styling_button);
	add_widget_to_box(vbox, callbacks_button);
	add_widget_to_box(vbox, delete_button);

	window_open = true;
#ifdef USE_GTK3
	gtk_widget_show_all(window);
#else
	gtk_window_present_ORIGINAL(GTK_WINDOW(window));
#endif
	return true;
}
