#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#define stupid_ass_macro char**
stupid_ass_macro;
stupid_ass_macro;
char** second_var;
void (*my_func_ptr)         (int, stupid_ass_macro);
char* some_complicated_function(
  int ass, 
  stupid_ass_macro ass2, 
  gpointer ass3){};
int some_original_variable = 20;
void some_original_function(int var) {
  g_print("Hi! I am some_original_function and now i will print: %d\n", var);
};
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
     some_original_variable *= 2;
    some_original_function(some_original_variable);
 
}