#include "reference_type.h"
#include "fix_function_body.h"

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
        g_string_append(buffer, g_strdup_printf("%s_t %s = (%s_t)(_data[%d]);\n", identifier, identifier, identifier, ref->number));
    } else {
        g_string_append(buffer, g_strdup_printf("%s_t %s = *((%s_t*)(_data[%d]));\n", identifier, identifier, identifier, ref->number));
    }
    
}

void write_var_overwrite(char* identifier, reference_type* ref, GString* buffer){
    g_string_append(buffer, g_strdup_printf("*((%s_t*)(_data[%d])) = %s;\n", identifier, ref->number, identifier));
}

char* create_fixed_function_body(char* modified_code, GHashTable* undefined_identifiers){
    // PREFIX
    GString* buffer = g_string_new("//PREFIX: \n");
    g_string_append(buffer, "void** _data = (void**)data; \n");

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, undefined_identifiers);
    
    for(; g_hash_table_iter_next(&iter, &key, &value); ){
        char* undefined_identifier = (char*)key;
        reference_type* undefined_type = (reference_type*)value;
        
        g_string_append(buffer, "    ");
        write_ref_typedef(undefined_identifier, undefined_type, buffer);
        g_string_append(buffer, "    ");
        write_ref_declaration(undefined_identifier, undefined_type, buffer);
    }

    // BODY
    g_string_append(buffer, "\n //THEIR FUNCTION: \n");
    g_string_append(buffer, modified_code);
    g_string_append(buffer, "\n");

    // POSTFIX
    g_string_append(buffer, "//POSTFIX: \n");
    g_hash_table_iter_init(&iter, undefined_identifiers);
    for(; g_hash_table_iter_next(&iter, &key, &value); ){
        char* undefined_identifier = (char*)key;
        reference_type* undefined_type = (reference_type*)value;
        if(undefined_type->is_function){continue;}

        g_string_append(buffer, "    ");
        write_var_overwrite(undefined_identifier, undefined_type, buffer);
    }

    char* str = g_strdup(buffer->str);
    g_string_free(buffer, TRUE);
    return str;
}