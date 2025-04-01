#include "document_parsing.h"
#include "reference_type.h"
#include "util.h"
#include <stdio.h>
#include "callback_map.h"

// ==================================================
// UTILITY
// ==================================================
int get_line(CXCursor c){
    CXSourceRange range = clang_getCursorExtent(c);
    CXSourceLocation start = clang_getRangeStart(range);
    unsigned line;
    clang_getSpellingLocation(start, NULL, &line, NULL, NULL);
    return line;
}


// ==================================================
// FUNCTIONS TO GET CURSORS
// ==================================================
CXCursor get_root_cursor(char* filepath){
    CXIndex index = clang_createIndex(0,0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index, filepath, NULL, 0, NULL, 0, 
        CXTranslationUnit_DetailedPreprocessingRecord);
    if (!unit) {
        CXCursor empty = {};
        return empty;
        }

    return clang_getTranslationUnitCursor(unit);
}

typedef struct {
    char* function_name;
    CXCursor* function_cursor;
} visit_function_with_name_args;

enum CXChildVisitResult visit_function_with_name(CXCursor c, CXCursor parent, CXClientData data){
    visit_function_with_name_args* args = (visit_function_with_name_args*)data;

    char* identifier = clang_getCString(clang_getCursorSpelling(c));
    enum CXCursorKind kind = clang_getCursorKind(c);
    if(kind == CXCursor_FunctionDecl && (strcmp(identifier, args->function_name) == 0)){
        *(args->function_cursor) = c;
        return CXChildVisit_Break;
    }

    return CXChildVisit_Continue;
}

CXCursor get_function_cursor(CXCursor c, char* function_name){
    CXCursor function_cursor;
    visit_function_with_name_args args = {function_name, &function_cursor};
    clang_visitChildren(c, visit_function_with_name, &args);
    return function_cursor;
}

enum CXChildVisitResult visit_next_compound_stmt(CXCursor c, CXCursor parent, CXClientData data){
    CXCursor* next_compound = (CXCursor*)data;
    enum CXCursorKind kind = clang_getCursorKind(c);
    if(kind == CXCursor_CompoundStmt){
        *next_compound = c;
        return CXChildVisit_Break;
    }
    return CXChildVisit_Recurse;
}

CXCursor get_function_body_cursor(CXCursor c){
    CXCursor next_compound = {};
    clang_visitChildren(c, visit_next_compound_stmt, &next_compound);
    return next_compound;
}


// ======================================================
// CONDITIONAL FUNCTIONS:
// ======================================================

int is_part_of_main_file(CXCursor cursor) {
    CXTranslationUnit tu = clang_Cursor_getTranslationUnit(cursor);
    CXFile mainFile = clang_getFile(tu, clang_getCString(clang_getTranslationUnitSpelling(tu)));

    CXSourceLocation loc = clang_getCursorLocation(cursor);
    CXFile cursorFile;
    clang_getSpellingLocation(loc, &cursorFile, NULL, NULL, NULL);

    return cursorFile == mainFile; 
}

int cursor_compare(CXSourceLocation c1, CXSourceLocation c2){
    CXFile file1, file2;
    unsigned line1, column1, offset1;
    unsigned line2, column2, offset2;
    clang_getSpellingLocation(c1, &file1, &line1, &column1, &offset1);
    clang_getSpellingLocation(c2, &file2, &line2, &column2, &offset2);
    if(line1 == line2 && column1 == column2){return 0;}
    if(line1 == line2)                      {return column1 < column2 ? -1 : 1;}
    else                                    {return line1 < line2 ? -1 : 1;}
}


// ===============================================================
// PARSING FUNCTIONS
// ===============================================================

void write_cursor_element(CXCursor* c, GString* buffer, bool semicolons, bool newline){
    // Get string contents:
    CXSourceRange range = clang_getCursorExtent(*c);
    CXSourceLocation start = clang_getRangeStart(range);
    CXSourceLocation end = clang_getRangeEnd(range);
    CXFile file;
    unsigned start_offset, end_offset;
    clang_getSpellingLocation(start, &file, NULL, NULL, &start_offset);
    clang_getSpellingLocation(end, NULL, NULL, NULL, &end_offset);
    CXTranslationUnit tu = clang_Cursor_getTranslationUnit(*c);
    const char* fileContents = clang_getFileContents(tu, file, NULL);
    char* contents = g_strdup_printf("%.*s", (int)(end_offset - start_offset), fileContents + start_offset);

    // Add them to buffer with certain edge cases for inclusion directives and macros:
    enum CXCursorKind kind = clang_getCursorKind(*c);
    g_string_append(
        buffer,
        (kind == CXCursor_MacroDefinition) ? "#define " : ""
    );
    g_string_append(buffer, contents); 
    if(semicolons && !((kind == CXCursor_InclusionDirective || kind == CXCursor_MacroDefinition))){
        g_string_append(buffer, ";");
    }
    if(newline){
        g_string_append(buffer, "\n");
    }
}

enum CXChildVisitResult write_cursor_to_buffer(CXCursor c, CXCursor parent, CXClientData data){
    GString* buffer = (GString*)data;
    g_string_append(buffer, "    ");
    write_cursor_element(&c, buffer, true, true);
    return CXChildVisit_Continue;
}


