#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

static int x = 20; 
static int y = 0;
static int z = 30; 

/* extern int dlclose (void *__handle) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))){ */
/* 	return 1; */
/* } */
int strcmp(const char *s1, const char *s2){
	return 0;
}

static __attribute__((noinline)) void goo1(int arg1, int arg2){
	dlclose(NULL);
    printf("arg1 + arg2 = %d\n", arg1+arg2);
}

static __attribute__((noinline)) void goo2(char x, char y){ 
    char* str = malloc(sizeof(char)*2);
    *str = x;
    *(str + 1) = y;
    printf("concat is %s\n", str);
}

static void goo3(int arg1, char arg2){
    if(arg2 == 'a'){
        printf("%d\n", arg1);
    }   
}

static __attribute__((noinline)) void goo4(int arg1, char arg2){
    printf("arg1 + arg2 = %d\n", arg1+arg2);
}


void dependency(){
	x = 0;
	y = 0;
	z = 0;
}

void foo(int arg1, void* data){

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
}
