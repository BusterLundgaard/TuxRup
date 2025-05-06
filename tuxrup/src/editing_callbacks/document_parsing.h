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

#include "../callback_map.h"

CXCursor get_root_cursor(char* document_path);
CXCursor get_function_cursor(CXCursor c, char* function_name);
CXCursor get_function_body_cursor(CXCursor c_func);   

int is_part_of_main_file(CXCursor cursor);
void write_cursor_element(CXCursor* c, GString* buffer, bool semicolons, bool newline);
enum CXChildVisitResult write_cursor_to_buffer(CXCursor c, CXCursor parent, CXClientData data);

char* get_function_arguments(CXCursor c_func);
char* get_function_return_type(CXCursor c_func);
char* get_function_code(CXCursor c);
char* get_variable_type(CXCursor c_var);
char* get_variable_type(CXCursor c_var);
#endif
