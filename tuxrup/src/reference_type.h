#ifndef REFERENCE_TYPE_H
#define REFERENCE_TYPE_H

typedef struct {
    int number;
    bool is_function;
    char* return_type;
    char* args_types;
} reference_type;

#endif