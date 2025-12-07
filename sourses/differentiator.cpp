#include "colors.h"
#include "../../SquareSolver1/FuncsForRealNum.h"
#include "debugUtils.h"
#include "errors.h"
#include "differentiator.h"
#include "errors.h"
#include "Files.h"
#include "latexDumpDiff.h"
#include "tree.h"
#include "safetyTreeDiff.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <unistd.h>

static tree_t* diffTreeInit (const tree_t* tree);
static node_t* getDiffNode  (const tree_t* tree, tree_t* diff_tree, const node_t* node, char diff_variable);
static node_t* copySubtree (const tree_t* tree, const node_t* node);
static node_t* creatOperatorNode (tree_t* tree, node_t* left_node, node_t* right_node, oper_t code_operation);

extern FILE* global_latex_file;
extern int global_count_dumps;
extern FILE* global_log_file_html;

tree_t* startDiffTree (const tree_t* tree)
{
    TREE_VERIFY;

    tree_t* diff_tree = diffTreeInit(tree);

    if (!diff_tree)
    {
        fprintf(stderr, COLOR_BRED "FATAL ERROR (%s:%d): failed to allocate memory in diffTreeInit\n" COLOR_RESET,
                    __FILE__, __LINE__);
        exit(1);
    }

    diff_tree->root = getDiffNode(tree, diff_tree, tree->root, 'x');

    TREE_VERIFY;

    return diff_tree;
}

tree_t* diffTreeInit (const tree_t* tree)
{
    TREE_VERIFY;

    tree_t* diff_tree = (tree_t*)calloc(1, sizeof(tree_t));
    if (!diff_tree)
    {
        LOG_ERROR(&global_error_log, ERR_FAIL_INIT_DIFF_TREE, code_ERR_FAIL_INIT_DIFF_TREE,
                  "failed to allocate memory for init diff tree!");
        ADD_CONTEXT(&global_error_log);
        return nullptr;
    }

    int count_variables = sizeof(tree->variables) / sizeof(tree->variables[0]);
    DEBUG_PRINT("count_variables = %d\n", count_variables);

    for (int index = 0; index < count_variables; index++)
    {
        diff_tree->variables[index] = tree->variables[index];
    }

    node_t* root = (node_t*)calloc(1, sizeof(node_t));
    // diff_tree->root = nullptr;
    // diff_tree->size = 0;
    if (!root)
    {
        LOG_ERROR(&global_error_log, ERR_FAIL_INIT_ROOT_DIFF_TREE, code_ERR_FAIL_INIT_ROOT_DIFF_TREE,
                  "failed to allocate memory for root of diff tree");
        ADD_CONTEXT(&global_error_log);
        return nullptr;
    }

    DEBUG_PRINT("value in root of diff tree [%p]\n", &root->value);

    root->type.code_type = TYPE_NULL;
    root->type.name_type = "nothing_type";
    root->value.index    = 0;
    root->left           = nullptr;
    root->right          = nullptr;
    root->parent         = nullptr;
    diff_tree->root      = root;
    diff_tree->size      = 1;

    TREE_VERIFY;

    return diff_tree;
}

