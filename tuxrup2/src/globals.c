#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "globals.h"

//------------------------------------
// GlOBALS
// -----------------------------------
GtkWidget* application_root = NULL; // the first window from the application we are modifying. Should remain constant.
GtkWidget* tuxrup_root = NULL; // the tuxrup window. Should remain constant.   
GtkWidget* selected_widget = NULL; //the currently selected widget.

g_signal_connect_data_t g_signal_connect_data_original = NULL;

