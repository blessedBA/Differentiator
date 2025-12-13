#pragma once

#include "tree.h"

#include <stdio.h>

typedef struct  mathExp_t
{
    char* expression = nullptr;
    size_t size_expression = 0;
    size_t capacity_expression = 0;
    size_t position = 0;
} mathExp_t;

void readFromFile (tree_t* tree, const char* file_name);
node_t* getRuleG  (tree_t* tree, mathExp_t* math_exp);
node_t* getRuleV  (tree_t* tree, mathExp_t* math_exp);
node_t* getRuleN  (mathExp_t* math_exp);
node_t* getRuleE  (mathExp_t* math_exp);
node_t* getRuleT  (mathExp_t* math_exp);
node_t* getRuleP  (mathExp_t* math_exp);