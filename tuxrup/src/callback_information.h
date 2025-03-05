#ifndef CALLBACK_INFORMATION_H
#define CALLBACK_INFORMATION_H

typedef struct {
    char* function_name;
    char* document_path;
    char* before_code;
    char* args_code;
    char* function_code;
    char* after_code;
    char* definitions_code;
} callback_code_information;

callback_code_information* get_callback_code_information(void* callback, char* callback_name);

void free_callback_code_information(callback_code_information* info);

#endif