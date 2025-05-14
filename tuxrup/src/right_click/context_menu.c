#include "context_menu.h"

#include "../method_intercepting/hooks.h"
#include "../globals.h"
#include "../editing_properties/edit_properties_window.h"
#include "../editing_css/edit_css_window.h"
#include "../editing_callbacks/edit_callbacks_window.h"
#include "../utilities/util.h"

static GtkWidget* active_widget = NULL;

static void on_open_edit_properties_window(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    open_edit_properties_window(active_widget);
}

static void on_open_edit_styling_window(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    open_css_editor(active_widget);
}

static void on_open_edit_callbacks_window(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    open_edit_callbacks_window(active_widget);
}

static void on_remove_widget(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    GtkWidget* parent = gtk_widget_get_parent(active_widget);
    if(parent){
#ifdef USE_GTK3
		gtk_container_remove(GTK_CONTAINER(parent), active_widget);
#else
        gtk_widget_unparent(active_widget);
#endif
	}
}

static void pause_session(GtkWidget* widget, gpointer user_data){
	g_print("wow pause\n");	
}

void open_actions_menu(){
	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget* vbox = create_and_add_scrollable_item_list(window, 200,200);

	GtkWidget* props_button = gtk_button_new_with_label("Edit properties");
	GtkWidget* styling_button = gtk_button_new_with_label("Edit styling");
	GtkWidget* callbacks_button = gtk_button_new_with_label("Edit callbacks");
	GtkWidget* delete_button = gtk_button_new_with_label("Delete widget");

	GtkWidget* debug_pause = gtk_button_new_with_label("GDB pause");

	g_signal_connect_data_ORIGINAL(props_button, "clicked", G_CALLBACK(on_open_edit_properties_window), NULL, NULL, (GConnectFlags)0);
	g_signal_connect_data_ORIGINAL(styling_button, "clicked", G_CALLBACK(on_open_edit_styling_window), NULL, NULL, (GConnectFlags)0);
	g_signal_connect_data_ORIGINAL(callbacks_button, "clicked", G_CALLBACK(on_open_edit_callbacks_window), NULL, NULL, (GConnectFlags)0);
	g_signal_connect_data_ORIGINAL(delete_button, "clicked", G_CALLBACK(on_remove_widget), NULL, NULL, (GConnectFlags)0);

	add_widget_to_box(vbox, props_button);
	add_widget_to_box(vbox, styling_button);
	add_widget_to_box(vbox, callbacks_button);
	add_widget_to_box(vbox, delete_button);

	g_signal_connect(debug_pause, "clicked", G_CALLBACK(pause_session), NULL);
	add_widget_to_box(vbox, debug_pause);

#ifdef USE_GTK3
	gtk_widget_show_all(window);
#else
	gtk_window_present_ORIGINAL(GTK_WINDOW(window));
#endif
}

int select_widget_for_action(GtkWidget* widget, GdkEventButton* event, void* user_data){
    if(!(event->type == GDK_BUTTON_PRESS && event->button == 3)){
        return false;
    }

	if(active_widget != NULL && GTK_IS_WIDGET(active_widget)){
		gtk_style_context_remove_class(gtk_widget_get_style_context(active_widget), "tuxrup_selected");
	}
	if(widget != NULL && GTK_IS_WIDGET(widget)){
		active_widget = widget;
		gtk_style_context_add_class(gtk_widget_get_style_context(widget), "tuxrup_selected");
	}		
}
