#ifndef FILES_H
#define FILES_H

#include <stdio.h>

#define STANDARD_FILE_FOR_HTML_DUMP  "graphDumpDiff.html"
#define STANDARD_FILE_FOR_READ       "textDumpDifferentiator.txt"
#define STANDARD_FILE_FOR_LATEX_DUMP "latexDump/latexDump.tex"

typedef struct file_t
{
    FILE* pointer = nullptr;
    char* name    = nullptr;
} file_t;

typedef struct FileStorage
{
    file_t input_file;
    file_t log_file_html;
    file_t latex_file;
} FileStorage;

#endif // FILES_H
