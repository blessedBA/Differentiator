#ifndef ARG_PARSER_DIFF_H
#define ARG_PARSER_DIFF_H

#include "Files.h"

#include <string.h>

struct FlagStorage
{
    char* input_file_name      = nullptr;
    char* dump_html_file_name  = nullptr;
    char* dump_latex_file_name = nullptr;
};

typedef void (*FlagFunc)(FlagStorage* storage, char* argv);

struct ConsoleFlag
{
    const char name[30];
    const char short_name[5];
    FlagFunc flag_func; //   без typedef-a: void (*flag_func)(FlagStorage*);
    bool isArg;
};

void getFlags(const int argc, char* argv[],
              const ConsoleFlag* list_flags,
              const size_t count_flags,
              FlagStorage* storage);

void inputFileEnable     (FlagStorage* storage, char* arg);
void dumpHtmlFileEnable  (FlagStorage* storage, char* arg);
void dumpLatexFileEnable (FlagStorage* storage, char* arg);

int openFiles(const FlagStorage* storage, FileStorage* file_storage);

#endif // ARG_PARSER_DIFF_H
