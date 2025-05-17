#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <glib.h>
#include <ctype.h>
#include <link.h>

#include "io.h"

typedef struct {
	size_t size;
	void* address;
} symbol_info;

GHashTable* identifiers_to_pointers_map = NULL;
GHashTable* pointers_to_identifiers_map = NULL; 


bool is_variable(char symbol_type){
	return
		symbol_type == 'D' ||
		symbol_type == 'd' ||
		symbol_type == 'B' ||
		symbol_type == 'b' ||
		symbol_type == 'R' ||
		symbol_type == 'r' ||
		symbol_type == 'U' ||
		symbol_type == 'u' ||
		symbol_type == 'C' ||
		symbol_type == 'c';
	/* T: global function in .text */
	/* t: local function in .text */
	/* D: initialized data */
	/* B: uninitialized data */
	/* R: read-only data */
	/* U: undefined (external) symbol */
	/* C: common symbol (old-style uninitialized global) */
	/* so you should check after D, B, R, U, C */
}


GHashTable* get_main_symbols(){
	if(identifiers_to_pointers_map != NULL){
		return identifiers_to_pointers_map;
	}

	identifiers_to_pointers_map = g_hash_table_new(g_str_hash, g_str_equal);
	pointers_to_identifiers_map = g_hash_table_new(g_direct_hash, g_direct_equal);

    char command[512];
    snprintf(command, sizeof(command), "nm %s", get_executable_symbols_path());

	// Pass 1: Get the base (real, not relative) address you will use to compute the other addresses
	void* base_addr_absolute;
	void* base_addr_relative;

	FILE *fp = popen(command, "r");
    char line[1024];

    while (fgets(line, sizeof(line), fp)) {
		if (!isdigit(line[0]))
		{continue;}

        unsigned long address;
        char type;
        char name[512];
        if (sscanf(line, "%lx %c %511s", &address, &type, name) != 3) {
			continue;
		}
		
		void* sym_pointer = dlsym(RTLD_DEFAULT, name);
		if(sym_pointer == NULL){
			continue;
		}

		base_addr_absolute = (void*)sym_pointer;
		base_addr_relative = (void*)address;
		break;
    }
    pclose(fp);

	// Pass 2: Use this base address and the offsets in nm to compute the real address of every other symbol
	fp = popen(command, "r");
    while (fgets(line, sizeof(line), fp)) {
		if (!isdigit(line[0])){
            continue;
		}

        unsigned long address;
        char type;
        char name[512];
        if (sscanf(line, "%lx %c %511s", &address, &type, name) != 3) {
			continue;
		}

		void* real_address = (void*)((uintptr_t)base_addr_absolute + ((uintptr_t)address - (uintptr_t)base_addr_relative)); 	
		char* symbol_name = g_strdup_printf(name);
		g_hash_table_insert(identifiers_to_pointers_map, symbol_name, real_address); 
		g_hash_table_insert(pointers_to_identifiers_map, real_address, symbol_name);  
	}

	return identifiers_to_pointers_map;
    pclose(fp);
}


void sync_variables(char* shared_lib_path, void* shared_lib_dl_open_pointer, bool to_from){
    char command[512];
    snprintf(command, sizeof(command), "nm -C -S %s", shared_lib_path);

	// Pass 1: Get the base (real, not relative) address you will use to compute the other addresses
	FILE *fp = popen(command, "r");

    char line[1024];
	char type;
	char name[512];
	size_t address;
	size_t size;

	void* base_absolute = NULL;
	void* base_relative = NULL;
    while (fgets(line, sizeof(line), fp)) {

        // Format: address size type name
		if(!isdigit(line[0]) || !isdigit(line[18]))
		{continue;}
        if (!sscanf(line, "%lx %lx %c %511s", &address, &size, &type, name) == 3)
		{continue;}

		void* real_address = dlsym(shared_lib_dl_open_pointer, name);
		if(real_address == NULL)
		{continue;}

		base_absolute = (void*)real_address;
		base_relative = (void*)address;
		break;
    }
    pclose(fp);
	
	// Pass 2: Get real addresses 
	GHashTable* shared_lib_identifiers = g_hash_table_new(g_str_hash, g_str_equal);  
	fp = popen(command, "r");
    while (fgets(line, sizeof(line), fp)) {

        // Format: address size type name
		if(!isdigit(line[0]) || !isdigit(line[18])){
			continue;
		}
        if (!sscanf(line, "%lx %lx %c %511s", &address, &size, &type, name) == 3) {
			continue;
		}
		if(!is_variable(type)){
			continue;
		}

		symbol_info* value = malloc(sizeof(symbol_info));
		value->size = size;
		value->address = (void*)((uintptr_t)base_absolute + ((uintptr_t)address - (uintptr_t)base_relative)); 	
		g_hash_table_insert(shared_lib_identifiers, g_strdup(name), value);
    }

	// Pass 3: Actually sync!
	GHashTableIter iter;
	gpointer key, value;

	g_hash_table_iter_init(&iter, shared_lib_identifiers);
	while(g_hash_table_iter_next(&iter, &key, &value)){
		char* var_name = (char*)key;
		size_t var_size = ((symbol_info*)(value))->size;
		void* var_address = ((symbol_info*)(value))->address;
		if(var_address == NULL){
			printf("Could not find pointer to a variable in the shared lib with name %s\n", var_name);
			exit(1);
		}

		void* variable_pointer_main;
		if(g_hash_table_contains(identifiers_to_pointers_map, var_name)){
			variable_pointer_main = g_hash_table_lookup(identifiers_to_pointers_map, var_name);
		} else {
			variable_pointer_main = dlsym(RTLD_NEXT, var_name);
		}
		if(variable_pointer_main == NULL){
			printf("Could not find a pointer in main with name %s\n", var_name);
			exit(1);
		}

		g_print("syncing the variable %s\n", var_name);	
		if(to_from){
			memcpy(var_address, variable_pointer_main, var_size);
		} else {
			memcpy(variable_pointer_main, var_address, var_size);
		}
	}
	
    pclose(fp);
	
}


char* identifier_from_pointer(void* pointer){
	if(pointers_to_identifiers_map == NULL){
		get_main_symbols();	
	}
	return g_hash_table_lookup(pointers_to_identifiers_map, pointer);
}
char* pointer_from_identifier(char* identifier){
	if(identifiers_to_pointers_map == NULL){
		get_main_symbols();
	} 
	return g_hash_table_lookup(identifiers_to_pointers_map, identifier);
}


