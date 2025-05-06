#include <stddef.h>
void foo(int x, int y){
	typeof(foo) *foo_p = NULL;
	foo_p(2,3);
}

int main(){
	return 0;
}
