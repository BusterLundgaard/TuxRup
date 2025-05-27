#include "globals.h"
#include "image_viewer.h"

int images_numbers[3] = {0, 1, 2};
int image_margins[3] = {0, 0, 0};
GtkWidget* images[3] = {};
GtkWidget* fixed[3] = {};

int move_left_tool = 4;
int move_right_tool = 5;

static void move_image(bool left){
	printf("selected_layer = %d\n", selected_layer);
	if(selected_layer < 0 || selected_layer > 2){return;}

    image_margins[selected_layer] += left ? 20 : -20;
	gtk_fixed_move(GTK_FIXED(fixed[selected_layer]), images[selected_layer], image_margins[selected_layer], 0);
}

void on_move_image_left(GtkWidget* widget, gpointer user_data){
	move_image(true);
}
void on_move_image_right(GtkWidget* widget, gpointer user_data){
	move_image(false);
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
	gtk_container_add(GTK_CONTAINER(imageviewer), overlay);	

    for (int i = 0; i < 3; i++) {
        char* image_name = g_strdup_printf("jump%d.png", i+1);
        images[i] = gtk_image_new_from_file(image_name);
        gtk_image_set_pixel_size(GTK_IMAGE(images[i]), 300);

		fixed[i] = gtk_fixed_new();
		gtk_fixed_put(GTK_FIXED(fixed[i]), images[i], 0, 0);

		if(i == 0){
			gtk_container_add(GTK_CONTAINER(overlay), fixed[i]);
		} else {
			gtk_overlay_add_overlay(GTK_OVERLAY(overlay), fixed[i]);
		}

    }
    
    // Spinbutton at the bottom of the box (fixed height)
    GtkAdjustment *adjustment = gtk_adjustment_new(0, 0, 10, 1, 10, 0);
    GtkWidget* spinbutton = gtk_spin_button_new(adjustment, 1, 0);
    gtk_widget_set_size_request(spinbutton, -1, 40);  // Fixed height (40px)

    g_signal_connect(spinbutton, "changed", G_CALLBACK(on_zoom_changed), NULL);

	gtk_container_add(GTK_CONTAINER(imageviewer), spinbutton);
    gtk_widget_set_valign(spinbutton, GTK_ALIGN_END);
    gtk_widget_set_vexpand(spinbutton, true);

	return imageviewer;
}
