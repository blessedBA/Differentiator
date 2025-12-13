#ifndef MATH_EXP_PARSER_H
#define MATH_EXP_PARSER_H

#include <stdio.h>

int getRuleG(const char* math_expression, size_t* index);
void printSyntaxError(const char* math_exp, size_t* position);

#endif // MATH_EXP_PARSER_H
