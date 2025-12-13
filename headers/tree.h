#ifndef TREE_H
#define TREE_H

#include "errors.h"

#include <stdio.h>

#define NUM_TYPES_OPER   4
#define NUMBER_VARIABLES 5
#define BAD_VALUE       -666
#define NUMBER_OPERATIONS 5
#define MAX_SIZE_OF_NAME_OPERATION 10
#define MAX_SIZE_OF_GENERAL_VALUE  100
#define MAX_LENGTH_OF_NUMBER       10
typedef int index_t;

typedef enum side_t
{
    WRONG_SIDE = 0,
    LEFT       = 1,
    RIGHT      = 2
} side_t;

typedef enum mode_print
{
    PREORDER  = 1,
    POSTORDER = 2,
    INORDER   = 3
} mode_print;

typedef enum mode_dump
{
    ERROR         = 0,
    BEFORE_INSERT = 1,
    AFTER_INSERT  = 2,
    BEFORE_DELETE = 3,
    AFTER_DELETE  = 4
} mode_dump;

typedef enum type_t
{
    TYPE_NULL = 0,
    OPERATION = 1,
    VARIABLE  = 2,
    NUMBER    = 3
} type_t;

typedef enum prior_t
{
    WRONG_PR    = 0,
    FIRST_PR    = 1,
    SECOND_PR   = 2,
    THIRD_PR    = 3,
    FOURTH_PR   = 4
} prior_t;

typedef enum oper_t
{
    NULL_OPER      = 0,
    ADDITION       = 1,
    SUBTRACTION    = 2,
    MULTIPLICATION = 3,
    DIVISION       = 4,
    SIN            = 5,
    COS            = 6
} oper_t;

typedef struct operation_t
{
    oper_t code;
    const char*  name;
} operation_t;

typedef union value_t
{
    operation_t oper;
    int         index;
    double      number;
} value_t;

typedef struct variable_t
{
    double value = BAD_VALUE;
    char   name;
} variable_t;

typedef struct type_node
{
    type_t code_type = TYPE_NULL;
    const char* name_type  = "nothing_type";
} type_node;

typedef struct node_t
{
    type_node type;
    value_t  value;
    node_t*  left;
    node_t*  right;
    node_t*  parent;
    int      priority; // TODO приоритет можно просто числом
} node_t;

typedef struct tree_t
{
    node_t*    root;
    size_t     size;
    variable_t variables[NUMBER_VARIABLES];
} tree_t;


tree_t* treeInit ();
bool nodeInit    (tree_t* tree, node_t* node, side_t side, type_t type, void* value);
void treeDestroy (tree_t* tree);
void nodeDestroy (tree_t* tree, node_t* node, int rank);

node_t* creatNode(type_t type, value_t value, node_t* left, node_t* right);

void printNode          (const node_t* node, int rank, mode_print mode);
void printNodePREorder  (const node_t* node, int rank);
void printNodePOSTorder (const node_t* node, int rank);
void printNodeINorder   (const node_t* node, int rank);
bool printInOrder       (const tree_t* tree, const node_t* node);

int identifyVariable (tree_t* tree, void* value);
oper_t getCodeOperation (char* value);

const char* getNameOperation (oper_t code_operation);
const char* getShortNameOperation (oper_t code_operation);
const char* getNameType (type_t type);

int getPriorityNode (oper_t code_operation);
side_t getSideOfNodeInParentNode (tree_t* tree, node_t* node);
void deleteNodeWithoutSubtree (tree_t* tree, node_t* node);

#endif // TREE_H
