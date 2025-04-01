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
    int* line;
} set_before_after_code_args;
enum CXChildVisitResult set_before_after_code(CXCursor c, CXCursor parent, CXClientData data);

enum CXChildVisitResult set_definitions_code(CXCursor c, CXCursor parent, CXClientData data);

void write_cursor_element(CXCursor* c, GString* buffer, bool semicolons, bool newline);

enum CXChildVisitResult write_cursor_to_buffer(CXCursor c, CXCursor parent, CXClientData data);

// The purpose of this function is to take the document containing the function the user wants to edit, then make a copy of that document the user will see and edit it 
// This copy will include some additional comments for pedagogic reasons
// If the user has previously edited the function in this document (path_to_document_with_modified_function != NULL), it will also be replaced with their modified function
// For convenience, this function also returns the line in the document where the function to be modified is
// This makes it possible to open their text editor exactly on the function they want to edit 
int create_version_of_document_for_code_editing_and_get_location(char* path_to_document_with_original_function, char* original_function_name, char* path_to_document_with_modified_function, char* output_path);

char* set_callback_code_information(callback_info info);

char* get_function_code(char* document_path, char* function_name);

#endif