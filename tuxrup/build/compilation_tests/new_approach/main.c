#include <dlfcn.h>
#include <string.h>
#include <stdio.h>

typedef void (*foo_type)(int, void*);
typedef int (*strcmp_type)(const char*, const char*);

int sus(int x, char y){
	typeof(sus) *poo = &sus;
	sus(x,y);
}

int main(){
	strcmp_type p = (strcmp_type)dlsym(RTLD_NEXT, "strcmp");
	if(p("hello", "helloooooo") == 0){
		printf("Motherfucker it worked!!\n");	
	}
	
	/* void* p = dlopen("./original.so", RTLD_LAZY); */
	/* foo_type foo = (foo_type)dlsym(p, "foo"); */
	/* foo(20, p); */
	return 0;
}
