normal:
	gcc -g $(shell pkg-config --cflags gtk4) fakegimp.c -o fg $(shell pkg-config --libs gtk4) -lelf -ldl

debug:
	gcc -g $(shell pkg-config --cflags gtk4) fakegimp.c -o fg_stripped $(shell pkg-config --libs gtk4) -lelf -ldl
	objcopy --only-keep-debug fg_stripped fw.debug
	strip --strip-debug --strip-unneeded fg_stripped 
	

