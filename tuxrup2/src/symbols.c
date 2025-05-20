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

__attribute__((used)) static void* force_link = (void*)g_direct_hash;
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


bool is_pie_executable()
{
    int fd = open(get_executable_symbols_path(), O_RDONLY);

    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
    {
        close(fd);
        return false;
    }
    close(fd);

    return (ehdr.e_type == ET_DYN); // PIE executables are of type ET_DYN
}

void *get_base_address()
{
	if(!is_pie_executable()){
		return 0;
	}

    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp){return NULL;}

    void *base_addr = NULL;
    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "r--p") && strstr(line, get_executable_name()))
        {
            base_addr = (void *)strtoull(line, NULL, 16);
            break;
        }
    }

    fclose(fp);
    return base_addr;
}

GHashTable* get_main_symbols(){
	if(identifiers_to_pointers_map != NULL){
		return identifiers_to_pointers_map;
	}

	identifiers_to_pointers_map = g_hash_table_new(g_str_hash, g_str_equal);
	pointers_to_identifiers_map = g_hash_table_new(g_direct_hash, g_direct_equal);

	void* base_addr = get_base_address();

    char command[512];
    snprintf(command, sizeof(command), "nm %s", get_executable_symbols_path());
	FILE* fp = popen(command, "r");

    char line[256];
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

		void* sym_pointer = (void*)((uintptr_t)address + base_addr);
		char* sym_name = g_strdup(name);
		g_hash_table_insert(identifiers_to_pointers_map, sym_name, sym_pointer); 
		g_hash_table_insert(pointers_to_identifiers_map, sym_pointer, sym_name);  
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
		if(size == 0){
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
		if(var_name == NULL){
			g_print("Syncing variables, but some var_name is NULL. Continuing on with the next symbol\n");
			continue;
		}
		if(value == NULL){
			g_print("Pointer to size and address info about symbol %s is NULL. Continuing on with the next symbol\n", var_name);
			continue;
		}

		size_t var_size = ((symbol_info*)(value))->size;
		if(var_size == 0){
			g_print("Variable %s has size 0, so we cant sync it. Continuing on with the next symbol\n", var_name);
			continue;
		}

		void* var_address_shared = ((symbol_info*)(value))->address;
		if(var_address_shared == NULL){
			printf("Could not find pointer to a variable in the shared lib with name %s. Continuing on with the next symbol\n", var_name);
			continue;
		}

		void* var_address_main;
		if(g_hash_table_contains(identifiers_to_pointers_map, var_name)){
			var_address_main = g_hash_table_lookup(identifiers_to_pointers_map, var_name);
		} else {
			var_address_main = dlsym(RTLD_NEXT, var_name);
		}
		if(var_address_main == NULL){
			printf("Could not find a pointer in main with name %s. Continuing on with the next symbol.\n", var_name);
			continue;
		}
		
		if(((uintptr_t)var_address_shared > (uintptr_t)var_address_main && (uintptr_t)var_address_main + var_size > (uintptr_t)var_address_shared) ||
		   ((uintptr_t)var_address_main > (uintptr_t)var_address_shared && (uintptr_t)var_address_shared + var_size > (uintptr_t)var_address_main) ||
		   ((uintptr_t)var_address_main < var_size) ||
		   ((uintptr_t)var_address_shared < var_size)){
			printf("Detected a memcpy that seemed sus (overlapping regions or the size of the allocation being greater than the actual addresses. Stopping and continuing to next variable\n");
			continue;
		}

		if(to_from){
			memcpy(var_address_shared, var_address_main, var_size);
		} else {
			memcpy(var_address_main, var_address_shared, var_size);
		}
	}
	
    pclose(fp);
	
}


char* identifier_from_pointer(void* pointer){
	if(pointers_to_identifiers_map == NULL){
		get_main_symbols();	
	}
	if(!g_hash_table_contains(pointers_to_identifiers_map, pointer)){
		return "N/A";
	}
	return g_hash_table_lookup(pointers_to_identifiers_map, pointer);
}
char* pointer_from_identifier(char* identifier){
	if(identifiers_to_pointers_map == NULL){
		get_main_symbols();
	} 
	if(!g_hash_table_contains(identifiers_to_pointers_map, identifier)){
		g_print("could not find pointer for the identifier %s\n", identifier);
		return NULL;
	}
	return g_hash_table_lookup(identifiers_to_pointers_map, identifier);
}


