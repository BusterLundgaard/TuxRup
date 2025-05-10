#include <gtk-4.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

static int x = 2;

static void increment(GtkWidget* my_cool_widget, gpointer data){
	x++;
}
static void show(GtkWidget* widget, gpointer data){
	g_print("%d\n", x);	
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *button_A;
  GtkWidget *button_B;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "HelloWorldGtkApplication");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  increment(button_A, NULL);
  show(button_A, NULL);

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
