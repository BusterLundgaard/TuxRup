#include "globals.h"
#include "layers.h"
#include "image_viewer.h"

#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

int layers[3] = {0, 1, 2};
GtkWidget* layer_buttons[3] = {};
GtkWidget* opacity_slider;

int selected_layer = 0;
GtkWidget* images[3] = {};
float opacities[3] = {0,0,0};

static void on_layer_select(GtkWidget* self, gpointer data){
    selected_layer = *((int*)data);
    for(int i = 0; i < 3; i++){
        GtkStyleContext *context = gtk_widget_get_style_context(layer_buttons[i]);
        if(i == selected_layer){
            gtk_style_context_add_class(context, "selected");
        } else {
            gtk_style_context_remove_class(context, "selected");
        }
    }
}

static void on_opacity_changed(GtkSpinButton* spin, gpointer user_data){
	opacities[selected_layer] = gtk_spin_button_get_value(spin);		
}

void on_make_invisible(GtkWidget* widget, gpointer user_data){
g_print("hello!\n");	
gtk_widget_set_visible(images[selected_layer], !gtk_widget_get_visible(images[selected_layer])); 	
}

GtkWidget* create_layerpicker(){
    GtkWidget* layerpicker = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_vexpand(layerpicker, TRUE);

    // Buttons inside A2
    layer_buttons[0] = gtk_button_new_with_label("Layer 1");
    layer_buttons[1] = gtk_button_new_with_label("Layer 2");
    layer_buttons[2] = gtk_button_new_with_label("Layer 3");
   
    g_signal_connect(layer_buttons[0], "clicked", G_CALLBACK(on_layer_select), &layers[0]);
    g_signal_connect(layer_buttons[1], "clicked", G_CALLBACK(on_layer_select), &layers[1]);
    g_signal_connect(layer_buttons[2], "clicked", G_CALLBACK(on_layer_select), &layers[2]);

	gtk_container_add(GTK_CONTAINER(layerpicker), layer_buttons[0]);
	gtk_container_add(GTK_CONTAINER(layerpicker), layer_buttons[1]);
	gtk_container_add(GTK_CONTAINER(layerpicker), layer_buttons[2]);

	//Make invisible button
	GtkWidget* make_invisible = gtk_button_new_with_label("toggle invisible");
	gtk_container_add(GTK_CONTAINER(layerpicker), make_invisible);
	g_signal_connect(make_invisible, "clicked", G_CALLBACK(on_make_invisible), NULL);

	return layerpicker;
}
