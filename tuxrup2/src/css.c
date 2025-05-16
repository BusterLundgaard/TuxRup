

#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include "globals.h"
#include "util.h"



void on_done_clicked(GtkWidget *widget, gpointer user_data)
{   
    GtkTextBuffer *textbuffer = user_data;
    char* text = get_text_from_buffer(textbuffer);

    // Apply the css
    GtkWidget*  w = selected_widget;
    if(w == NULL) {return;}
    if(!GTK_IS_WIDGET(w)) {return;}
    apply_css_to_widget(w, text);

    // Binds css directly to gobject data field
    g_object_set_data_full(G_OBJECT(w),"tuxrupcss",g_strdup(text),g_free); 
}

void on_load(GtkWidget *widget, gpointer user_data) {
    GtkTextBuffer *textbuffer = user_data;
    GtkWidget* w = selected_widget;
    if(w == NULL) {return;}
    if(!GTK_IS_WIDGET(w)) {return;}
    g_print("getting data");
    gpointer csspointer = g_object_get_data(G_OBJECT(w),"tuxrupcss");
    g_print("Data is %s",csspointer);

    if(csspointer == NULL) {
        return;
    }
    gtk_text_buffer_set_text(textbuffer, csspointer, -1);

}