char* extract_type(const char* argument) {
    if (!argument) return NULL;

    size_t len = strlen(argument);
    if (len == 0) return NULL;

    // Find the last word (parameter name)
    int last_space_index = -1;
    for (int i = len - 1; i >= 0; i--) {
        if (argument[i] == ' ') {
            last_space_index = i;
            break;
        }
    }

    // If no spaces were found, the input is invalid
    if (last_space_index == -1) return NULL;

    // Allocate memory for the extracted type
    char* type = (char*)malloc(last_space_index + 1);
    if (!type) return NULL;

    strncpy(type, argument, last_space_index);
    type[last_space_index] = '\0';  // Null-terminate the string

    return type;
}

void set_function_argument_types(CXCursor c_func, GString* buffer){
    CXType type = clang_getCursorType(c_func);
    unsigned int num_args = clang_getNumArgTypes(type);
    for(int i = 0; i < num_args; i++){
        CXCursor arg_cursor = clang_Cursor_getArgument(c_func, i);
        GString* full_arg = g_string_new("");
        write_cursor_element(&arg_cursor, full_arg, false, false);
        char* arg_type = extract_type(full_arg->str);
        g_string_append(buffer, arg_type);
        
        if(i == num_args - 1){continue;}
        g_string_append(buffer, ",");
    }
}

void set_function_arguments(CXCursor c_func, GString* buffer) {
    CXType type = clang_getCursorType(c_func);
    unsigned int num_args = clang_getNumArgTypes(type);

    for (unsigned int i = 0; i < num_args; i++) {
        CXCursor arg_cursor = clang_Cursor_getArgument(c_func, i);
        write_cursor_element(&arg_cursor, buffer, false, false);
        if (i < num_args - 1) {
            g_string_append(buffer, ", ");
        }
    }
}


enum CXChildVisitResult set_before_after_code(CXCursor c, CXCursor parent, CXClientData data){
    set_before_after_code_args* args = (set_before_after_code_args*)data;

    if(!is_part_of_main_file(c))
    {return CXChildVisit_Continue;}

    CXSourceLocation loc = clang_getCursorLocation(c);
    int cursor_comparison = cursor_compare(loc, args->modified_function_location);
    if(cursor_comparison == 0){
        *(args->line) = get_line(c); 
        return CXChildVisit_Continue;
    }
    else if(cursor_comparison == -1){
        write_cursor_element(&c, args->before_code, true, true);
    } 
    else {
        write_cursor_element(&c, args->after_code, true, true);
    }

    return CXChildVisit_Continue;
} 


enum CXChildVisitResult set_definitions_code(CXCursor c, CXCursor parent, CXClientData data){
    GString* buffer = (GString*)data;
    
    enum CXCursorKind kind = clang_getCursorKind(c);
    if(kind == CXCursor_VarDecl || kind == CXCursor_MacroExpansion || kind == CXCursor_FunctionDecl || !is_part_of_main_file(c)){
        return CXChildVisit_Continue;
    }

    write_cursor_element(&c, buffer, true, true);

    return CXChildVisit_Continue;
}


reference_type* create_identifier_type_info(CXCursor identifier_cursor){
    reference_type* identifier_info = malloc(sizeof(reference_type));

    enum CXCursorKind identifier_kind = clang_getCursorKind(identifier_cursor);
    if(identifier_kind == CXCursor_VarDecl){
        identifier_info->is_function = false;
        identifier_info->args_types = "";
        identifier_info->return_type = g_strdup(clang_getCString(clang_getTypeSpelling(clang_getCursorType(identifier_cursor))));
    } 
    else {
        identifier_info->is_function=true;
        
        CXType type = clang_getCursorType(identifier_cursor);
        identifier_info->return_type = g_strdup(clang_getCString(clang_getTypeSpelling(clang_getResultType(type))));
       
        GString* argstr = g_string_new("");
        set_function_argument_types(identifier_cursor, argstr);
        identifier_info->args_types = g_strdup(argstr->str);
        g_string_free(argstr, TRUE);
    }

    return identifier_info;
}


enum CXChildVisitResult set_undefined_references(CXCursor c, CXCursor parent, CXClientData data){
    find_undefined_references_args* args = (find_undefined_references_args*)data;
    GHashTable* undefined = args->undefined_identifiers;
    GHashTable* declared = args->declared_identifiers;

    enum CXCursorKind kind = clang_getCursorKind(c);

    // Add declarations 
    if(kind == CXCursor_DeclStmt){
        g_hash_table_add(declared, g_strdup(clang_getCString(clang_getCursorSpelling(c))));
        return CXChildVisit_Recurse;
    }

    // Check it is a reference to something undeclared we haven't seen before:
    if(kind != CXCursor_DeclRefExpr)
    {return CXChildVisit_Recurse;}

    CXCursor referenced_cursor = clang_getCursorReferenced(c);

    enum CXCursorKind referenced_kind = clang_getCursorKind(referenced_cursor);
    char* identifier = clang_getCString(clang_getCursorSpelling(referenced_cursor));
    if(g_hash_table_contains(declared, identifier) || g_hash_table_contains(undefined, identifier))
    {return CXChildVisit_Continue;}

    if(clang_equalCursors(referenced_cursor, clang_getNullCursor())){
    return CXChildVisit_Continue;}

    // Build the type information
    g_print("in parsing, adding the undeclared identifier: %s\n", identifier);
    reference_type* undefined_ref_type = create_identifier_type_info(referenced_cursor);
    undefined_ref_type->number = g_hash_table_size(undefined);
    g_hash_table_insert(undefined, identifier, undefined_ref_type);    

    return CXChildVisit_Continue;
}



