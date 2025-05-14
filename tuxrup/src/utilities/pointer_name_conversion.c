#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <libelf.h>
#include <fcntl.h>
#include <gelf.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <glib.h>
#include <ctype.h>
#include <link.h>

#include "pointer_name_conversion.h"

// Executables or debug symbols that we read
static char* executable_path = NULL;
static char* debug_symbols_path = NULL;
static char* shared_lib_path = NULL;

// Pointer to shared library (overwritten) function about to be called
static void* shared_lib_dl_open_pointer = NULL;
// Cached results 
static bool has_debugging_symbols_embedded; 
static void* base_address;

GHashTable* main_identifiers_to_pointers;
GHashTable* main_pointers_to_identifiers;
GHashTable* shared_lib_identifiers; //from variable names to variable sizes 

char* program_name;
typedef struct {
	size_t size;
	void* address;
} symbol_info;

// ========================================================
// Extracting required supplementary information
// ========================================================
bool set_has_debugging_symbols_embedded() {
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed!\n");
        return false;
    }

    int fd = open(executable_path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return false;
    }

    Elf *e = elf_begin(fd, ELF_C_READ, NULL);
    if (!e) {
        fprintf(stderr, "elf_begin failed: %s\n", elf_errmsg(-1));
        close(fd);
        return false;
    }

    size_t shstrndx;
    if (elf_getshdrstrndx(e, &shstrndx) != 0) {
        fprintf(stderr, "elf_getshdrstrndx failed: %s\n", elf_errmsg(-1));
        elf_end(e);
        close(fd);
        return false;
    }

    Elf_Scn *scn = NULL;
    while ((scn = elf_nextscn(e, scn)) != NULL) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) != &shdr) continue;

        const char *name = elf_strptr(e, shstrndx, shdr.sh_name);
        if (!name) continue;

        if (strcmp(name, ".debug_info") == 0 || strcmp(name, ".symtab") == 0) {
            elf_end(e);
            close(fd);
            return true;  // Debug symbols or symbol table found
        }
    }

    elf_end(e);
    close(fd);
    return false;  // No debug symbols found
}

void download_debug_symbols(){
	char* executable_path_copy = g_strdup(executable_path);
	char* dir = dirname(executable_path_copy);

    const char* command = g_strdup_printf("source ./get_debug_symbols.sh %s %s", dir, program_name);
    FILE* pipe = popen(command, "r");

    char buffer[1024];
    if(fgets(buffer, sizeof(buffer), pipe) == NULL){}

	char* symbols_path = g_strdup_printf("./symbols/%s_symbols", program_name);
	if(access(symbols_path, F_OK) == 0) {
		debug_symbols_path = symbols_path;
		g_print("Succesfully downloaded debug symbols!\n");
	} else{
		g_print("Could not succesfully download debug symbols\n");
	}
}

void *get_base_address()
{
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp){return NULL;}

    void *base_addr = NULL;
    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "r--p") && strstr(line, program_name))
        {
            base_addr = (void *)strtoull(line, NULL, 16);
            break;
        }
    }

    fclose(fp);
    return base_addr;
}

bool is_pie_executable()
{
	int fd;
	if(has_debugging_symbols_embedded){
        fd = open("/proc/self/exe", O_RDONLY);
    } else {
        fd = open(debug_symbols_path, O_RDONLY);
	}
    if (fd < 0) {return NULL;}

    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
    {
        close(fd);
        return false;
    }
    close(fd);

    return (ehdr.e_type == ET_DYN); // PIE executables are of type ET_DYN
}


// =====================================================================
// Reading ELF files
// =====================================================================
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
}
/* T: global function in .text */
/* t: local function in .text */
/* D: initialized data */
/* B: uninitialized data */
/* R: read-only data */
/* U: undefined (external) symbol */
/* C: common symbol (old-style uninitialized global) */
/* so you should check after D, B, R, U, C */

void read_main_symbols(){
    char command[512];
    snprintf(command, sizeof(command), "nm %s", debug_symbols_path);

	FILE *fp = popen(command, "r");
    char line[1024];
    void* base_addr = is_pie_executable() ? get_base_address() : 0;

    while (fgets(line, sizeof(line), fp)) {
		if (!isdigit(line[0])){
            continue;
		}

        unsigned long address;
        char type;
        char name[512];

        if (sscanf(line, "%lx %c %511s", &address, &type, name) == 3) {
            char *sym_name = g_strdup(name);
			g_print("reading symbol with name %s\n", sym_name);

			void* sym_pointer = (void*)((uintptr_t)address + base_addr);
            g_hash_table_insert(main_pointers_to_identifiers, sym_pointer, sym_name);
            g_hash_table_insert(main_identifiers_to_pointers, sym_name, sym_pointer);
        }
    }

    pclose(fp);
}


