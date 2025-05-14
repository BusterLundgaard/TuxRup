normal:
	gcc -g $(shell pkg-config --cflags gtk4) \
	globals.c image_viewer.c layers.c toolbar.c toolpicker.c \
	fakegimp.c -o fg $(shell pkg-config --libs gtk4)

gtk3:
	gcc -g -DUSE_GTK3 $(shell pkg-config --cflags --libs gtk+-3.0) \
	globals.c image_viewer.c layers.c toolbar.c toolpicker.c \
	fakegimp.c -o fg

debug:
	gcc -g $(shell pkg-config --cflags gtk4) fakegimp.c -o fg_stripped $(shell pkg-config --libs gtk4) -lelf -ldl
	objcopy --only-keep-debug fg_stripped fw.debug
	strip --strip-debug --strip-unneeded fg_stripped 
	

