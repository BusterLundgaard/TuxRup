#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

static int x = 20; 
static int y = 0;
static int z = 30; 

static __attribute__((noinline)) void goo1(int arg1, int arg2){
	typeof(goo1) goo1_fun = pointer_from_identifier("goo1");
	goo1(arg1, arg2);
}

static __attribute__((noinline)) void goo2(char x, char y){ 
	typeof(goo1) goo1_fun = pointer_from_identifier("goo1");
	goo1(arg1, arg2);
}

static __attribute__((noinline)) void goo3(int arg1, char arg2){
	typeof(goo1) goo1_fun = pointer_from_identifier("goo1");
	goo1(arg1, arg2);
}

static __attribute__((noinline)) void goo4(int arg1, char arg2){
	typeof(goo1) goo1_fun = pointer_from_identifier("goo1");
	goo1(arg1, arg2);
}


void dependency(){
	x = 0;
	y = 0;
	z = 0;
}

void foo(int arg1, void* data){
	sync_variables_from_main();

	if(strcmp("abc", "acd") != 0){
		printf("Hello!\n");	
	} 
	else {
		printf("Opposite hello!\n");
	}

    if(x + y > 25){
        goo1(x,y);
        goo4(x,y);
    }

	sync_main_to_variables();	
}
