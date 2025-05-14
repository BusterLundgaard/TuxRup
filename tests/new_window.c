#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

static void new_window(GtkWidget* w, gpointer user_data){
	// Get parent application 
	GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_toplevel(w));
	GtkApplication* app = GTK_APPLICATION(gtk_window_get_application(parent_window));

	// Create the new window
	GtkWidget* window = gtk_application_window_new(app);
	
	// Add the stuff
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget* button = gtk_button_new_with_label ("button");
    GtkWidget* checkbutton = gtk_check_button_new();
    GtkWidget* scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0,100,1);
    GtkWidget* spinbutton = gtk_spin_button_new_with_range(0,100,1);
    GtkWidget* textentry = gtk_entry_new();
    GtkWidget* dropdown = gtk_combo_box_text_new();
    gtk_container_add(GTK_CONTAINER(box), button); 
    gtk_container_add(GTK_CONTAINER(box), checkbutton); 
    gtk_container_add(GTK_CONTAINER(box), scale); 
    gtk_container_add(GTK_CONTAINER(box), spinbutton); 
    gtk_container_add(GTK_CONTAINER(box), textentry); 
    gtk_container_add(GTK_CONTAINER(box), dropdown); 
 
    const gchar* dropdown_items[3] = {"wow", "damn", "cool"};
    for(int i = 0; i < 3; i++){
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), dropdown_items[i]);
    }

	// Show the window
  	gtk_widget_show_all(window);
}



static void
activate (GtkApplication *app,
          gpointer        user_data)
{
	  GtkWidget* window = gtk_application_window_new (app);
	  gtk_window_set_title (GTK_WINDOW (window), "HelloWorldGtkApplication");
	  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

	  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
	  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
	  gtk_container_add(GTK_CONTAINER(window), box);

	  GtkWidget* button = gtk_button_new_with_label ("button");
	  GtkWidget* checkbutton = gtk_check_button_new();
	  GtkWidget* scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0,100,1);
	  GtkWidget* spinbutton = gtk_spin_button_new_with_range(0,100,1);
	  GtkWidget* textentry = gtk_entry_new();
	  GtkWidget* dropdown = gtk_combo_box_text_new();
	  gtk_container_add(GTK_CONTAINER(box), button); 
	  gtk_container_add(GTK_CONTAINER(box), checkbutton); 
	  gtk_container_add(GTK_CONTAINER(box), scale); 
	  gtk_container_add(GTK_CONTAINER(box), spinbutton); 
	  gtk_container_add(GTK_CONTAINER(box), textentry); 
	  gtk_container_add(GTK_CONTAINER(box), dropdown); 
	 
	  const gchar* dropdown_items[3] = {"wow", "damn", "cool"};
	  for(int i = 0; i < 3; i++){
		  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dropdown), dropdown_items[i]);
	  }
	  
	  GtkWidget* new_window_button = gtk_button_new_with_label("make new window");
	  gtk_container_add(GTK_CONTAINER(box), new_window_button);
	  g_signal_connect(new_window_button, "clicked", G_CALLBACK(new_window), NULL);

	  gtk_widget_show_all(window);
}

int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int status;
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
