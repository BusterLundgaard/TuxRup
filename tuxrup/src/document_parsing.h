#ifndef DOCUMENT_PARSING_H
#define DOCUMENT_PARSING_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <clang-c/Index.h>

CXCursor get_root_cursor(char* document_path);
CXCursor get_function_cursor(CXCursor c, char* function_name);
CXCursor get_function_body_cursor(CXCursor c_func);   

void set_function_arguments(CXCursor c_func, GString* buffer);

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

void write_cursor_element(CXCursor* c, GString* buffer, bool semicolons, bool newline);

enum CXChildVisitResult write_cursor_to_buffer(CXCursor c, CXCursor parent, CXClientData data);


#endif