#include "colors.h"
#include "debugUtils.h"
#include "fileFuncs.h"
#include "Files.h"
#include "safetyTreeDiff.h"
#include "tree.h"
#include "../SquareSolver1/FuncsForRealNum.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static char*   creatBuffer        (const char* filename);
static void    writeInFile        (tree_t* tree, node_t* node, FILE* output_file);
static node_t* readTreeFromFile   (tree_t* tree, char* buffer, size_t* position, size_t* size, node_t* parent_node);
static void    writeInfoInNode    (tree_t* tree, char* buffer, size_t* position, node_t* node);
static bool    writeVariable      (tree_t* tree, char* buffer, size_t* position, node_t* node);
static void    skipSpacesInBuffer (char* buffer, size_t* position);
static bool    checkNil           (char* buffer, size_t* position);
static bool    checkVariable      (char* buffer, size_t* position);

static bool openConcreteFile (FILE** ptr_to_file, char* file_name, const char* standard_name_file,
                              char** str_to_save_file_name, const char* mode_opening);

extern operation_t operations[NUMBER_OPERATIONS];

int openFiles(FlagStorage* storage, FileStorage* file_storage)
{
    assert(storage);
    assert(file_storage);

    const char* r = "r";
    const char* w = "w";

    if (openConcreteFile(&file_storage->input_file.pointer,    storage->input_file_name,      STANDARD_FILE_FOR_READ,
                         &file_storage->input_file.name,    r)) return -1;

    if (openConcreteFile(&file_storage->log_file_html.pointer, storage->dump_html_file_name,  STANDARD_FILE_FOR_HTML_DUMP,
                         &file_storage->log_file_html.name, w)) return -1;

    if (openConcreteFile(&file_storage->latex_file.pointer,    storage->dump_latex_file_name, STANDARD_FILE_FOR_LATEX_DUMP,
                         &file_storage->latex_file.name,    w)) return -1;

    assert(storage);
    assert(file_storage);

    return 0;
}

bool openConcreteFile (FILE** ptr_to_file, char* file_name, const char* standard_name_file,
                       char** str_to_save_file_name, const char* mode_opening)
{
    if (file_name)
    {
        *ptr_to_file = fopen(file_name, mode_opening);
        *str_to_save_file_name = file_name;
    }
    else
    {
        *ptr_to_file = fopen(standard_name_file, mode_opening);
        *str_to_save_file_name = (char*)standard_name_file;
    }
    if (!(*ptr_to_file))
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed to open file for latex dump!\n" COLOR_RESET);
        return true;
    }

    assert(ptr_to_file);

    return false;
}

void closeFiles (FileStorage* file_storage)
{
    assert(file_storage);

    if (file_storage->input_file.pointer)
    {
        fclose(file_storage->input_file.pointer);
        file_storage->input_file.name = nullptr;
    }
    if (file_storage->log_file_html.pointer)
    {
        fclose(file_storage->log_file_html.pointer);
        file_storage->log_file_html.name = nullptr;
    }
    if (file_storage->latex_file.pointer)
    {
        fclose(file_storage->latex_file.pointer);
        file_storage->latex_file.name = nullptr;
    }

    assert(file_storage);

    return;
}

bool clearFile (const char* file_name)
{
    assert(file_name);

    FILE* file = fopen(file_name, "w");
    if (file == nullptr) return true;
    fclose(file);

    assert(file_name);

    return false;
}

FILE* getFilename (class_file_t class_file)
{
    char file_name[MAX_LENGTH_FILENAME] = "";
    char mode_opening[2] = "";

    switch (class_file)
    {
        case DUMP_FILE:
            printf("which dump file do you want to use?\n");
            do scanf("%s", file_name);
            while (strlen(file_name) == MAX_LENGTH_FILENAME && printf("too big name for file, try another one\n"));
            mode_opening[0] = 'w';
            break;
        case READING_FILE:
            printf("which file do you want to read from?\n");
            do scanf("%s", file_name);
            while (strlen(file_name) == MAX_LENGTH_FILENAME && printf("too big name for file, try another one\n"));
            mode_opening[0] = 'r';
            break;
        default:
            fprintf(stderr, COLOR_BRED "\nERROR: invalid class of file in %s %s:%d\n" COLOR_RESET, __func__, __FILE__, __LINE__);
            return nullptr;
    }

    DEBUG_PRINT(COLOR_BBLUE "after reading info about file:\n" COLOR_RESET);
    DEBUG_PRINT("file_name [%p] = %s\n",file_name, file_name);
    DEBUG_PRINT("mode_opening = %s\n", mode_opening);

    FILE* opened_file = fopen(file_name, mode_opening);

    if (opened_file == nullptr)
    {
        fprintf(stderr, COLOR_BRED "\nERROR: failed to open file "
                        COLOR_BBLUE "%s "   COLOR_BRED "in mode "
                        COLOR_BBLUE " %s\n" COLOR_RESET, file_name, mode_opening);
    }

    return opened_file;
}

