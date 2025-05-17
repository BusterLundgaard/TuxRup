normal:
	gcc -g $(shell pkg-config --cflags gtk3) \
	globals.c image_viewer.c layers.c toolbar.c toolpicker.c \
	fakegimp.c -o fg $(shell pkg-config --libs gtk3)\&&
	objcopy --only-keep-debug fg ../../tuxrup2/build/symbols/fg.debug

