#include "argParserDiff.h"
#include "colors.h"
#include "fileFuncs.h"
#include "Files.h"
#include "tree.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void getFlags(const int argc, char* argv[],
              const ConsoleFlag* list_flags,
              const size_t count_flags,
              FlagStorage* storage)
{
    assert(storage);
    assert(&list_flags);

    for (int i = 1; i < argc; i++)
    {
        for (size_t j = 0; j < count_flags; j++)
        {
            if (strcmp(argv[i], list_flags[j].name) == 0 ||
                strcmp(argv[i], list_flags[j].short_name) == 0)
            {
                if (list_flags[j].isArg)
                {
                    if (i + 1 >= argc)
                    {
                        fprintf(stderr, COLOR_BRED "invalid argument!!!\n" COLOR_RESET);
                        return;
                    }
                     list_flags[j].flag_func(storage, argv[i+1]);
                }
                else
                {
                    list_flags[j].flag_func(storage, NULL);
                }
            }
        }
    }

    assert(storage);
    assert(&list_flags);

    return;
}

void inputFileEnable(FlagStorage* storage, char* arg)
{
    assert(storage);

    storage->input_file_name = arg;

    assert(storage);
}

void dumpHtmlFileEnable(FlagStorage* storage, char* arg)
{
    assert(storage);

    storage->dump_html_file_name = arg;

    assert(storage);
}

void dumpLatexFileEnable(FlagStorage* storage, char* arg)
{
    assert(storage);

    storage->dump_latex_file_name = arg;

    assert(storage);
}

int openFiles(const FlagStorage* storage, FileStorage* file_storage)
{
    assert(storage);

    if (storage->input_file_name)
    {
        file_storage->input_file.pointer = fopen(storage->input_file_name, "r");
        file_storage->input_file.name    = storage->input_file_name;
    }
    else
    {
        file_storage->input_file.pointer = fopen(STANDARD_FILE_FOR_READ, "r");
        file_storage->input_file.name    = STANDARD_FILE_FOR_READ;
    }

    if (!(file_storage->input_file.pointer))
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed to open file for reading!\n" COLOR_RESET);
        return -1;
    }

    if (storage->dump_html_file_name)
    {
        file_storage->log_file_html.pointer = fopen(storage->dump_html_file_name, "r");
        file_storage->log_file_html.name    = storage->dump_html_file_name;
    }
    else
    {
        file_storage->log_file_html.pointer = fopen(STANDARD_FILE_FOR_HTML_DUMP, "r");
        file_storage->log_file_html.name    = STANDARD_FILE_FOR_HTML_DUMP;
    }

    if (!(file_storage->log_file_html.pointer))
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed to open file for html dump!\n" COLOR_RESET);
        return -1;
    }

    if (storage->dump_latex_file_name)
    {
        file_storage->latex_file.pointer = fopen(storage->dump_latex_file_name, "r");
        file_storage->latex_file.name    = storage->dump_latex_file_name;
    }
    else
    {
        file_storage->latex_file.pointer = fopen(STANDARD_FILE_FOR_LATEX_DUMP, "r");
        file_storage->latex_file.name    = STANDARD_FILE_FOR_LATEX_DUMP;
    }

    if (!(file_storage->latex_file.pointer))
    {
        fprintf(stderr, COLOR_BRED "ERROR: failed to open file for latex dump!\n" COLOR_RESET);
        return -1;
    }

    return 0;
}
