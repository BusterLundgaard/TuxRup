#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#define stupid_ass_macro char**
struct {
  int wow;
  long int more_wow;
};
typedef struct {
  int wow;
  long int more_wow;
} my_epic_struct;
typedef int my_own_int;
 
 void button_A_callback(GtkWidget *widget, gpointer   data){
 //PREFIX: 
void** _data = (void**)data; 
    typedef void(*some_original_function_t)(int);
    some_original_function_t some_original_function = (some_original_function_t)(_data[1]);
    typedef int some_original_variable_t;
    some_original_variable_t some_original_variable = *((some_original_variable_t*)(_data[0]));

 //THEIR FUNCTION: 
    some_original_variable *= 2;
    some_original_function(some_original_variable);

//POSTFIX: 
    *((some_original_variable_t*)(_data[0])) = some_original_variable;
 
}