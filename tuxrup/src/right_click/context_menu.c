#include "context_menu.h"

#include "../method_intercepting/hooks.h"
#include "../globals.h"
#include "../editing_properties/edit_properties_window.h"
#include "../editing_css/edit_css_window.h"
#include "../editing_callbacks/edit_callbacks_window.h"

static GtkWidget* context_menu_popover = NULL;
static GtkWidget* active_widget = NULL;

static void on_open_edit_properties_window(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    if(!active_widget){return;}
    open_edit_properties_window(active_widget);
}

static void on_open_edit_styling_window(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    if(!active_widget){return;}
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
        gtk_widget_unparent(active_widget);
    }
}

void initialize_right_click_context_menu(){
    GMenu* menu = g_menu_new();
    context_menu_popover = gtk_popover_menu_new_from_model(G_MENU_MODEL(menu));
    
    GMenuItem* edit_properties = g_menu_item_new("Edit properties", "tuxrup.editproperties");
    GMenuItem* edit_styling = g_menu_item_new("Edit styling", "tuxrup.editstyling");
    GMenuItem* edit_callbacks = g_menu_item_new("Edit callbacks", "tuxrup.editcallbacks");
    GMenuItem* delete_widget_item = g_menu_item_new("Delete widget", "tuxrup.deletewidget");
    
    g_menu_append_item(menu, edit_properties);
    g_menu_append_item(menu, edit_styling);
    g_menu_append_item(menu, edit_callbacks);
    g_menu_append_item(menu, delete_widget_item);

    GSimpleActionGroup* action_group = g_simple_action_group_new();
    gtk_widget_insert_action_group(application_root, "tuxrup", G_ACTION_GROUP(action_group));

    GSimpleAction* edit_properties_action = g_simple_action_new("editproperties", NULL);
    GSimpleAction* edit_styling_action = g_simple_action_new("editstyling", NULL);
    GSimpleAction* edit_callbacks_action = g_simple_action_new("editcallbacks", NULL);
    GSimpleAction* delete_widget_action = g_simple_action_new("deletewidget", NULL);
    g_signal_connect_data_ORIGINAL(edit_properties_action, "activate", G_CALLBACK(on_open_edit_properties_window), NULL, NULL, (GConnectFlags)0);
    g_signal_connect_data_ORIGINAL(edit_styling_action, "activate", G_CALLBACK(on_open_edit_styling_window), NULL, NULL, (GConnectFlags)0);
    g_signal_connect_data_ORIGINAL(edit_callbacks_action, "activate", G_CALLBACK(on_open_edit_callbacks_window), NULL, NULL, (GConnectFlags)0);
    g_signal_connect_data_ORIGINAL(delete_widget_action, "activate", G_CALLBACK(on_remove_widget), NULL, NULL, (GConnectFlags)0);
    g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_properties_action));
    g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_styling_action));
    g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(edit_callbacks_action));
    g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(delete_widget_action));
}

//Callback that is called when right-clicking a widget
void open_right_click_context_menu(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data){
    GtkWidget* widget = (GtkWidget*)user_data;
    active_widget = widget;

    // Create context menu if it does not exist
    if(context_menu_popover == NULL){
        initialize_right_click_context_menu();
        gtk_widget_set_parent(context_menu_popover, application_root);
    }

    // Set position to the current widget you right clicked on
    graphene_rect_t bounds;
    gtk_widget_compute_bounds(active_widget, application_root, &bounds);
    GdkRectangle rect = { bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height };
    gtk_popover_set_pointing_to(GTK_POPOVER(context_menu_popover), &rect);

    // Actually open/show the context menu
    gtk_popover_popup(GTK_POPOVER(context_menu_popover));
}
