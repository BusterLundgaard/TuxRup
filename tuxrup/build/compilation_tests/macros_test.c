#include <gtk/gtk.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
struct {
    GtkWidget *label;
    int counter;
};
typedef struct {
    GtkWidget *label;
    int counter;
} CountdownData;
 
 void customfunction(GtkWidget *buttonClicked, gpointer data){
    hour++;
    if(hour > 12){
        hour = 1;
    }
    char str[20];
    sprintf(str, "%d", hour);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);
}
