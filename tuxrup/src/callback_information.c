
#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include "callback_information.h"
#include "document_parsing.h"
#include "pointer_name_conversion.h"
#include "util.h"

callback_code_information* get_callback_code_information(void* callback, char* callback_name){
    // Parse document and retrieve all information
    char* function_name = get_identifier_from_pointer(callback);
    char* document_path = get_document_path(function_name);

    CXCursor c = get_root_cursor(document_path);
    CXCursor c_func = get_function_cursor(c, function_name);
    CXCursor c_func_body = get_function_body_cursor(c_func);

    GString *before_code = g_string_new("");
    GString *after_code = g_string_new("");
    CXSourceLocation func_loc = clang_getCursorLocation(c_func);

    set_before_after_code_args args = {before_code, after_code, func_loc};
    clang_visitChildren(c, set_before_after_code, &args);

    GString *definitions_code = g_string_new("");
    clang_visitChildren(c, set_definitions_code, definitions_code);

    GString *function_code = g_string_new("");
    clang_visitChildren(c_func_body, write_cursor_to_buffer, function_code);

    GString *args_code = g_string_new("");
    set_function_arguments(c_func, args_code);   

    // Allocate all this information on the heap:
    callback_code_information* info = malloc(sizeof(callback_code_information));
    info->function_name = function_name;
    info->document_path = document_path;
    info->before_code = g_strdup(before_code->str);
    info->after_code = g_strdup(after_code->str);
    info->definitions_code = g_strdup(definitions_code->str);
    info->function_code = g_strdup(function_code->str);
    info->args_code = g_strdup(args_code->str);
    
    //Free the GStrings:
    g_string_free(before_code, TRUE);
    g_string_free(after_code, TRUE);
    g_string_free(definitions_code, TRUE);
    g_string_free(function_code, TRUE);
    g_string_free(args_code, TRUE);

    // Return the info
    return info;
}

void free_callback_code_information(callback_code_information* info){
    g_free(info->function_name);
    g_free(info->document_path);
    g_free(info->before_code);
    g_free(info->args_code);
    g_free(info->function_code);
    g_free(info->after_code);
    g_free(info->definitions_code);
}