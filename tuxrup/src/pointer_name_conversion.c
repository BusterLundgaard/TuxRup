#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <libelf.h>
#include <fcntl.h>
#include <gelf.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <clang-c/Index.h>
#include <limits.h>

#include "pointer_name_conversion.h"

enum elf_scan_mode {
    ELF_FIND_POINTER,
    ELF_FIND_IDENTIFIER
};

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

bool is_pie_executable()
{
    int fd = open("/proc/self/exe", O_RDONLY);
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
    int fd = open("/proc/self/exe", O_RDONLY);
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
void* get_pointer_from_identifier(const char *name){
    return (void*)(scan_elf(ELF_FIND_IDENTIFIER, (void*)name));
}