void startWriting(tree_t* tree)
{
    TREE_VERIFY;

    printf("writing.");
    usleep(1e5);
    printf(".");
    usleep(1e5);
    printf(".\n");

    FILE* output_file = fopen(STANDARD_FILE_FOR_READ, "w");
    assert(output_file);

    writeInFile(tree, tree->root, output_file);

    printf(COLOR_GREEN "writing have done :)" COLOR_RESET "\n");

    fclose(output_file);

    TREE_VERIFY;

    return;
}

void writeInFile (tree_t* tree, node_t* node, FILE* output_file)
{
    TREE_VERIFY;
    assert(output_file);

    fprintf(output_file, "( ");
    switch (node->type.code_type)
    {
        case TYPE_NULL:
            fprintf(output_file, " null_value ");
            break;
        case OPERATION:
            fprintf(output_file, " %s ", node->value.oper.name);
            break;
        case VARIABLE:
            fprintf(output_file, " %c ", tree->variables[node->value.index].name);
            break;
        case NUMBER:
            fprintf(output_file, " %lf ", node->value.number);
            break;
        default:
            fprintf(stderr, COLOR_BRED "ERROR: invalid type of operation in %s %s:%d\n", __func__, __FILE__, __LINE__);
    }

    if (node->left)  writeInFile(tree, node->left,  output_file);
    if (node->right) writeInFile(tree, node->right, output_file);
    if (node->left == nullptr && node->right == nullptr) fprintf(output_file, " nil nil ");

    fprintf(output_file, ") ");

    TREE_VERIFY;
    assert(output_file);

    return;
}

node_t* startReading(tree_t* tree, FileStorage* file_storage)
{
    TREE_VERIFY;

    printf("reading.");
    usleep(5e5);
    printf(".");
    usleep(5e5);
    printf(".\n\n");

    char* buffer = creatBuffer(file_storage->input_file.name);
    //printf("%s", buffer);
    size_t position = 0;

    DEBUG_PRINT(COLOR_BBLUE "the beginning of reading file\n" COLOR_RESET);
    tree->root = readTreeFromFile(tree, buffer, &position, &tree->size, nullptr);
    DEBUG_PRINT(COLOR_BBLUE "the end of reading file\n" COLOR_RESET);

    printf(COLOR_GREEN "reading have done :)" COLOR_RESET "\n");

    TREE_VERIFY;

    return tree->root;
}

char* creatBuffer (const char* filename)
{
    assert(filename);

    FILE* input_file = fopen(filename, "r");
    assert(input_file);

    struct stat st = {};
    const int get_info = (int)stat(filename, &st);
    const size_t size_file = (size_t)st.st_size;
    if (get_info != 0)  assert(0 && "failed to get info about file");

    char* buffer = (char*)calloc(size_file + 2, sizeof(char));
    if (fread(buffer, sizeof(char), size_file, input_file) != size_file)
    {
        fprintf(stderr, COLOR_BRED "failed to read file to buffer" COLOR_RESET "\n");
        return nullptr;
    }

    assert(filename);

    return buffer;
}

node_t* readTreeFromFile(tree_t* tree, char* buffer, size_t* position, size_t* size, node_t* parent_node)
{
    TREE_VERIFY;
    assert(position && size);

    node_t* node = nullptr;
    skipSpacesInBuffer(buffer, position);
    DEBUG_PRINT("buffer[position] = %c " COLOR_BYELLOW " (expected '(')\n" COLOR_RESET, buffer[*position]);
    if (buffer[*position] == '(')
    {
        DEBUG_PRINT("if ACTIVATED\n");
        (*size)++;
        (*position)++;
        node = creatNode();
        if (node == nullptr) return nullptr;
        node->parent = parent_node;
    }
    else fprintf(stderr, COLOR_BRED "ERROR: unexpected symbol ('%c') in buffer\n" COLOR_RESET, buffer[*position]);

    DEBUG_PRINT("position [%p]\n", position);
    DEBUG_PRINT("position in %s = %zu\n", __func__, *position);

    usleep(1e5);

    DEBUG_PRINT("\nnode before call writeInfoInNode [%p]\n", node);

    writeInfoInNode(tree, buffer, position, node);

    DEBUG_PRINT("\nnode after call writeInfoInNode [%p]:\n", node);
    DEBUG_PRINT("type of node = %d (%s)\n", node->type.code_type, node->type.name_type);

    if (checkNil(buffer, position))
    {
        skipSpacesInBuffer(buffer, position);
        checkNil(buffer, position);
        skipSpacesInBuffer(buffer, position);
        if (buffer[*position] != ')') assert(0);
        (*position)++; // skip ')'

        return node;
    }
    node->left  = readTreeFromFile(tree, buffer, position, size, node);
    node->right = readTreeFromFile(tree, buffer, position, size, node);

    DEBUG_PRINT("\nnode->left = %p\n", node->left);
    DEBUG_PRINT("node->right = %p\n\n", node->right);

    skipSpacesInBuffer(buffer, position);
    (*position)++; // skip ')'
    DEBUG_PRINT("')' SKIPPED\n");
    assert(position && size);

    return node;
}

