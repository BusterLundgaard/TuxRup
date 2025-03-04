#include "document_parsing_and_generation.h"
#include "modify_callback.h"

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

    char* contents = g_strdup_printf("%.*s", (int)(end_offset - start_offset), fileContents + start_offset);
    g_string_append(buffer, contents); 
}

enum CXChildVisitResult add_cursor_to_buffer(CXCursor c, CXCursor parent, CXClientData data){
    GString* buffer = (GString*)data;
    g_string_append(buffer, "    ");
    write_cursor_element(&c, buffer);
    g_string_append(buffer, ";\n");
    return CXChildVisit_Continue;
}

// Takes a cursor c it assumes points to a FunctionDecl, returns a buffer with the arguments definition code
GString* get_function_arguments(CXCursor c){
    CXType type = clang_getCursorType(c);

    GString* argstr = g_string_new("");
    unsigned int num_args = clang_getNumArgTypes(type);
    for(int i = 0; i < num_args; i++){
        CXType arg_type = clang_getArgType(type, i);
        g_string_append(argstr, clang_getCString(clang_getTypeSpelling(arg_type)));
        if(i == num_args - 1){continue;}
        g_string_append(argstr, ",");
    }
    return argstr;
}

enum CXChildVisitResult visit_next_compound_stmt(CXCursor c, CXCursor parent, CXClientData data){
    CXCursor** next_compund = (CXCursor*)data;
    enum CXCursorKind kind = clang_getCursorKind(c);
    if(kind == CXCursor_CompoundStmt){
        *next_compund = &c;
        return CXChildVisit_Break;
    }
    return CXChildVisit_Recurse;
}

// Takes a cursor c it assumes points to a FunctionDecl, returns a cursor pointing to the body code (a compoundStmt);
CXCursor* get_function_body_cursor(CXCursor c){
    CXCursor* next_compound = NULL;
    clang_visitChildren(c, visit_next_compound_stmt, &next_compound);
    return next_compound;
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
       
        GString* argstr = get_function_arguments(identifier_cursor);
        identifier_info->args_types = g_strdup(argstr->str);
        g_string_free(argstr, TRUE);
    }

    return identifier_info;
}

void destroy_identifier_type_info(reference_type* identifier_info){
    // Function deallocating memory here
}

// Takes a cursor c it assumes points to a function body, fills out sets with the declared and undefined variables and type information about these:
enum CXChildVisitResult set_undefined_references(CXCursor c, CXCursor parent, CXClientData data){
    parsing_data* _data = (parsing_data*)data;
    GHashTable** undefined = _data->undefined_identifiers;
    GHashTable** declared = _data->declared_identifiers;

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

    // Build the type information
    reference_type* undefined_ref_type = create_identifier_type_info(referenced_cursor);
    undefined_ref_type->number = g_hash_table_size(*undefined);
    g_hash_table_insert(*undefined, identifier, undefined_ref_type);    

    return CXChildVisit_Continue;
}

void write_ref_typedef(char* identifier, reference_type* ref, GString* buffer){
    typedef int my_cooler_int;
    
    if(ref->is_function){
        g_string_append(buffer, g_strdup_printf("typedef %s(*%s_t)(%s);\n", ref->return_type, identifier, ref->args_types));
    } else {
        g_string_append(buffer, g_strdup_printf("typedef %s %s_t;\n", ref->return_type, identifier));     
    }
}

void write_ref_declaration(char* identifier, reference_type* ref, GString* buffer){
    if(ref->is_function){
        g_string_append(buffer, g_strdup_printf("%s_t %s = (%s_t)(&data[%d]);\n", identifier, identifier, identifier, ref->number));
    } else {
        g_string_append(buffer, g_strdup_printf("%s_t %s = *((%s_t*)(&data[%d]));\n", identifier, identifier, identifier, ref->number));
    }
    
}

void write_var_overwrite(char* identifier, reference_type* ref, GString* buffer){
    g_string_append(buffer, g_strdup_printf("*((%s_t*)(&data[%d])) = %s;\n", identifier, ref->number, identifier));
}

