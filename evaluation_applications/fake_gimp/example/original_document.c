// ORIGINAL DOCUMENT
#include <gtk/gtk.h>
#include <stdlib.h>

#include "globals.h"
#include "events.h"

int foo(int x){
	return x+2
}

char* some_string = "this is a string";

void button_A_callback(GtkWidget* widget, gpointer data){
	some_strin = "this is a new string";
	foo(50);
}