void read_shared_lib_symbols() {
    char command[512];
    snprintf(command, sizeof(command), "nm -C -S %s", shared_lib_path);

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
		if(!isdigit(line[0]) || !isdigit(line[18])){
			continue;
		}
        if (!sscanf(line, "%lx %lx %c %511s", &address, &size, &type, name) == 3) {
			continue;
		}

		void* real_address = dlsym(shared_lib_dl_open_pointer, name);
		if(real_address != NULL){
			base_absolute = (void*)real_address;
			base_relative = (void*)address;
			break;
		}
    }
    pclose(fp);
	
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
}

// ===========================================================
// Initialization
// ===========================================================
void initialize_debugging_symbols(char* _executable_path){
	executable_path = _executable_path;
	debug_symbols_path = executable_path;

	char* executable_path_copy = g_strdup(executable_path);
    program_name = basename(executable_path_copy);

	has_debugging_symbols_embedded = set_has_debugging_symbols_embedded();
	if(!has_debugging_symbols_embedded){
		download_debug_symbols();	
	}
	main_identifiers_to_pointers = g_hash_table_new(g_str_hash, g_str_equal);
	main_pointers_to_identifiers = g_hash_table_new(g_direct_hash, g_direct_equal);
	read_main_symbols();
}

void initialize_shared_lib(char* _shared_lib_path, void* _shared_lib_dl_open_pointer){
	shared_lib_path = _shared_lib_path;	
	shared_lib_dl_open_pointer = _shared_lib_dl_open_pointer;
	if(shared_lib_identifiers != NULL){
		g_hash_table_remove_all(shared_lib_identifiers);
	}
	shared_lib_identifiers = g_hash_table_new(g_str_hash, g_str_equal);
	read_shared_lib_symbols();
}


// ==========================================================
// Public methods
// =========================================================
bool has_debugging_symbols(){
	return has_debugging_symbols_embedded || (debug_symbols_path != NULL);
}
bool main_contains_symbol(char* symbol){
	return g_hash_table_contains(main_identifiers_to_pointers, symbol);
}

char* get_identifier_from_pointer(void *pointer){
	if(g_hash_table_contains(main_pointers_to_identifiers, pointer)){
		return (char*)(g_hash_table_lookup(main_pointers_to_identifiers, pointer));		
	} else {
		printf("Tried to get identifier from pointer %p, but failed!\n", pointer);
		exit(1);
	}
}

void *get_pointer_from_identifier(const char *name){
	GHashTableIter iter;
	gpointer key, value;
	g_hash_table_iter_init(&iter, main_identifiers_to_pointers);
	while(g_hash_table_iter_next(&iter, &key, &value)){
		char* name = (char*)key;
	}

	if(g_hash_table_contains(main_identifiers_to_pointers, name)){
		return g_hash_table_lookup(main_identifiers_to_pointers, name);
	} else{
		void* library_pointer = dlsym(RTLD_NEXT, name);
		if(library_pointer != NULL){
			return library_pointer;
		} else {
			printf("Tried to get pointer from identifier %s, but failed!\n", name);
			exit(1);			
		}
	}
}

void sync_shared_variables_to_main(bool direction){
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
		if(g_hash_table_contains(main_identifiers_to_pointers, var_name)){
			variable_pointer_main = g_hash_table_lookup(main_identifiers_to_pointers, var_name);
		} else {
			variable_pointer_main = dlsym(RTLD_NEXT, var_name);
		}
		if(variable_pointer_main == NULL){
			printf("Could not find a pointer in main with name %s\n", var_name);
			exit(1);
		}

		g_print("syncing the variable %s\n", var_name);	
		if(direction){
			memcpy(var_address, variable_pointer_main, var_size);
		} else {
			memcpy(variable_pointer_main, var_address, var_size);
		}
	}
}	

GHashTable* get_identifiers(char* symbols_path){
	initialize_shared_lib(symbols_path, NULL);
	return shared_lib_identifiers;	
}
