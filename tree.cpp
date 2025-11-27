#include "colors.h"
#include "debugUtils.h"
#include "safetyTreeDiff.h"
#include "tree.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>



static bool setValue (tree_t* tree, node_t* node, type_t type, void* value);
static int identifyVariable (tree_t* tree, void* value);
static char* getNameType (type_t type);

operation_t operations[NUMBER_OPERATIONS] = { {NULL_OPER,   "null_value"}, {ADDITION,       "+"},
                                              {SUBTRACTION, "-"},          {MULTIPLICATION, "*"},
                                              {DIVISION,    "/"} };

tree_t* treeInit ()
{
    tree_t* tree = (tree_t*)calloc(1, sizeof(tree_t));
    if (tree == nullptr) return nullptr;

    DEBUG_PRINT("struct variables right after allocate memory for tree [%p]\n", tree->variables);

    tree->variables[0] = {.value = BAD_VALUE, .name = 'x'};
    tree->variables[1] = {.value = BAD_VALUE, .name = 'y'};
    tree->variables[2] = {.value = BAD_VALUE, .name = 'z'};
    tree->variables[3] = {.value = BAD_VALUE, .name = 'a'};
    tree->variables[4] = {.value = BAD_VALUE, .name = 'b'};

    DEBUG_PRINT(COLOR_BBLUE "variables right after initialization: \n" COLOR_RESET);
    #ifndef NDEBUG
    for (size_t index = 0; index < NUMBER_VARIABLES; index++)
    {
        DEBUG_PRINT("variable[%zu] = %lf, name = %c\n", index, tree->variables[index].value, tree->variables[index].name);
    }
    DEBUG_PRINT("\n");
    #endif

    node_t* root = (node_t*)calloc(1, sizeof(node_t));
    if (root == nullptr) return nullptr;

    DEBUG_PRINT("value in root [%p]\n", &root->value);

    root->type.code_type = TYPE_NULL;
    root->type.name_type = "nothing_type";
    root->value.index    = 0;
    root->left           = nullptr;
    root->right          = nullptr;
    root->parent         = nullptr;
    tree->root           = root;
    tree->size           = 1;

    DEBUG_PRINT("value in root after initialization [%p]\n", &root->value);

    if (TREE_VERIFY) return nullptr;

    return tree;
}

bool nodeInit (tree_t* tree, node_t* node, side_t side, type_t type, void* value)
{
    if (TREE_VERIFY) return true;
    assert(node);

    DEBUG_PRINT_LOCATION;
    DEBUG_PRINT(COLOR_MAGENTA "node after which i want init new node: \n");
    DEBUG_PRINT("node [%p]\n", node);
    DEBUG_PRINT("parent = %p\n", node->parent);
    DEBUG_PRINT("type = %d (%s)\n", node->type.code_type, node->type.name_type);
    DEBUG_PRINT_VALUE(node, node->type.code_type);
    DEBUG_PRINT("\n" COLOR_RESET);

    node_t* new_node = (node_t*)calloc(1, sizeof(node_t));
    if (new_node == nullptr)
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed to allocate memory for new node in %s %s:%d\n" COLOR_RESET,
                    __func__, __FILE__, __LINE__);
        return true;
    }

    switch (side)
    {
        case LEFT:
            new_node->type.code_type = type;
            new_node->type.name_type = getNameType(type);
            if (setValue(tree, new_node, type, value)) return true;
            node->left = new_node;
            break;
        case RIGHT:
            new_node->type.code_type = type;
            new_node->type.name_type = getNameType(type);
            DEBUG_PRINT("new_node->type IN nodeInit = %d (%s)\n", new_node->type.code_type, new_node->type.name_type);

            if (setValue(tree, new_node, type, value)) return true;
            node->right = new_node;
            break;
        default:
            assert(0 && "invalid side for adding new node\n");
    }
    new_node->parent = node;
    tree->size++;

    if (TREE_VERIFY) return true;
    assert(node);

    return false;
}

node_t* creatNode() // for reading from file
{
    node_t* node = (node_t*)calloc(1, sizeof(node_t));
    if (node == nullptr)
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed to allocate memory for node in %s %s:%d\n" COLOR_RESET,
                    __func__, __FILE__, __LINE__);
        return nullptr;
    }
    node->parent = nullptr;
    node->left   = nullptr;
    node->right  = nullptr;

    return node;
}

