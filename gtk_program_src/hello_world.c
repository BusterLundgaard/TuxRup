#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>



#define stupid_ass_macro char**

char** second_var = NULL;
void (*my_func_ptr)         (int, stupid_ass_macro);

char* some_complicated_function(
  int ass, 
  stupid_ass_macro ass2, 
  gpointer ass3){}

int some_original_variable = 20;
void some_original_function(int var) {
  g_print("Hi! I am some_original_function and now i will print: %d\n", var);
}

typedef struct {
  int wow;
  long int more_wow;
} my_epic_struct;

typedef int my_own_int;

static void
button_A_callback 
(GtkWidget *widget,
 gpointer   data)
{
  some_original_variable++;
  some_original_function(some_original_variable);
}

static void
button_B_callback 
(GtkWidget *widget,
 gpointer   data)
{
  some_original_function(some_original_variable);
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button_A;
  GtkWidget *button_B;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Hello");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  button_A = gtk_button_new_with_label ("Button_A");
  gtk_widget_set_name(button_A, "button_A");
  g_signal_connect (button_A, "clicked", G_CALLBACK (button_A_callback), NULL);

  button_B = gtk_button_new_with_label ("Button_B");
  gtk_widget_set_name(button_B, "button_B");
  g_signal_connect (button_B, "clicked", G_CALLBACK (button_B_callback), NULL);

  // GtkGesture* gesture = gtk_gesture_click_new();
  // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3); //Maybe 3 needs to be changed to something else if right-click isn't working?
  // gtk_widget_add_controller(GTK_WIDGET(button_A), GTK_EVENT_CONTROLLER(gesture));
  // g_signal_connect(gesture, "pressed", G_CALLBACK(show_code_editor), button_A);

  // gesture = gtk_gesture_click_new();
  // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3); 
  // gtk_widget_add_controller(GTK_WIDGET(button_B), GTK_EVENT_CONTROLLER(gesture));
  // g_signal_connect(gesture, "pressed", G_CALLBACK(show_code_editor), button_B);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

  gtk_box_append(GTK_BOX(box), button_A);
  gtk_box_append(GTK_BOX(box), button_B);

  gtk_window_set_child(GTK_WINDOW(window), box);

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
