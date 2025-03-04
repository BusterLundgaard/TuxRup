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

CXCursor get_root_cursor(char* document_path);
CXCursor get_function_cursor(CXCursor c, char* function_name);
CXCursor get_function_body_cursor(CXCursor c_func);

void set_function_argument(CXCursor c_func, GString* buffer);   

typedef struct {
    GHashTable* undefined_identifiers; 
    GHashTable* declared_identifiers;
} find_undefined_references_args;
enum CXChildVisitResult set_undefined_references(CXCursor c, CXCursor parent, CXClientData data);

typedef struct {
    GString* before_code;
    GString* after_code;
    CXSourceLocation modified_function_location;
} set_before_after_code_args;
enum CXChildVisitResult set_before_after_code(CXCursor c, CXCursor parent, CXClientData data);

enum CXChildVisitResult set_definitions_code(CXCursor c, CXCursor parent, CXClientData data);

enum CXChildVisitResult write_cursor_to_buffer(CXCursor c, CXCursor parent, CXClientData data);


#endif