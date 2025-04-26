#include <gtk/gtk.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


//build command:
//gcc $(pkg-config --cflags gtk4) -o test-app gtk-test.c $(pkg-config --libs gtk4)

int hour = 12;
int minut = 0;
int diff = 0;
char amPm[] = "AM";

typedef struct {
    GtkWidget *label;
    int counter;
} CountdownData;

//am/pm switch method
void switchAmPm(GtkWidget *button, gpointer data){
    if (strcmp("AM", amPm) == 0)
    {
        amPm[0] = 'P';
        amPm[1] = 'M';
    }
    else if (strcmp("PM", amPm) == 0)
    {
        amPm[0] = 'A';
        amPm[1] = 'M';
    }
    GtkLabel *label = GTK_LABEL(data);
    gtk_label_set_text(label, amPm);
}

//increment hour by 1
void incHour(GtkWidget *buttonClicked, GtkWidget *buttonChanged){
    hour++;
    if(hour > 12){
        hour = 1;
    }
    char str[20];
    sprintf(str, "%d", hour);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);
}
//decrement hour by 1
void decHour (GtkWidget *buttonClicked, GtkWidget *buttonChanged){
    hour--;
    if(hour < 1){
        hour = 12;
    }
    char str[20];
    sprintf(str, "%d", hour);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);
}
//TODO makeit so that the inc-minut methods can also change the hour label, when over 59
//increment minut by 1
void incMinut (GtkWidget *buttonClicked, GtkWidget *buttonChanged){
    minut++;
    if(minut > 59){
        minut = 0;
        //incHour();
    }
    char str[20];
    sprintf(str, "%d", minut);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);
}
//decrement minut by 1
void decMinut (GtkWidget *buttonClicked, GtkWidget *buttonChanged){
    minut--;
    if(minut < 0){
        minut = 59;
        //decHour();
    }
    char str[20];
    sprintf(str, "%d", minut);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);
}

gboolean countdown(gpointer data){
    GtkLabel *label = GTK_LABEL(data);
    char buffer[32];
    diff--;
    snprintf(buffer, sizeof(buffer), "%d", diff);
    gtk_label_set_text(label, buffer);
    if (diff == 0){
        return FALSE;
    }
    return TRUE;
}

void timer(GtkWidget *button, gpointer data) {
    GtkLabel *label = GTK_LABEL(data);
    char buffer[32];
    time_t timeNow;
    time(&timeNow);
    //convert am/pm time to 24h time
    int hour24 = 0;
    if (strcmp("AM", amPm) == 0)
    {
        hour24 = hour;
    }
    else if (strcmp("PM", amPm) == 0)
    {
        hour24 = hour + 12;
    }
    //setting the time we want to reach
    struct tm *setTimeInfo;
    setTimeInfo = localtime(&timeNow);
    setTimeInfo->tm_hour = hour24;
    setTimeInfo->tm_min = minut;
    setTimeInfo->tm_sec = 0;
    time_t setTime = mktime(setTimeInfo);

    diff = (int)setTime - (int)timeNow;
    if (diff < 1) {
        fprintf(stderr, "Invalid input! Please enter a time that is NOT in the past!\n");
        return;
    }
    snprintf(buffer, sizeof(buffer), "%d", diff);
    gtk_label_set_text(label, buffer);
    g_timeout_add(1000, countdown, label);
}


static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Hello");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  //create labels and dummy buttons
  GtkWidget *hourLabel = gtk_button_new_with_label("0");
  GtkWidget *minutLabel = gtk_button_new_with_label("0");
  GtkWidget *amPmLabel = gtk_label_new("AM");
  GtkWidget *timerLabel = gtk_label_new("0");
  
  // Create inchour button
  GtkWidget* button0 = gtk_button_new_with_label ("ʌ");
  g_signal_connect (button0, "clicked", G_CALLBACK (incHour), hourLabel);

  // Create dechour button
  GtkWidget* button1 = gtk_button_new_with_label ("v");
  g_signal_connect (button1, "clicked", G_CALLBACK (decHour), hourLabel);

  // Create incminut button
  GtkWidget* button2 = gtk_button_new_with_label ("ʌ");
  g_signal_connect (button2, "clicked", G_CALLBACK (incMinut), minutLabel);

  // Create decminut button
  GtkWidget* button3 = gtk_button_new_with_label ("v");
  g_signal_connect (button3, "clicked", G_CALLBACK (decMinut), minutLabel);

  // Create set button
  GtkWidget* button4 = gtk_button_new_with_label ("Set");
  g_signal_connect (button4, "clicked", G_CALLBACK (timer), timerLabel);

  // Create am/pm button
  GtkWidget* button5 = gtk_button_new_with_label ("am/pm");
  g_signal_connect (button5, "clicked", G_CALLBACK (switchAmPm), amPmLabel);




  //create vertical box
  GtkWidget *V_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_halign(V_box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(V_box, GTK_ALIGN_CENTER);

  //create horizontal boxes
  GtkWidget *H_box0 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *H_box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *H_box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *H_box3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *H_box4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *H_box5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  //aligning set button
  gtk_widget_set_halign(H_box4, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(H_box4, GTK_ALIGN_CENTER);

  //add H boxes to V box
  gtk_box_append(GTK_BOX(V_box), H_box0);
  gtk_box_append(GTK_BOX(V_box), H_box1);
  gtk_box_append(GTK_BOX(V_box), H_box2);
  gtk_box_append(GTK_BOX(V_box), H_box3);
  gtk_box_append(GTK_BOX(V_box), H_box4);
  gtk_box_append(GTK_BOX(V_box), H_box5);
  
  //add buttons to H boxes
  gtk_box_append(GTK_BOX(H_box0), button0);
  gtk_box_append(GTK_BOX(H_box0), button2);
  gtk_box_append(GTK_BOX(H_box2), button1);
  gtk_box_append(GTK_BOX(H_box2), button3);
  gtk_box_append(GTK_BOX(H_box3), button5);
  gtk_box_append(GTK_BOX(H_box4), button4);

  //add labels and dummy buttons to H boxes
  gtk_box_append(GTK_BOX(H_box1), hourLabel);
  gtk_box_append(GTK_BOX(H_box1), minutLabel);
  gtk_box_append(GTK_BOX(H_box3), amPmLabel);
  gtk_box_append(GTK_BOX(H_box5), timerLabel);

  gtk_window_set_child (GTK_WINDOW (window), V_box);
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


