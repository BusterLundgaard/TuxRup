#include <gtk/gtk.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct {
    GtkWidget *label;
    int counter;
} CountdownData;

CountdownData ass = {};

int main(){
	printf("%d", ass);
	return 1;
}
