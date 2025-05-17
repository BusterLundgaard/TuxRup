#ifndef SYMBOLS_H
#define SYMBOLS_H
#include "io.h"

GHashTable* get_main_symbols();

char* identifier_from_pointer(void* pointer);
char* pointer_from_identifier(char* identifier);

void sync_variables(char* shared_lib_path, void* shared_lib_dl_open_pointer, bool to_from);
#endif
