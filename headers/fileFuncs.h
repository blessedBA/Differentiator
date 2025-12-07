#ifndef FILE_FUNCS_H
#define FILE_FUNCS_H

#include "Files.h"
#include "tree.h"
#include "argParserDiff.h"

#include <stdio.h>

#define MAX_LENGTH_FILENAME 50

typedef enum class_file_t
{
    DUMP_FILE    = 1,
    READING_FILE = 2
} class_file_t;

char*   creatBuffer  (const char* filename);
void    startWriting (tree_t* tree);
node_t* startReading (tree_t* tree, FileStorage* file_storage);
bool    clearFile    (const char* file_name);
FILE*   getFilename  (class_file_t class_file);

int openFiles   (FlagStorage* storage, FileStorage* file_storage);
void closeFiles (FileStorage* file_storage);

#endif // FILE_FUNCS_H
