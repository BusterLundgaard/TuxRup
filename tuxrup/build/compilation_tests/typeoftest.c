#include <stdlib.h>
#include <stdio.h>

int** some_pointer;
long int do_stuff(char x, char* p);

int main(){
	typeof(some_pointer) some_pointer = NULL;
	typeof(do_stuff) *do_stuff = NULL;
	return 0;
}
