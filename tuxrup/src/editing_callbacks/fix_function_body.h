#ifndef FIX_FUNCTION_BODY_H
#define FIX_FUNCTION_BODY_H
#include <stdlib.h>
#include <stdio.h>

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include "reference_type.h"
#include "document_parsing.h"

// Takes some code and a hash table of type information about the undefined identifiers in that code (identifiers from the original document that the code needs)
// Produces a "fixed" function body with a prefix and a postfix where these undeclared indentifiers are declared by dereferencing pointers
char* create_fixed_function_body(char* modified_code, GHashTable* undefined_identifiers, function_arguments args);

#endif
