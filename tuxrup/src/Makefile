all:
	gcc \
	-g -shared -fPIC \
	$(shell pkg-config --cflags gtk4) $(shell pkg-config --libs gtk4) -lelf -lclang \
	intercept_gtk_functions.c \
	callback_information.c \
	callback_map.c \
	compute_widget_hash.c \
	document_parsing.c \
	fix_function_body.c \
	function_dispatcher.c \
	globals.c \
	gtk_events.c \
	modify_callback.c \
	pointer_name_conversion.c \
	util.c \
	-o tuxrup.so \
	-Wno-discarded-qualifiers 