bool setValue (tree_t* tree, node_t* node, type_t type, void* value)
{
    assert(node && value);
    DEBUG_PRINT_LOCATION;
    DEBUG_PRINT("node [%p]\n", node);
    DEBUG_PRINT("type = %d\n", type);
    DEBUG_PRINT_VALUE(node, node->type.code_type);

    switch (type)
    {
        case TYPE_NULL:
            fprintf(stderr, COLOR_BRED "ERROR: invalid type of node\n" COLOR_RESET);
            return true;
        case OPERATION:
            node->value.oper.name = (char*)value;
            node->value.oper.code = getCodeOperation((char*)value);
            break;
        case VARIABLE:
        {
            int temp = identifyVariable(tree, value);
            if (temp == BAD_VALUE)
            {
                setError(ERR_INV_NAME_VAR);
                return true;
            }
            node->value.index = temp;
            break;
        }
        case NUMBER:
            node->value.number = *(double*)value;
            break;
        default:
            fprintf(stderr, COLOR_BRED "FATAL ERROR: unexpected error in %s:%d\n" COLOR_RESET, __func__, __LINE__);
            return true;
    }

    DEBUG_PRINT(COLOR_BLUE "\nafter setting a value:\n" COLOR_RESET);
    DEBUG_PRINT("node [%p]\n", node);
    DEBUG_PRINT("type = %d\n", type);
    DEBUG_PRINT_VALUE(node, node->type.code_type);

    return false;
}

void treeDestroy (tree_t* tree)
{
    TREE_VERIFY;

    nodeDestroy(tree, tree->root, 1);
    free(tree);

    return;
}

void nodeDestroy (tree_t* tree, node_t* node, int rank)
{
    TREE_VERIFY;
    assert(node);

    tree->size--;
    if (node->left)  nodeDestroy(tree, node->left,  rank + 1);
    if (node->right) nodeDestroy(tree, node->right, rank + 1);

    if (node->parent != nullptr)
    {
        if (node->parent->left  == node) node->parent->left  = nullptr;
        if (node->parent->right == node) node->parent->right = nullptr;
        node->parent = nullptr;
    }

    node->type.code_type = TYPE_NULL;
    node->type.name_type = "nothing_type";
    node->left           = nullptr;
    node->right          = nullptr;

    //free(node->type.name_type);
    if (node->type.code_type == OPERATION) free(node->value.oper.name);
    free(node);

    return;
}

int identifyVariable (tree_t* tree, void* value)
{
    TREE_VERIFY;
    assert(value);

    char name_variable = *(char*)value;

    for (int index = 0; index < NUMBER_VARIABLES; index++)
    {
        if (tree->variables[index].name == name_variable) return index;
    }

    TREE_VERIFY;
    assert(value);

    return BAD_VALUE;
}

char* getNameType (type_t type)
{
    char* temp_type = (char*)calloc(MAX_SIZE_OF_NAME_OPERATION, sizeof(char)); //NOTE - don't forget to free memory!

    if (temp_type == nullptr)
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed allocate memory for temp_type in %s %s:%d\n" COLOR_RESET, __func__, __FILE__, __LINE__);
        return nullptr;
    }

    switch (type)
    {
        case TYPE_NULL:
            temp_type = "NULL_TYPE";
            break;
        case OPERATION:
            temp_type = "OPERATION";
            break;
        case VARIABLE:
            temp_type = "VARIABLE";
            break;
        case NUMBER:
            temp_type = "NUMBER";
            break;
        default:
            fprintf(stderr, COLOR_BRED "ERROR: invalid type of node in %s %s:%d\n" COLOR_RESET, __func__, __FILE__, __LINE__);
    }

    return temp_type;
}

// char* getValue (node_t* node)
// {
//     assert(node);
//
//     char* temp = (char*)calloc(MAX_SIZE_OF_GENERAL_VALUE, sizeof(char));
//
//     switch (node->type.code_type)
//     {
//         case TYPE_NULL:
//             strncpy(temp, "null_value", sizeof("null_value"
//     }
//
//     assert(node);
//
//
// }

oper_t getCodeOperation (char* value)
{
    for (int index = 0; index < NUMBER_OPERATIONS; index++)
    {
        if (strcmp(operations[index].name, value) == 0) return operations[index].code;
    }

    return NULL_OPER; // TODO добавить какую то другую константу, отвечающую за нераспознанную операцию
}
