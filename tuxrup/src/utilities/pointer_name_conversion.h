#ifndef POINTER_NAME_CONVERSION
#define POINTER_NAME_CONVERSION
void initialize_debugging_symbols(char* executable_path);
bool has_debugging_symbols();
bool main_contains_symbol(char* symbol);

void *get_pointer_from_identifier(const char *name);
char *get_identifier_from_pointer(void *pointer);

void initialize_shared_lib(char* shared_lib_path, void* shared_lib_dl_open_pointer);
GHashTable* get_identifiers(char* symbol_path);

void sync_shared_variables_to_main(bool direction);
#endif
