#include "context_menu.h"
#include "globals.h"

static void open_edit_properties_window(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    g_print("Opened the edit properties window! TODO");
}

static void open_edit_styling_window(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    g_print("Opened edit styling window! TODO");
}

static void open_edit_callbacks_window(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    g_print("Opened edit callbacks window! TODO");
}

static void remove_widget(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    GtkWidget* widget = (GtkWidget*)user_data;
    GtkWidget* parent = gtk_widget_get_parent(widget);
    if(parent){
        gtk_widget_unparent(widget);
    }
}

void create_right_click_context_menu(GMenu* menu, GtkWidget* widget){
    GMenuItem* edit_properties = g_menu_item_new("Edit properties", "tuxrup.editproperties");
    GMenuItem* edit_styling = g_menu_item_new("Edit styling", "tuxrup.editstyling");
    GMenuItem* edit_callbacks = g_menu_item_new("Edit callbacks", "tuxrup.editcallbacks");
    GMenuItem* delete_widget_item = g_menu_item_new("Delete widget", "tuxrup.deletewidget");
    
    g_menu_append_item(menu, edit_properties);
    g_menu_append_item(menu, edit_styling);
    g_menu_append_item(menu, edit_callbacks);
    g_menu_append_item(menu, delete_widget_item);

    GSimpleActionGroup* action_group = g_simple_action_group_new();
    gtk_widget_insert_action_group(widget, "tuxrup", G_ACTION_GROUP(action_group));

    GSimpleAction* edit_properties_action = g_simple_action_new("editproperties", NULL);
    GSimpleAction* edit_styling_action = g_simple_action_new("editstyling", NULL);
    GSimpleAction* edit_callbacks_action = g_simple_action_new("editcallbacks", NULL);
    GSimpleAction* delete_widget_action = g_simple_action_new("deletewidget", NULL);
    normal_g_signal_connect_data(edit_properties_action, "activate", G_CALLBACK(open_edit_properties_window), NULL, NULL, (GConnectFlags)0);
    normal_g_signal_connect_data(edit_styling_action, "activate", G_CALLBACK(open_edit_styling_window), NULL, NULL, (GConnectFlags)0);
    normal_g_signal_connect_data(edit_callbacks_action, "activate", G_CALLBACK(open_edit_callbacks_window), NULL, NULL, (GConnectFlags)0);
    normal_g_signal_connect_data(delete_widget_action, "activate", G_CALLBACK(remove_widget), widget, NULL, (GConnectFlags)0);
    g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_properties_action));
    g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_styling_action));
    g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_callbacks_action));
    g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(delete_widget_action));
}

void create_and_open_right_click_context_menu(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data){
    GtkWidget* widget = (GtkWidget*)user_data;
    
    GMenu* menu = g_menu_new();
    GtkWidget* popover = gtk_popover_menu_new_from_model(G_MENU_MODEL(menu));
    gtk_widget_set_parent(popover, widget);

    create_right_click_context_menu(menu, widget);
    
    GtkPopoverMenu* popover_menu = GTK_POPOVER_MENU(popover);
    gtk_popover_popup(GTK_POPOVER(popover_menu));
}
