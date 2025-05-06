#include <stdlib.h>
#include <stdio.h>

static int x = 20;
static int y;
static int z = 30;

static void goo1(int arg1, int arg2){
	printf("arg1 + arg2 = %d\n", arg1+arg2);
}

static void goo2(char x, char y){
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

static void goo4(int arg1, char arg2);

void foo(int arg1, void* data){
	if(x + y > 25){
		goo1(x,y);
		goo4(x,y);
	}
}
