#ifndef POINTER_NAME_CONVERSION
#define POINTER_NAME_CONVERSION

bool has_debug_symbols();
void get_debug_symbols();
void *get_pointer_from_identifier(const char *name);
char *get_identifier_from_pointer(void *pointer);



#endif