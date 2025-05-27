#include <stddef.h>
#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include "globals.h"
#include "toolpicker.h"
#include "image_viewer.h"

char* icons[25] = {
	"audio-volume-high","audio-volume-low","audio-volume-medium","audio-volume-muted",
    "go-previous","go-next",
    "edit-cut","edit-find-replace","edit-paste","object-rotate-left","object-rotate-right","insert-image","insert-text","mail-attachment","call-stop","audio-input-microphone","input-tablet","x-office-calendar","face-cool","document-print","dialog-password","semi-starred","dialog-information","help-browser","contact-new"};

GtkWidget* create_toolpicker(){	
    GtkWidget* toolpicker = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_vexpand(toolpicker, TRUE);

    for(int i = 0; i < 5; i++){
        GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        for(int j = 0; j < 5; j++){
            GtkWidget* tool = gtk_button_new();
            gtk_button_set_image(GTK_BUTTON(tool), gtk_image_new_from_icon_name(icons[i*5+j], GTK_ICON_SIZE_BUTTON));
			gtk_container_add(GTK_CONTAINER(hbox), tool);

			if(icons[i*5 + j] == "go-previous"){
				g_signal_connect(tool, "clicked", G_CALLBACK(on_move_image_right), NULL);
			}
			else if(icons[i*5 + j] == "go-next"){
				g_signal_connect(tool, "clicked", G_CALLBACK(on_move_image_left), NULL);
			}
        }
		gtk_container_add(GTK_CONTAINER(toolpicker), hbox);
	}

	return toolpicker;
}

