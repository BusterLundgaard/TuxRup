all:	
	gcc -g -w -fPIC -shared -Wl,--no-as-needed $(shell pkg-config --cflags --libs gtk+-3.0 glib-2.0) \
  	../src/*.c -o libtuxrup.so
	$(MAKE) -C ../../tests/test1
	LD_PRELOAD=./libtuxrup.so ../../tests/test1/test1
build:
	gcc -g -w -fPIC -shared $(shell pkg-config --cflags --libs gtk+-3.0) ../src/callbacks.c ../src/io.c ../src/symbols.c ../src/globals.c ../src/properties.c ../src/css.c ../src/util.c ../src/main.c -o libtuxrup.so
debug:
	gcc -g -w -fPIC -shared $(shell pkg-config --cflags --libs gtk+-3.0) ../src/callbacks.c ../src/io.c ../src/symbols.c ../src/globals.c ../src/properties.c ../src/css.c ../src/util.c ../src/main.c -o libtuxrup.so
	LD_PRELOAD=./libtuxrup.so gdb ../../tests/test1/test1
clock:	
	gcc -g -w -fPIC -shared -Wl,--no-as-needed $(shell pkg-config --cflags --libs gtk+-3.0 glib-2.0) \
  	../src/*.c -o libtuxrup.so
	$(MAKE) -C ../../evaluation_applications/clock
	LD_PRELOAD=./libtuxrup.so ../../evaluation_applications/clock/clock
fakegimp:	
	gcc -g -w -fPIC -shared -Wl,--no-as-needed $(shell pkg-config --cflags --libs gtk+-3.0 glib-2.0) \
  	../src/*.c -o libtuxrup.so
	$(MAKE) -C ../../evaluation_applications/fake_gimp
	LD_PRELOAD=./libtuxrup.so ../../evaluation_applications/fake_gimp/fg

	