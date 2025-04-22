normal:
	gcc -g $(shell pkg-config --cflags gtk4) hello_world.c -o hw $(shell pkg-config --libs gtk4) -lelf -ldl

debug:
	gcc -g $(shell pkg-config --cflags gtk4) hello_world.c -o hw_stripped $(shell pkg-config --libs gtk4) -lelf -ldl
	objcopy --only-keep-debug hw_stripped hw.debug
	strip --strip-debug --strip-unneeded hw_stripped 
	

