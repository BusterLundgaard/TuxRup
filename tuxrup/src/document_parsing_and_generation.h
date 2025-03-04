#ifndef MODIFIED_DOCUMENT_GENERATION_H
#define MODIFIED_DOCUMENT_GENERATION_H

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

typedef struct {
    int number;
    bool is_function;
    char* return_type;
    char* args_types;
} reference_type;

typedef struct {
    callback_code_information* callback_info;
    GString* buffer; 
    GHashTable* undefined_identifiers; 
    GHashTable* declared_identifiers} 
parsing_data;

parsing_data create_modified_document_and_find_identifiers(const char* filepath, const char* modified_function_name);

void document_generation_test1(char* document_path, char* function_name);

#endif