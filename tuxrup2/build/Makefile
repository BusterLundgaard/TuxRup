all:
	gcc -g -w -fPIC -shared $(shell pkg-config --cflags --libs gtk+-3.0) ../src/callbacks.c ../src/io.c ../src/symbols.c ../src/globals.c ../src/properties.c ../src/css.c ../src/util.c ../src/main.c -o libtuxrup.so
	LD_PRELOAD=./libtuxrup.so ../../tests/test1/test1
build:
	gcc -g -w -fPIC -shared $(shell pkg-config --cflags --libs gtk+-3.0) ../src/callbacks.c ../src/io.c ../src/symbols.c ../src/globals.c ../src/properties.c ../src/css.c ../src/util.c ../src/main.c -o libtuxrup.so
debug:
	gcc -g -w -fPIC -shared $(shell pkg-config --cflags --libs gtk+-3.0) ../src/callbacks.c ../src/io.c ../src/symbols.c ../src/globals.c ../src/properties.c ../src/css.c ../src/util.c ../src/main.c -o libtuxrup.so
	LD_PRELOAD=./libtuxrup.so gdb ../../tests/test1/test1

