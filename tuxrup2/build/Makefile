all:
	gcc -w -fPIC -shared $(shell pkg-config --cflags --libs gtk+-3.0) ../src/globals.c ../src/properties.c ../src/css.c ../src/util.c ../src/main.c -o libtuxrup.so
	LD_PRELOAD=./libtuxrup.so ../../tests/test1/test1
