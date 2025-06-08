all:
	gcc gtk-clock.c \
	-g $(shell pkg-config --cflags gtk+-3.0) \
	$(shell pkg-config --libs gtk+-3.0) \
	-o clock && \
	objcopy --only-keep-debug clock ../../tuxrup2/build/symbols/clock_symbols
