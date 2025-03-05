#ifndef FIX_FUNCTION_BODY_H
#define FIX_FUNCTION_BODY_H

#include <gtk/gtk.h>

// Takes some code and a hash table of type information about the undefined identifiers in that code (identifiers from the original document that the code needs)
// Produces a "fixed" function body with a prefix and a postfix where these undeclared indentifiers are declared by dereferencing pointers
char* create_fixed_function_body(char* modified_code, GHashTable* undefined_identifiers);

#endif