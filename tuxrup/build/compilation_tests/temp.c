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
 //PREFIX: 
void** _data = (void**)data; 
GtkWidget *buttonChanged = (gpointer*)(_data[0]);
    typedef int hour_t;
    hour_t hour = *((hour_t*)(_data[1]));
    typedef int(*GTK_BUTTON_t)();
    GTK_BUTTON_t GTK_BUTTON = (GTK_BUTTON_t)(_data[4]);

 //THEIR FUNCTION: 
    {
    hour++;
    if(hour > 12){
        hour = 1;
    }
    char str[20];
    sprintf(str, "%d", hour);
    gtk_button_set_label(GTK_BUTTON(buttonChanged), str);
};

//POSTFIX: 
    *((hour_t*)(_data[0])) = hour;
 
}