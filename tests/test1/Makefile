all:
	gcc fake_test.c -fPIC -shared -o libfake_test.so &&\
	gcc $(shell pkg-config --cflags --libs gtk+-3.0) -L. -lfake_test -Wl,-rpath=../../tests/test1 test1.c -g -o test1
