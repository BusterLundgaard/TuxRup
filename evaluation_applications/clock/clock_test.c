#include <gtk/gtk.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


//build command:
//gcc $(pkg-config --cflags gtk4) -o test-app gtk-test.c $(pkg-config --libs gtk4)

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
