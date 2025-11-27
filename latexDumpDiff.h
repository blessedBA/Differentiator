#ifndef LATEX_DUMP_DIFF_H
#define LATEX_DUMP_DIFF_H

#include "tree.h"

#include <stdio.h>

#define LENGTH_BASIC_OPER 1

void latexDump (FILE* latex_file, const tree_t* tree, const char* file_name, const char* func_name, int line,
                int count_log_files, const char* reason, ...);


#endif // LATEX_DUMP_DIFF_H
