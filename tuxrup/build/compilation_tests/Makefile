all:
	gcc -g $(shell pkg-config --cflags gtk4) temp.c -o isolated $(shell pkg-config --libs gtk4)

includes:
	clang -g $(shell llvm-config --cflags) includes_test.c -o includes_test \
    $(shell llvm-config --libs --system-libs) -lclang

