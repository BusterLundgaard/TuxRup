#include "modified_document_generation.h"

typedef struct {
    GHashTable** undefined;
    GHashTable** declared;
} set_undefined_needed_data;

bool is_part_of_file(CXCursor* cursor, char* filename){
    CXFile file;
    clang_getFileLocation(clang_getCursorLocation(*cursor), &file, NULL, NULL, NULL);
    if (!file)
    {return false;}

    CXString clang_filename = clang_getFileName(file);
    bool is_our_file = strcmp(clang_getCString(clang_filename), filename) == 0;
    clang_disposeString(clang_filename);
    if(!is_our_file)
    {return false;}

    return true;
}

void write_cursor_element(CXCursor* c, GString* buffer){
    CXSourceRange range = clang_getCursorExtent(*c);
    CXSourceLocation start = clang_getRangeStart(range);
    CXSourceLocation end = clang_getRangeEnd(range);
    CXFile file;
    unsigned start_offset, end_offset;
    clang_getSpellingLocation(start, &file, NULL, NULL, &start_offset);
    clang_getSpellingLocation(end, NULL, NULL, NULL, &end_offset);
    CXTranslationUnit tu = clang_Cursor_getTranslationUnit(*c);
    const char* fileContents = clang_getFileContents(tu, file, NULL);

    char* contents = g_strdup_printf("%.*s\n", (int)(end_offset - start_offset), fileContents + start_offset);
    g_string_append(buffer, contents); 
}

enum CXChildVisitResult add_cursor_to_buffer(CXCursor c, CXCursor parent, CXClientData data){
    GString* buffer = (GString*)data;
    write_cursor_element(&c, buffer);
}

enum CXChildVisitResult set_undefined_references(CXCursor c, CXCursor parent, CXClientData data){
    GHashTable** undefined = ((set_undefined_needed_data*)data)->undefined;
    GHashTable** declared = ((set_undefined_needed_data*)data)->declared;

    enum CXCursorKind kind = clang_getCursorKind(c);

    // Add declarations 
    if(kind == CXCursor_DeclStmt){
        g_hash_table_add(*declared, g_strdup(clang_getCString(clang_getCursorSpelling(c))));
        return CXChildVisit_Recurse;
    }

    // Check it is a reference to something undeclared we haven't seen before:
    if(kind != CXCursor_DeclRefExpr)
    {return CXChildVisit_Recurse;}

    CXCursor referenced_cursor = clang_getCursorReferenced(c);
    enum CXCursorKind referenced_kind = clang_getCursorKind(referenced_cursor);
    char* identifier = clang_getCString(clang_getCursorSpelling(referenced_cursor));
    if(g_hash_table_contains(*declared, identifier) || g_hash_table_contains(*undefined, identifier))
    {return CXChildVisit_Continue;}

    reference_type undefined_ref_type = {};
    undefined_ref_type.number = g_hash_table_size(*undefined);

    if(referenced_kind == CXCursor_VarDecl){
        undefined_ref_type.is_function=false;
        undefined_ref_type.args_types = "";
        undefined_ref_type.return_type = g_strdup(clang_getCString(clang_getTypeSpelling(clang_getCursorType(referenced_cursor))));
    } 
    else if(referenced_kind == CXCursor_FunctionDecl) {
        undefined_ref_type.is_function=true;
        
        CXType type = clang_getCursorType(referenced_cursor);
        undefined_ref_type.return_type = g_strdup(clang_getCString(clang_getTypeSpelling(clang_getResultType(type))));
       
        GString* argstr = g_string_new("");
        unsigned int num_args = clang_getNumArgTypes(type);
        for(int i = 0; i < num_args; i++){
            CXType arg_type = clang_getArgType(type, i);
            g_string_append(argstr, clang_getCString(clang_getTypeSpelling(arg_type)));
            if(i == num_args - 1){continue;}
            g_string_append(argstr, ",");
        }
        undefined_ref_type.args_types = g_strdup(argstr->str);
        g_string_free(argstr,TRUE);
    }
    g_hash_table_insert(*undefined, identifier, &undefined_ref_type);    

    return CXChildVisit_Continue;
}

void write_ref_typedef(char* identifier, reference_type* ref, GString* buffer){
    typedef int my_cooler_int;
    
    if(!ref->is_function){
        g_string_append(buffer, g_strdup_printf("typdef %s %s_t;\n", ref->return_type, identifier));        
    } else {
        g_string_append(buffer, g_strdup_printf("typedef %s(*%s_t)(%s);\n", ref->return_type, identifier, ref->args_types));
    }
}

