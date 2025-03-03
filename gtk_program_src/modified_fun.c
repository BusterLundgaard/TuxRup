#define _GNU_SOURCE
#include <execinfo.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <libelf.h>
#include <fcntl.h>
#include <gelf.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
stupid_ass_macro char**
;struct my_epic_struct{
  int wow;
  long int more_wow;
}
;typedef int my_own_int
;void button_A_callback (GtkWidget* widget, gpointer data){
 //PREFIX: 
     typedef void(*some_original_function_t)(int);
    some_original_function_t some_original_function = *((some_original_function_t*)data[1]);
    typdef int some_original_variable_t;
    some_original_variable_t some_original_variable = *((some_original_variable_t*)data[0]);

 //THEIR FUNCTION: 
    some_original_variable++
    some_original_function(some_original_variable)

//POSTFIX: 
    *((some_original_variable_t*)data[0]) = some_original_variable;
} 