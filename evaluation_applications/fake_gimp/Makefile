normal:
	gcc -w fakegimp.c \
	globals.c image_viewer.c layers.c toolbar.c toolpicker.c \
	-g $(shell pkg-config --cflags --libs gtk+-3.0) \
	-o fg && \
	objcopy --only-keep-debug fg ../../tuxrup2/build/symbols/fg_symbols
