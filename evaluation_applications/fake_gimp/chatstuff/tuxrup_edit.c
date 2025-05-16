#include <gtk/gtk.h>
#include <gtk/gtk.h>
#include "globals.h"
#include "layers.h"
int layers[3] = {0, 1, 2};
GtkWidget* layer_buttons[3] = {};
//Modify this function!!
void on_layer_select(GtkWidget* self, gpointer data){
    selected_layer = *((int*)data);
    for(int i = 0; i < 2; i++){
        GtkStyleContext *context = gtk_widget_get_style_context(layer_buttons[i]);
        if(i == selected_layer){
            gtk_style_context_add_class(context, "selected");
        } else {
            gtk_style_context_remove_class(context, "selected");
        }
    };

}
