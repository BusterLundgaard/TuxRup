all:
	gcc -g $(shell pkg-config --cflags gtk4) -o clock gtk-clock.c $(shell pkg-config --libs gtk4)
gtk3:
	gcc -g $(shell pkg-config --cflags --libs gtk+-3.0) -DGTK3 -o clock gtk-clock.c 