void write_ref_declaration(char* identifier, reference_type* ref, GString* buffer){
    g_string_append(buffer, g_strdup_printf("%s_t %s = *((%s_t*)data[%d])", identifier, identifier, identifier, ref->number));
}

void write_var_overwrite(char* identifier, reference_type* ref, GString* buffer){
    g_string_append(buffer, g_strdup_printf("*((%s_t*)data[%d]) = %s", identifier, ref->number, identifier));
}

enum CXChildVisitResult clang_parse_build_buffer_modified_function(CXCursor c, CXCursor parent, CXClientData data){
    GString* document_buffer = ((create_modified_function_document_data*)data)->buffer;
    char* function_name = ((create_modified_function_document_data*)data)->function_name;
    GList** required_identifiers = ((create_modified_function_document_data*)data)->required_identifiers;

    enum CXCursorKind kind = clang_getCursorKind(c);
    if(kind == CXCursor_CompoundStmt){
        GHashTable* undefined = g_hash_table_new(g_str_hash, g_str_equal);
        GHashTable* declarations = g_hash_table_new(g_str_hash, g_str_equal);
        set_undefined_needed_data sets = {&undefined, &declarations};
        clang_visitChildren(c, set_undefined_references, &sets);
        
        // print your own function start here
        g_string_append(document_buffer, "{");

        GHashTableIter* iter;
        char* undefined_identifier;
        reference_type undefined_type;
        g_hash_table_iter_init(iter, undefined);

        for(; g_hash_table_iter_next(iter, (gpointer)undefined_identifier, (gpointer)(&undefined_type)); ){
            *required_identifiers = g_list_append(*required_identifiers, g_strdup(undefined_identifier));
            write_ref_typedef(undefined_identifier, &undefined_type, document_buffer);
            write_ref_declaration(undefined_identifier, &undefined_type, document_buffer);
        }

        // print the actual function body here
        clang_visitChildren(c, add_cursor_to_buffer, data);

        // print your function end here:
        g_hash_table_iter_init(iter, undefined);
        for(; g_hash_table_iter_next(iter, NULL, (gpointer)(&undefined_type)); ){
            if(undefined_type.is_function){continue;}
            write_var_overwrite(undefined_identifier, &undefined_type, document_buffer);
        }

        g_string_append(document_buffer, "}");

        g_hash_table_destroy(undefined);
        g_hash_table_destroy(declarations);
        return CXChildVisit_Continue;
    }

    write_cursor_element(&c, document_buffer);
    return CXChildVisit_Continue;
}

enum CXChildVisitResult clang_parse_build_buffer_ignore_other_identifiers(CXCursor c, CXCursor parent, CXClientData data){
    GString* document_buffer = ((create_modified_function_document_data*)data)->buffer;
    char* function_name = ((create_modified_function_document_data*)data)->function_name;

    enum CXCursorKind kind = clang_getCursorKind(c);
    if(kind == CXCursor_VarDecl)
    {return CXChildVisit_Continue;}

    if(kind == CXCursor_FunctionDecl){
        char* identifier = clang_getCString(clang_getCursorSpelling(c));
        if(strcmp(identifier, function_name) != 0)
        {return CXChildVisit_Continue;}

        clang_visitChildren(c, clang_parse_build_buffer_modified_function, data);
        return CXChildVisit_Continue;
    }

    write_cursor_element(&c, document_buffer);
    return CXChildVisit_Continue;
}

create_modified_function_document_data 
create_modified_document_and_find_identifiers(const char* filepath, const char* modified_function_name){
    GString* buffer = g_string_new("");
    GList* required_identifiers = NULL;
    create_modified_function_document_data data = {buffer, modified_function_name, &required_identifiers};
    
    CXIndex index = clang_createIndex(0,0);
    CXTranslationUnit unit = clang_parseTranslationUnit(index, filepath, NULL, 0, NULL, 0, CXTranslationUnit_None);
    if (!unit) {
        printf("Failed to parse translation unit\n"); 
        create_modified_function_document_data empty = {};
        return empty;
        }

    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursor, clang_parse_build_buffer_ignore_other_identifiers, &data);

    return data;
}