#include <stddef.h>
/* #include <gtk/gtk.h> */
#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else
    #include <gtk-4.0/gtk/gtk.h>
#endif
#include "globals.h"
#include "toolpicker.h"

char* icons[25] = {"audio-volume-high","audio-volume-low","audio-volume-medium","audio-volume-muted",
    "go-previous","go-next",
    "edit-cut","edit-find-replace","edit-paste","object-rotate-left","object-rotate-right","insert-image","insert-text","mail-attachment","call-stop","audio-input-microphone","input-tablet","x-office-calendar","face-cool","document-print","dialog-password","semi-starred","dialog-information","help-browser","contact-new"};
int tools[25] = {};

GtkWidget* selected_tool_widget = NULL;

static void on_tool_select(GtkWidget* self, gpointer data){
    int selected_tool_number = *((int*)data);
    if(selected_tool_widget != NULL){
        gtk_style_context_remove_class(gtk_widget_get_style_context(selected_tool_widget), "selected");
    }
    selected_tool_widget = self;
	selected_tool = selected_tool_number;
    gtk_style_context_add_class(gtk_widget_get_style_context(selected_tool_widget), "selected");
}

GtkWidget* create_toolpicker(){	
    GtkWidget* toolpicker = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_vexpand(toolpicker, TRUE);

    for(int i = 0; i < 5; i++){
        GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        for(int j = 0; j < 5; j++){
            tools[5*i + j] = 5*i + j;
            GtkWidget* tool = gtk_button_new();
            gtk_button_set_icon_name(GTK_BUTTON(tool), icons[i*5+j]);
            g_signal_connect(tool, "clicked", G_CALLBACK(on_tool_select), &tools[5*i+j]);
            gtk_box_append(GTK_BOX(hbox), tool);
        }
        gtk_box_append(GTK_BOX(toolpicker),hbox);
	}

	return toolpicker;
}

