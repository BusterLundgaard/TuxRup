				#include <gtk/gtk.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
struct {
    GtkWidget *label;
    int counter;
};
typedef struct {
    GtkWidget *label;
    int counter;
} CountdownData;
 
				void noop(void* ident){} 
				int main(){ 
				noop(sprintf);
				return 0;
				}