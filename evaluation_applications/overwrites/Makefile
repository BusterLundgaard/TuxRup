normal:
	gcc -g $(shell pkg-config --cflags --libs gtk+-3.0) -D USE_GTK3 ow.c -o ow &&\
	objcopy --only-keep-debug ow ../../tuxrup2/build/symbols/ow.debug
