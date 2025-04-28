#define _GNU_SOURCE
#include <gtk-4.0/gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#define mymacrotype(t) t**
#define mymacroexp(varname) varname+2
typedef int my_own_int;
typedef int(*foo_type)(int);
struct {
  my_own_int some_int;
  mymacrotype(char) some_char_pointer;
};
typedef struct {
  my_own_int some_int;
  mymacrotype(char) some_char_pointer;
} my_struct;
 
 void customfunction(GtkWidget *widget, gpointer   data){
 //PREFIX: 
void** _data = (void**)data; 
    typedef int(*some_function_t)(int);
    some_function_t some_function = (some_function_t)(_data[0]);
    typedef my_own_int some_int_t;
    some_int_t some_int = *((some_int_t*)(_data[1]));

 //THEIR FUNCTION: 
    foo_type foo = &some_function;;
    ;;
    int local_var = mymacroexp(some_int);;
    ;;
    some_int = foo(local_var)*2;

//POSTFIX: 
    *((some_int_t*)(_data[1])) = some_int;
 
}