void writeInfoInNode (tree_t* tree, char* buffer, size_t* position, node_t* node)
{
    TREE_VERIFY;
    assert(buffer && position);

    DEBUG_PRINT(COLOR_BBLUE "\nIN %s:\n" COLOR_RESET, __func__);

    char* operation = (char*)calloc(MAX_SIZE_OF_NAME_OPERATION, sizeof(char));
    if (operation == nullptr) fprintf(stderr, COLOR_BRED "ERROR: failed to allocate memory for operation in %s %s:%d\n"
                                              COLOR_RESET, __func__, __FILE__, __LINE__);

    char number_str[MAX_LENGTH_OF_NUMBER] = "";
    double number = 0;

    skipSpacesInBuffer(buffer, position);

    DEBUG_PRINT("position in %s = %zu\n", __func__, *position);
    DEBUG_PRINT("buffer[*position] in %s before scanning = %c\n", __func__, buffer[*position]);

    sscanf(buffer + *position, "%9s", number_str);
    number = atoi(number_str);

    if (compareDoubleNumbers(number, 0)) // TODO it will be very good to add processing invalid input
                                          // NOTE DONT PROCESS NUMBER 0, BE ATTENTIVE
    {
        if (checkVariable(buffer, position))
        {
            DEBUG_PRINT(COLOR_BMAGENTA "\nVARIABLE\n" COLOR_RESET);
            if (writeVariable(tree, buffer, position, node) == false)
            {
                // TODO  add processing errors
            }
            (*position)++;
            DEBUG_PRINT("index of variable = %d\n", node->value.index);
        }
        else
        {
            DEBUG_PRINT(COLOR_BMAGENTA "\nOPERATION\n" COLOR_RESET);
            sscanf(buffer + *position, "%s", operation);
            node->type.code_type  = OPERATION;
            node->type.name_type  = "operation";
            node->value.oper.code = getCodeOperation(operation); // TODO add processing errors and undetected operations
            DEBUG_PRINT("code operation = %d\n", node->value.oper.code);
            node->value.oper.name = operation;
            DEBUG_PRINT("name operation = %s\n", node->value.oper.name);
            (*position) += strlen(operation);
        }
    }
    else
    {
        node->value.number = number;
        node->type.code_type = NUMBER;
        node->type.name_type = "number";
        DEBUG_PRINT(COLOR_BMAGENTA "\nNUMBER\n" COLOR_RESET);
        DEBUG_PRINT("number = %lf\n", node->value.number);
        (*position) += strlen(number_str);
    }

    DEBUG_PRINT("buffer[*position] in %s after scanning = %c\n", __func__, buffer[*position]);

    skipSpacesInBuffer(buffer, position);

    DEBUG_PRINT("buffer[*position] in %s after scanning and after skipSpacesInBuffer = %c\n",
                    __func__, buffer[*position]);

    assert(buffer && position && node);

    return;
}


void skipSpacesInBuffer(char* buffer, size_t* position)
{
    assert(buffer && position);

    char ch = buffer[*position];
    if (ch != ' ') return;
    while ((ch = buffer[(*position)]) == ' ') (*position)++;

    printf("position in skipSpacesInBuffer after skipping = %c\n", buffer[(*position)]);
    assert(buffer && position);

    return;
}

bool checkNil (char* buffer, size_t* position)
{
    assert(buffer && position);

    char ch[4] = "";
    sscanf(buffer + *position, "%3s", ch);
    if (strcmp("nil", ch) == 0)
    {
        DEBUG_PRINT(COLOR_CYAN "nil detected!!!\n" COLOR_RESET);
        (*position) += 3;
        return true;
    }

    assert(buffer && position);

    return false;
}

bool checkVariable (char* buffer, size_t* position)
{
    assert(buffer);
    assert(position);

    return isalpha(buffer[*position]) && isspace(buffer[*position + 1]);
}

bool writeVariable(tree_t* tree, char* buffer, size_t* position, node_t* node)
{
    TREE_VERIFY;
    assert(buffer);
    assert(position);
    assert(node);

    for (int index = 0; index < NUMBER_VARIABLES; index++)
    {
        if (buffer[*position] == tree->variables[index].name)
        {
            node->type.code_type = VARIABLE;
            node->type.name_type = "variable";
            node->value.index    = index;
        }
    }

    TREE_VERIFY;
    assert(buffer);
    assert(position);
    assert(node);

    return (node->type.code_type == VARIABLE) ? true : false;
}
