#include <stdlib.h>
#include <stdio.h>

int main(){
	int result = system("gcc -g $(pkg-config --cflags gtk4) temp2.c -o isolated2 $(pkg-config --libs gtk4) -lelf -ldl");
	printf("result is %d\n", result);
	if(result != 0){ 
		printf("Compilation failed\n");
	} 
	else {
		printf("Compilation sucesss\n");
	}
	return 0;
}
