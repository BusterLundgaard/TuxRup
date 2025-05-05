#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
	FILE* pipe = popen("gcc $(pkg-config --cflags gtk4) macros_expanded.c -o errors $(pkg-config --libs gtk4) 2>&1", "r");

	char line[512];
    char name[128];

    while (fgets(line, sizeof(line), pipe)) {
		if (strstr(line, "undeclared")) {
       		 if (sscanf(line, "%*[^‘]‘%[^’]’ undeclared", name) == 1) {
       		     printf("Found undeclared: %s\n", name);
       		 }
		}
    }	
}

