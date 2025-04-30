#include "toolbar.h"
#include <gtk/gtk.h>

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

GMenu* create_toolbar(GApplication* application){
    GMenu* menu_bar = g_menu_new();

    for(int i = 0; i < 5; i++){
        GMenu* menu = g_menu_new();
        g_menu_append_submenu ( menu_bar, menus[i].menu_name, G_MENU_MODEL(menu));
    
        for(int j = 0; j < 5; j++){
			gchar *action_name = g_strdup_printf("menuitem_%d_%d", i, j);	
			gchar *detailed_action_name = g_strdup_printf("app.%s", action_name);
			
            GMenuItem* menu_item = g_menu_item_new(menus[i].menu_items[j], detailed_action_name);
            g_menu_append_item(menu, menu_item);

			// Create and insert the action
            GSimpleAction *action = g_simple_action_new(action_name, NULL);
            g_signal_connect(action, "activate", G_CALLBACK(do_nothing), NULL);
            g_action_map_add_action(G_ACTION_MAP(GTK_APPLICATION(application)), G_ACTION(action));
            /* g_object_unref(action); */

            // Free temporary strings
            /* g_free(detailed_action_name); */
        }
    }
	return menu_bar;
}

void enable_toolbar(GtkApplication* app, GtkWindow* win, GMenu* menu_bar){
	gtk_application_set_menubar(app, G_MENU_MODEL(menu_bar));
	gtk_application_window_set_show_menubar(GTK_APPLICATION_WINDOW(win), TRUE);
}
