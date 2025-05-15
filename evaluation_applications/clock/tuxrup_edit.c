#include <gtk/gtk.h>
#include <gtk/gtk.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
int hour = 12;
int minut = 0;
int diff = 0;
char amPm[] = "AM";
typedef struct {
    GtkWidget *label;
    int counter;
} CountdownData;
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
    };

}
//Modify this function!!
void incHour(GtkWidget *buttonClicked, GtkWidget *buttonChanged){
    hour++;
    if(hour > 24){
        hour = 1;
    };
    char str[20];;
    sprintf(str, "%d", hour);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);

}
void decHour(GtkWidget *buttonClicked, GtkWidget *buttonChanged){
    hour--;
    if(hour < 1){
        hour = 12;
    };
    char str[20];;
    sprintf(str, "%d", hour);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);

}
void incMinut(GtkWidget *buttonClicked, GtkWidget *buttonChanged){
    minut++;
    if(minut > 59){
        minut = 0;
        //incHour();
    };
    char str[20];;
    sprintf(str, "%d", minut);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);

}
void decMinut(GtkWidget *buttonClicked, GtkWidget *buttonChanged){
    minut--;
    if(minut < 0){
        minut = 59;
        //decHour();
    };
    char str[20];;
    sprintf(str, "%d", minut);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);

}
int countdown(gpointer data){
    char buffer[32];;
    diff--;
    snprintf(buffer, sizeof(buffer), "%d", diff);
    if (diff == 0){
        return FALSE;
    };

}
void timer(GtkWidget *button, gpointer data){
    char buffer[32];;
    time_t timeNow;;
    time(&timeNow);
    int hour24 = 0;;
    if (strcmp("AM", amPm) == 0)
    {
        hour24 = hour;
    }
    else if (strcmp("PM", amPm) == 0)
    {
        hour24 = hour + 12;
    };
    struct tm *setTimeInfo;;
    setTimeInfo = localtime(&timeNow);
    setTimeInfo->tm_hour = hour24;
    setTimeInfo->tm_min = minut;
    setTimeInfo->tm_sec = 0;
    time_t setTime = mktime(setTimeInfo);;
    diff = (int)setTime - (int)timeNow;
    if (diff < 1) {
        fprintf(stderr, "Invalid input! Please enter a time that is NOT in the past!\n");
        return;
    };
    snprintf(buffer, sizeof(buffer), "%d", diff);

}
void activate(GtkApplication *app, gpointer        user_data){

}
