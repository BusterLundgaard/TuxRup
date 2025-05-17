all:
	gcc -g $(shell pkg-config --cflags --libs gtk+-3.0) -DGTK3 -o clock gtk-clock.c &&\
	objcopy --only-keep-debug clock ../../tuxrup2/build/symbols/clock.debug	

