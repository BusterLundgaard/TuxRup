all:
	gcc fake_test.c -fPIC -shared -o libfake_test.so &&\
	gcc test1.c	  $(shell pkg-config --cflags --libs gtk+-3.0) -Wl,-rpath=../../tests/test1 -g -o test1 -L. -lfake_test &&\
	objcopy --only-keep-debug test1 ../../tuxrup2/build/symbols/test1.debug
