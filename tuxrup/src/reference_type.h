#ifndef REFERENCE_TYPE_H
#define REFERENCE_TYPE_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include <stdbool.h>

typedef struct {
    int number;
    bool is_function;
    char* return_type;
    char* args_types;
} reference_type;

#endif