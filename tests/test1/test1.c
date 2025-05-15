#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

// DESCRIBE THE TESTS HERE
bool tuxrup_test1();
bool tuxrup_test2();

void on_open_clicked(GtkWidget* widget, gpointer user_data){
	g_print("on open\n");
}

void on_new_clicked(GtkWidget* widget, gpointer user_data){
	g_print("on new\n");
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

	GtkWidget* button = gtk_button_new_with_label ("Bangladesh");

	GtkWidget* checkbutton = gtk_check_button_new();

	GtkWidget* scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0,100,1);
	gtk_widget_set_name(scale, "buster");

	GtkWidget* spinbutton = gtk_spin_button_new_with_range(0,100,1);
	GtkWidget* textentry = gtk_entry_new();
	GtkWidget* dropdown = gtk_combo_box_text_new();
	gtk_widget_set_name(dropdown, "super cool dropdown");

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

	// Toolbar
	GtkWidget *toolbar;
	GtkToolItem *new_button, *open_button, *sep;

	toolbar = gtk_toolbar_new();

	new_button = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	g_signal_connect(new_button, "clicked", G_CALLBACK(on_new_clicked), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_button, -1);

	open_button = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_clicked), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_button, -1);

	sep = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep, -1);
	gtk_container_add(GTK_CONTAINER(box), toolbar);

	gtk_widget_show_all(window);

	// TESTING
	tuxrup_test1();
	tuxrup_test2();
	g_print("ALL TESTS PASSED!\n");
	/* gtk_window_close(GTK_WINDOW(window)); */
}

int
main (int    argc,
      char **argv)
{
	g_print("i was called!!\n");
    GtkApplication *app;
    int status;
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
