#include <gtk-3.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

static int x = 2;
static int y = 3;

static int double_number(int number){
	return number*2;
}

static void show(GtkWidget* widget, gpointer data){
	g_print("%d\n", x);	
}

static void increment(GtkWidget* my_cool_widget, gpointer data){
	x = double_number(x);
	show(NULL, NULL);
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *button_A = gtk_button_new_with_label("A");
  GtkWidget *button_B = gtk_button_new_with_label("B");

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "HelloWorldGtkApplication");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

	gtk_window_set_child(GTK_WINDOW(window), box);  
	gtk_box_append(GTK_BOX(box), button_A);  
	gtk_box_append(GTK_BOX(box), button_B);  

	g_signal_connect(button_A, "clicked", G_CALLBACK(increment), NULL);
	g_signal_connect(button_B, "clicked", G_CALLBACK(show), NULL);

  gtk_window_present (GTK_WINDOW (window));
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
