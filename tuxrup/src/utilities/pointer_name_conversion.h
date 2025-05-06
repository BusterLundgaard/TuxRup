#ifndef POINTER_NAME_CONVERSION
#define POINTER_NAME_CONVERSION

void initialize_debugging_symbols(char* executable_path, char* program_name);
bool has_debugging_symbols();

void *get_pointer_from_identifier(const char *name);
char *get_identifier_from_pointer(void *pointer);

void initialize_shared_lib(char* shared_lib_path, void* shared_lib_dl_open_pointer);
void sync_shared_variables_to_main();
void sync_main_variables_to_shared(); 
#endif
