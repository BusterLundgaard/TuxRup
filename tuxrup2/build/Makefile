all:
	gcc -fPIC -shared $(shell pkg-config --cflags --libs gtk+-3.0) ../src/util.c ../src/main.c -o libtuxrup.so
	LD_PRELOAD=./libtuxrup.so ../../tests/test1/test1
