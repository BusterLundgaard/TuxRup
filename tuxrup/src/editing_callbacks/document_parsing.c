#include "../callback_map.h"
#include "../utilities/util.h"

#include "document_parsing.h"
#include "reference_type.h"

#include <stdio.h>


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
    const char *args[] = {
        "-I/usr/include/gtk-4",
        "-D_GNU_SOURCE"
    };
    CXIndex index = clang_createIndex(0,0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index, filepath, args, 0, NULL, 0, 
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


char* get_function_arguments(CXCursor c_func){
	CXType funcType = clang_getCursorType(c_func);
	int n = clang_getNumArgTypes(funcType);
	GString* buffer = g_string_new("");
    for (unsigned int i = 0; i < n; i++) {
        CXCursor arg_cursor = clang_Cursor_getArgument(c_func, i);
        write_cursor_element(&arg_cursor, buffer, false, false);
		if(i != n - 1){
			g_string_append(buffer, ", ");
		}
    }
	return buffer->str;
}

char* get_function_return_type(CXCursor c_func){
	CXType funcType = clang_getCursorType(c_func);
	return clang_getCString(clang_getTypeSpelling(clang_getResultType(clang_getCursorType(c_func))));
}


char* get_function_code(CXCursor c){
	CXCursor c_body = get_function_body_cursor(c);
    GString* code_buffer = g_string_new("");
    clang_visitChildren(  
		c_body,	
        write_cursor_to_buffer,
        code_buffer
    );
    return code_buffer->str;
}
char* get_variable_type(CXCursor c_var){
	CXType varType = clang_getCursorType(c_var);
    CXString typeSpelling = clang_getTypeSpelling(varType);
	return clang_getCString(typeSpelling);
}
