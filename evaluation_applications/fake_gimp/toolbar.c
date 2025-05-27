#include "toolbar.h"
#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

typedef struct{
    char* menu_name;
    char* menu_items[5];
} Toolbar_menu_data;

Toolbar_menu_data menus[5] = {
    {"File",  {"Save", "Save As", "Import", "Export", "Quit"}},
    {"Edit",  {"Undo", "Redo", "Cut", "Copy", "Paste"}},
    {"Image", {"Resize", "Crop", "Rotate", "Flip", "Adjust"}},
    {"Layer", {"New Layer", "Duplicate Layer", "Merge Down", "Delete Layer", "Layer Properties"}},
    {"Help",  {"Documentation", "Keyboard Shortcuts", "Check for Updates", "Report Bug", "About"}},
};

void do_nothing(GSimpleAction *action, GVariant *parameter, GApplication *application){
}

GtkWidget* create_toolbar(GApplication* application){
	GtkWidget *toolbar = gtk_toolbar_new();

    GtkToolItem *toolitem;
    GtkToolItem *separator;
    GSList *group;

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_tool_button_new(NULL, menus[i].menu_items[j]), i*5 + j);
		}
		gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_separator_tool_item_new(), (i+1)*5);
	}	
    /* toolitem = gtk_tool_button_new_from_stock(GTK_STOCK_NEW); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, 0); */
    /* toolitem = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, 1); */
    /* separator = gtk_separator_tool_item_new(); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), separator, 2); */
    /* toolitem = gtk_toggle_tool_button_new_from_stock(GTK_STOCK_MEDIA_PLAY); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, 3); */
    /* separator = gtk_separator_tool_item_new(); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), separator, 4); */
    /* toolitem = gtk_menu_tool_button_new_from_stock(GTK_STOCK_GO_BACK); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, 5); */
    /* toolitem = gtk_menu_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, 6); */
    /* separator = gtk_separator_tool_item_new(); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), separator, 7); */
    /* toolitem = gtk_radio_tool_button_new_from_stock(NULL, GTK_STOCK_ADD); */
    /* group = gtk_radio_tool_button_get_group(GTK_RADIO_TOOL_BUTTON(toolitem)); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, 8); */
    /* toolitem = gtk_radio_tool_button_new_from_stock(group, GTK_STOCK_REMOVE); */
    /* gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolitem, 9); */

	return toolbar;

	/* GMenu* menu_bar = g_menu_new(); */
    /* for(int i = 0; i < 5; i++){ */
    /*     GMenu* menu = g_menu_new(); */
    /*     g_menu_append_submenu ( menu_bar, menus[i].menu_name, G_MENU_MODEL(menu)); */
    
    /*     for(int j = 0; j < 5; j++){ */
			/* gchar *action_name = g_strdup_printf("menuitem_%d_%d", i, j); */	
			/* gchar *detailed_action_name = g_strdup_printf("app.%s", action_name); */
			
    /*         GMenuItem* menu_item = g_menu_item_new(menus[i].menu_items[j], detailed_action_name); */
    /*         g_menu_append_item(menu, menu_item); */

			/* // Create and insert the action */
    /*         GSimpleAction *action = g_simple_action_new(action_name, NULL); */
    /*         g_signal_connect(action, "activate", G_CALLBACK(do_nothing), NULL); */
    /*         g_action_map_add_action(G_ACTION_MAP(GTK_APPLICATION(application)), G_ACTION(action)); */
    /*         /1* g_object_unref(action); *1/ */

    /*         // Free temporary strings */
    /*         /1* g_free(detailed_action_name); *1/ */
    /*     } */
    /* } */
	/* return menu_bar; */
}

/* void enable_toolbar(GtkApplication* app, GtkWindow* win, GMenu* menu_bar){ */
/* 	gtk_application_set_menubar(app, G_MENU_MODEL(menu_bar)); */
/* 	gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(win), TRUE); */
/* } */
