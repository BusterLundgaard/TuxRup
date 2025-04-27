normal:
	gcc -g $(shell pkg-config --cflags gtk4) hello_world.c -o hw $(shell pkg-config --libs gtk4) -lelf -ldl

gtk3:
	gcc -g $(shell pkg-config --cflags --libs gtk+-3.0) -D USE_GTK3 hello_world.c -o hw3

debug:
	gcc -g $(shell pkg-config --cflags gtk4) hello_world.c -o hw_stripped $(shell pkg-config --libs gtk4) -lelf -ldl
	objcopy --only-keep-debug hw_stripped hw.debug
	strip --strip-debug --strip-unneeded hw_stripped 
	

