#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
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

#include "../globals.h"
#include "pointer_name_conversion.h"

enum elf_scan_mode {
    ELF_FIND_POINTER,
    ELF_FIND_IDENTIFIER
};

bool has_debug_symbols() {
    char* path = executable_path;
    
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed!\n");
        return false;
    }

    int fd = open(path, O_RDONLY);
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

void *get_base_address()
{
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp){return NULL;}

    void *base_addr = NULL;
    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "r--p") && strstr(line, "hw"))
        {
            base_addr = (void *)strtoull(line, NULL, 16);
            break;
        }
    }

    fclose(fp);
    return base_addr;
}

int open_debug_symbols(){
    int fd;
    if(debug_symbols_path != NULL){
        fd = open(debug_symbols_path, O_RDONLY);
    } else {
        fd = open("/proc/self/exe", O_RDONLY);
    }
    return fd;
}

bool is_pie_executable()
{
    int fd = open_debug_symbols();
    if (fd < 0){return false;}

    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
    {
        close(fd);
        return false;
    }
    close(fd);

    return (ehdr.e_type == ET_DYN); // PIE executables are of type ET_DYN
}

void* scan_elf(enum elf_scan_mode mode, void* comparison_value){
    int fd = open_debug_symbols();
    if (fd < 0) {return NULL;}

    void* base_addr = is_pie_executable() ? get_base_address() : 0;
    
    elf_version(EV_CURRENT);
    Elf *elf = elf_begin(fd, ELF_C_READ, NULL);
    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;

    // Look through every section of elf
    while ((scn = elf_nextscn(elf, scn)) != NULL)
    {   
        // Only look at symbol table sections
        gelf_getshdr(scn, &shdr);
        if(shdr.sh_type != SHT_SYMTAB){continue;}

        Elf_Data *data = elf_getdata(scn, NULL);
        int count = shdr.sh_size / shdr.sh_entsize;

        // Look through every symbol in symbol table section
        for(int i = 0; i < count; i++){
            GElf_Sym sym;
            gelf_getsym(data, i, &sym);
            
            void *sym_pointer = (void *)((uintptr_t)base_addr + sym.st_value);
            const char *sym_name = elf_strptr(elf, shdr.sh_link, sym.st_name);

            void* ret;
            if(mode == ELF_FIND_POINTER && sym_pointer == comparison_value){
                ret = (void*)(g_strdup(sym_name));
            }
            else if(mode==ELF_FIND_IDENTIFIER && strcmp(sym_name, (char*)comparison_value) == 0){
                ret = sym_pointer;
            } else {
                continue;
            }

            elf_end(elf);
            close(fd);
            return ret;
        }
    }

    elf_end(elf);
    close(fd);
    return NULL;
}

char* get_identifier_from_pointer(void *pointer){
    return (char*)(scan_elf(ELF_FIND_POINTER, pointer));
}

void *get_pointer_from_identifier(const char *name){
    return scan_elf(ELF_FIND_IDENTIFIER, name);
}

void get_debug_symbols() {
    char *dir = dirname(executable_path);              
    char *program_name = basename(executable_path);  
    
    char* symbols_path = g_strdup_printf("./%s_symbols", program_name);
    debug_symbols_path = symbols_path;
    
    struct stat st;
    if (stat(program_name, &st) == 0 && S_ISREG(st.st_mode)) {
        return;
    }

    const char* command = g_strdup_printf("source get_debug_symbols %s %s", dir, program_name);
    FILE* pipe = popen(command, "r");

    char buffer[1024];
    if(fgets(buffer, sizeof(buffer), pipe) == NULL){}
}