node_t* getDiffNode (const tree_t* tree, tree_t* diff_tree, const node_t* node, char diff_variable)
{
    TREE_VERIFY;
    assert(diff_tree);

    node_t* diff_node = nullptr;
    char curr_variable = '\0';
    double zero = 0.0;
    double one  = 1.0;

    switch (node->type.code_type)
    {
        case NUMBER:
            diff_node = creatDiffNode(NUMBER, value_t {.number = 0});
            break;
        case VARIABLE:
            curr_variable = tree->variables[node->value.index].name;
            diff_node = (curr_variable == diff_variable) ?
                                creatDiffNode(NUMBER, value_t {.number = 1}) :
                                creatDiffNode(NUMBER, value_t {.number = 0});
            break;
        case OPERATION:
        {
            diff_node = nullptr;
            switch (node->value.oper.code)
            {
                case ADDITION:
                case SUBTRACTION:
                {
                    diff_node = (node->value.oper.code == ADDITION) ?
                                ADD_(DIFF_LEFT, DIFF_RIGHT) :
                                SUB_(DIFF_LEFT, DIFF_RIGHT);
                    break;
                }
                case MULTIPLICATION:
                {
                    if (node->left->type.code_type == NUMBER) // this work good
                        diff_node = MUL_(COPY_LEFT, DIFF_RIGHT);
                    else if (node->right->type.code_type == NUMBER)
                        diff_node = MUL_(DIFF_LEFT, COPY_RIGHT);
                    else
                        diff_node = ADD_(MUL_(DIFF_LEFT, COPY_RIGHT), MUL_(COPY_LEFT, DIFF_RIGHT));
                    break;
                }
                case DIVISION:
                {
                    diff_node = DIV_(SUB_(MUL_(DIFF_LEFT, COPY_RIGHT), MUL_(COPY_LEFT, DIFF_RIGHT)),
                                     MUL_(COPY_RIGHT, COPY_RIGHT));
                    break;
                }
                case SIN:
                {
                    diff_node = MUL_(COS_(COPY_LEFT), DIFF_LEFT);
                    break;
                }
                case COS:
                {
                    diff_node = MUL_(MUL_(NUM_(-1), SIN_(COPY_LEFT)), DIFF_LEFT);
                    break;
                }
                default:
                    LOG_ERROR(&global_error_log, ERR_INVALID_CODE_OPER, code_ERR_INVALID_CODE_OPER, "invalid code of operation in getDiffNode");
                    ADD_CONTEXT(&global_error_log);
            }
        }
    }

    TREE_VERIFY;

    treeDump (global_log_file_html, diff_tree, diff_node, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);
    return diff_node;
}

// void diffOperationNode (const tree_t* tree, tree_t** diff_node, node_t* node, )
// {

// }
node_t* copySubtree (const tree_t* tree, const node_t* node)
{
    if (!node) return nullptr;

    node_t* new_node = creatDiffNode(node->type.code_type, node->value);

    new_node->left  = copySubtree(tree, node->left);
    if (new_node->left) new_node->left->parent = new_node;

    new_node->right = copySubtree(tree, node->right);
    if (new_node->right) new_node->right->parent = new_node;

    treeDump (global_log_file_html, tree, new_node, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);
    return new_node;
}

node_t* creatDiffNode (type_t type, value_t value)
{
    node_t* node = creatNode();
    if (!node) return nullptr;

    node->type.code_type = type;
    node->type.name_type = getNameType(type);

    switch (type)
    {
        case OPERATION:
        {
            node->value.oper.code = value.oper.code;
            node->priority        = getPriorityNode(node->value.oper.code);
            char* name_buffer = (char*)calloc(MAX_SIZE_OF_NAME_OPERATION, sizeof(char));
            if (!name_buffer)
            {
                LOG_ERROR(&global_error_log, ERR_FAIL_INIT_NAME_OPER, code_ERR_FAIL_INIT_NAME_OPER,
                          "failed to allocate memory for name of operation in creatDiffNode");
                ADD_CONTEXT(&global_error_log);
                free(node);
                return nullptr;
            }

            const char* src_name = getShortNameOperation(value.oper.code);
            strncpy(name_buffer, src_name, MAX_SIZE_OF_NAME_OPERATION - 1);
            name_buffer[MAX_SIZE_OF_NAME_OPERATION - 1] = '\0';
            node->value.oper.name = name_buffer;
            break;
        }
        case VARIABLE:
            node->value.index = value.index;
            break;
        case NUMBER:
            node->value.number = value.number;
            break;
        case TYPE_NULL:
        default:
            node->value.index = 0;
            break;
    }

    DEBUG_PRINT("creatDiffNode: node->type.code_type = %d\n", node->type.code_type);


    assert(node);

    return node;
}

node_t* creatOperatorNode (tree_t* tree, node_t* left_node, node_t* right_node, oper_t code_operation)
{
    assert(tree);
    assert(left_node);

    node_t* operator_node = creatDiffNode(OPERATION, value_t { .oper = {code_operation, getShortNameOperation(code_operation)} });
    operator_node->left  = left_node;
    operator_node->right = right_node;
    operator_node->priority = getPriorityNode(code_operation);
    left_node->parent  = operator_node;
    if (right_node) right_node->parent = operator_node;

    assert(tree);
    assert(left_node);
    treeDump (global_log_file_html, tree, operator_node, LOC_CALL, nullptr, " DIFF TREE DUMP %d", ++global_count_dumps);
    
    return operator_node;
}