void free_reference_type(void* data){
    reference_type* value = (reference_type*)data;
    printf("I am freeing some shit bro!\n");
    g_free(value->return_type);
    g_free(value->args_types);
    g_free(value);
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
        g_string_append(document_buffer, g_strdup_printf("void %s (GtkWidget* widget, gpointer data){\n //PREFIX: \n ", function_name));

        GHashTableIter iter;
        g_hash_table_iter_init(&iter, undefined);
        
        char* undefined_identifier;
        reference_type* undefined_type;
        gpointer key, value;

        for(; g_hash_table_iter_next(&iter, &key, &value); ){
            undefined_identifier = (char*)key;
            undefined_type = (reference_type*)value;
            *required_identifiers = g_list_append(*required_identifiers, g_strdup(undefined_identifier));
            g_string_append(document_buffer, "    ");
            write_ref_typedef(undefined_identifier, undefined_type, document_buffer);
            g_string_append(document_buffer, "    ");
            write_ref_declaration(undefined_identifier, undefined_type, document_buffer);
        }

        // // print the actual function body here
        g_string_append(document_buffer, "\n //THEIR FUNCTION: \n");
        clang_visitChildren(c, add_cursor_to_buffer, document_buffer);
        g_string_append(document_buffer, "\n");
        g_string_append(document_buffer, "//POSTFIX: \n");

        // // print your function end here:
        g_hash_table_iter_init(&iter, undefined);
        for(; g_hash_table_iter_next(&iter, &key, &value); ){
            undefined_identifier = (char*)key;
            undefined_type = (reference_type*)value;
            if(undefined_type->is_function){continue;}
            g_string_append(document_buffer, "    ");
            write_var_overwrite(undefined_identifier, undefined_type, document_buffer);
        }

        g_string_append(document_buffer, "}");

        g_hash_table_destroy(undefined);
        g_hash_table_destroy(declarations);
        return CXChildVisit_Continue;
    }

    //write_cursor_element(&c, document_buffer);
    return CXChildVisit_Continue;
}

void* build_modified_function(CXCursor body_c, create_modified_function_document_data* _data){
    GHashTable* undefined = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable* declarations = g_hash_table_new(g_str_hash, g_str_equal);
    set_undefined_needed_data sets = {&undefined, &declarations};
    clang_visitChildren(body_c, set_undefined_references, &sets);

    // print your own function start here
    g_string_append(document_buffer, g_strdup_printf("void %s (GtkWidget* widget, gpointer data){\n //PREFIX: \n ", function_name));

    GHashTableIter iter;
    g_hash_table_iter_init(&iter, undefined);
    
    char* undefined_identifier;
    reference_type* undefined_type;
    gpointer key, value;

    for(; g_hash_table_iter_next(&iter, &key, &value); ){
        undefined_identifier = (char*)key;
        undefined_type = (reference_type*)value;
        *required_identifiers = g_list_append(*required_identifiers, g_strdup(undefined_identifier));
        g_string_append(document_buffer, "    ");
        write_ref_typedef(undefined_identifier, undefined_type, document_buffer);
        g_string_append(document_buffer, "    ");
        write_ref_declaration(undefined_identifier, undefined_type, document_buffer);
    }

    // // print the actual function body here
    g_string_append(document_buffer, "\n //THEIR FUNCTION: \n");
    clang_visitChildren(c, add_cursor_to_buffer, document_buffer);
    g_string_append(document_buffer, "\n");
    g_string_append(document_buffer, "//POSTFIX: \n");

    // // print your function end here:
    g_hash_table_iter_init(&iter, undefined);
    for(; g_hash_table_iter_next(&iter, &key, &value); ){
        undefined_identifier = (char*)key;
        undefined_type = (reference_type*)value;
        if(undefined_type->is_function){continue;}
        g_string_append(document_buffer, "    ");
        write_var_overwrite(undefined_identifier, undefined_type, document_buffer);
    }

    g_string_append(document_buffer, "}");

    g_hash_table_destroy(undefined);
    g_hash_table_destroy(declarations);
}

enum CXChildVisitResult clang_parse_build_buffer_ignore_other_identifiers(CXCursor c, CXCursor parent, CXClientData data){    
    parsing_data* _data = (parsing_data*)data;
    enum CXCursorKind kind = clang_getCursorKind(c);
    char* identifier = clang_getCString(clang_getCursorSpelling(c));

    // Check for cases to ignore:
    if( kind == CXCursor_VarDecl || 
        kind == CXCursor_MacroExpansion || 
        (kind == CXCursor_FunctionDecl && (strcmp(identifier, _data->callback_info->function_name) != 0)) ||
        !is_part_of_file(&c, _data->callback_info->document_path))

