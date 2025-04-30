#include "globals.h"
#include "image_viewer.h"

int images_numbers[3] = {0, 1, 2};
int image_margins[3] = {0, 0, 0};
GtkWidget* images[3] = {};

int move_left_tool = 4;
int move_right_tool = 5;

static void on_image_click(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data){
    int clicked_image = *((int*)user_data);
    if(clicked_image != selected_layer){return;}
	
	bool left = selected_tool == move_left_tool;
	bool right = selected_tool == move_right_tool;
	if(left || right){	
        image_margins[clicked_image] += left ? 20 : -20;
		if(image_margins[clicked_image] > 0){
        	gtk_widget_set_margin_start(images[clicked_image], image_margins[selected_layer]);
		} else {
        	gtk_widget_set_margin_end(images[clicked_image], image_margins[selected_layer]);
		}
    }
}


static void on_zoom_changed(GtkSpinButton* spinbutton, gpointer data){
    double spin_button_value = gtk_spin_button_get_value(spinbutton);
    for (int i = 0; i < 3; i++) {
        gtk_image_set_pixel_size(GTK_IMAGE(images[i]), 300 + 50*spin_button_value);
    }
}


GtkWidget* create_imageviewer(){
    GtkWidget* imageviewer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_vexpand(imageviewer, true);

    // Images at the top of the right box
    GtkWidget *overlay = gtk_overlay_new();
    gtk_widget_set_halign(overlay, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(overlay, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(imageviewer), overlay); // Add the overlay to your layout box
    
    for (int i = 0; i < 3; i++) {
        char* image_name = g_strdup_printf("jump%d.png", i+1);
        images[i] = gtk_image_new_from_file(image_name);
        gtk_image_set_pixel_size(GTK_IMAGE(images[i]), 300);
    
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), images[i]);
    
        gtk_widget_set_halign(images[i], GTK_ALIGN_CENTER);
        gtk_widget_set_valign(images[i], GTK_ALIGN_CENTER);
    
        GtkGesture *gesture = gtk_gesture_click_new();
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 1);
        gtk_widget_add_controller(images[i], GTK_EVENT_CONTROLLER(gesture));
        g_signal_connect_data(gesture, "pressed", G_CALLBACK(on_image_click),
                              &images_numbers[i], NULL, (GConnectFlags)0);
    }
    
    // Spinbutton at the bottom of the box (fixed height)
    GtkAdjustment *adjustment = gtk_adjustment_new(0, 0, 10, 1, 10, 0);
    GtkWidget* spinbutton = gtk_spin_button_new(adjustment, 1, 0);
    gtk_widget_set_size_request(spinbutton, -1, 40);  // Fixed height (40px)

    g_signal_connect(spinbutton, "changed", G_CALLBACK(on_zoom_changed), NULL);

    gtk_box_append(GTK_BOX(imageviewer), spinbutton);
    gtk_widget_set_valign(spinbutton, GTK_ALIGN_END);
    gtk_widget_set_vexpand(spinbutton, true);

	return imageviewer;
}
