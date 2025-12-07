#ifndef READ_MATH_EXP_H
#define READ_MATH_EXP_H

#include <stdio.h>

typedef struct  mathExp_t
{
    char* expression = nullptr;
    size_t size_expression = 0;
    size_t capacity_expression = 0;
    size_t position = 0;
} mathExp_t;

tree_t* readFromFile (const char* file_name);

#endif // READ_MATH_EXP_H