        {return CXChildVisit_Continue;}

    // Write it directly if it isn't a function:
    if( kind != CXCursor_FunctionDecl){
        g_string_append(
            _data->buffer,
            (kind == CXCursor_MacroDefinition) ? "#define" : ""
        );
        write_cursor_element(&c, _data->buffer);
        g_string_append(
            _data->buffer, 
            (kind == CXCursor_InclusionDirective || kind == CXCursor_MacroDefinition) ? "\n" : ";\n");

        return CXChildVisit_Continue;
    }

    // Write the modified function:
    CXCursor* c_body = get_function_body_cursor(c);




    if(kind == CXCursor_FunctionDecl){
        char* identifier = clang_getCString(clang_getCursorSpelling(c));
        if(strcmp(identifier, function_name) != 0)
        {return CXChildVisit_Continue;}

        CXCursor* function_body_cursor = get_function_body_cursor(c);


        clang_visitChildren(c, clang_parse_build_buffer_modified_function, data);
        return CXChildVisit_Continue;
    }
}

create_modified_function_document_data 
create_modified_document_and_find_identifiers(callback_code_information* callback_info){
    CXIndex index = clang_createIndex(0,0);
    CXTranslationUnit unit = clang_parseTranslationUnit(index, filepath, NULL, 0, NULL, 0, CXTranslationUnit_DetailedPreprocessingRecord);
    if (!unit) {
        create_modified_function_document_data nothing = {};
        return nothing;
        }

    CXCursor cursor = clang_getTranslationUnitCursor(unit);

    GHashTable* undefined_identifers = g_hash_table_new(g_str_hash, g_str_equal); // This needs to have a custom destroy function for the value!
    GHashTable* declarations = g_hash_table_new(g_str_hash, g_str_equal); // This does not, it only stores keys (works as a set), not values
    GString* buffer = g_string_new("");
    
    create_modified_function_document_data data = {callback_info, buffer, undefined_identifers, declarations};
    clang_visitChildren(cursor, clang_parse_build_buffer_ignore_other_identifiers, &data);

    return data;
}

// Takes a code_info with document_path and function_name set, fills out the rest
enum CXChildVisitResult clang_parse_function_code(CXCursor c, CXCursor parent, CXClientData data){    
    callback_code_information* code_info = (callback_code_information*)data;
    
    enum CXCursorKind kind = clang_getCursorKind(c);

    if(kind == CXCursor_VarDecl)
    {return CXChildVisit_Continue;}

    if(kind == CXCursor_FunctionDecl){
        char* identifier = clang_getCString(clang_getCursorSpelling(c));
        if(strcmp(identifier, code_info->function_name) != 0)
        {return CXChildVisit_Continue;}

        GString* args = get_function_arguments(c);
        code_info->args_code = g_strdup(args->str);
        g_string_free(args, TRUE);



        clang_visitChildren(c, clang_parse_build_buffer_modified_function, data);
        return CXChildVisit_Continue;
    }

    if(kind == CXCursor_MacroExpansion){
        return CXChildVisit_Continue;
    }

    if(!is_part_of_file(&c, document_path))
    {return CXChildVisit_Continue;}

    if(kind == CXCursor_MacroDefinition){
        g_string_append(document_buffer, "#define ");
    } 

    write_cursor_element(&c, document_buffer);
    

    if(kind == CXCursor_InclusionDirective || kind == CXCursor_MacroDefinition)
    {
        g_string_append(document_buffer, "\n");
    } else {
        g_string_append(document_buffer, ";\n");
    }
    return CXChildVisit_Continue;
}

void set_function_code(callback_code_information* code_info){
    if(code_info->document_path == NULL){
        g_print("You need to fill out document_path before calling set_function_code!\n");
        return;
    }
    if(code_info->function_name == NULL){
        g_print("You need to fill out function_name before calling set_function_code!\n");
        return;
    }

    CXIndex index = clang_createIndex(0,0);
    CXTranslationUnit unit = clang_parseTranslationUnit(index, code_info->document_path, NULL, 0, NULL, 0, CXTranslationUnit_DetailedPreprocessingRecord);
    if (!unit) {
        return;
        }

    CXCursor cursor = clang_getTranslationUnitCursor(unit);

    clang_visitChildren(cursor, clang_parse_function_code, code_info);
}

