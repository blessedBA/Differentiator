#include "colors.h"
#include "errors.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void grow_error_log (errorLog* log);
static void grow_contexts (storageErrors* errors);
static inline void updateErrorMask (errorLog* log, code_error_t code);

errorLog global_error_log;

void grow_error_log (errorLog* log)
{
    assert(log);

    if (log->size < log->capacity) return;
    log->capacity = (log->capacity == 0 ? 8 : log->capacity * 2);
    storageErrors* new_data = (storageErrors*)realloc(log->entries, log->capacity * sizeof(storageErrors));
    if (!new_data)
    {
        fprintf(stderr, "FATAL: Cannot grow error log.\n");
        return;
    }
    log->entries = new_data;

    assert(log);

    return;
}

void grow_contexts (storageErrors* errors)
{
    assert(errors);

    errors->ctx_capacity = (errors->ctx_capacity == 0) ? 2 : errors->ctx_capacity * 2;

    errorContext* new_context = (errorContext*)realloc(errors->contexts, errors->ctx_capacity * sizeof(errorContext));
    if (!new_context)
    {
        fprintf(stderr, COLOR_BRED "FATAL ERROR: Cannot grow error context array.\n" COLOR_RESET);
        exit(1);
    }
    errors->contexts = new_context;

    assert(errors);

    return;
}

void initErrorLog (errorLog* log)
{
    assert(log);

    log->log_file = nullptr;
    log->global_mask_error = 0;
    log->entries = nullptr;
    log->size = 0;
    log->capacity = 0;

    assert(log);

    return;
}

void setErrorLogFile (errorLog* log, FILE** log_file, const char* file_name)
{
    assert(log);
    assert(log_file);

    if (!file_name)
    {
        log->log_file = stderr;
        log->log_filename = "stderr";
    }
    else
    {
        *log_file = fopen(file_name, "w");
        if (*log_file == nullptr)
        {
            fprintf(stderr, COLOR_BRED "FATAL ERROR: failed to open text log file!\n" COLOR_RESET);
            exit(1);
        }
        log->log_file = *log_file;
        log->log_filename = file_name;
    }

    assert(log);
    assert(log_file);

    return;
}

void freeErrorLog (errorLog* log)
{
    assert(log);

    for (size_t i = 0; i < log->size; i++) free(log->entries[i].contexts);
    free(log->entries);
    log->entries = nullptr;
    log->size = log->capacity = 0;

    assert(log);

    return;
}

void updateErrorMask (errorLog* log, code_error_t code)
{
    assert(log);

    log->global_mask_error |= code;

    assert(log);

    return;
}

storageErrors* addError (errorLog* log, error_t code, code_error_t byte_code_error, const char* desc)
{
    assert(log);
    assert(desc);

    grow_error_log(log);

    storageErrors *err = &log->entries[log->size++];
    err->code = code;
    err->has_error = 1;

    strncpy(err->description, desc, sizeof(err->description) - 1);
    err->description[sizeof(err->description) - 1] = '\0';

    err->contexts = nullptr;
    err->ctx_size = 0;
    err->ctx_capacity = 0;

    updateErrorMask(log, byte_code_error);

    assert(log);
    assert(desc);

    return err;
}

void addErrorContext (errorLog* log, const char* file_name, const char* func_name, int line)
{
    assert(log);
    assert(log->entries);
    assert(file_name);
    assert(func_name);
    assert(log->size > 0); // there must be at least one error

    storageErrors* errors = &log->entries[log->size - 1]; // use last added error

    if (errors->ctx_size >= errors->ctx_capacity) grow_contexts(errors);

    errorContext *curr_context = &errors->contexts[errors->ctx_size++];
    curr_context->file      = file_name;
    curr_context->function  = func_name;
    curr_context->line      = line;
    curr_context->timestamp = time(NULL);

    char time_buf[32];
    struct tm* tm_info = localtime(&curr_context->timestamp);
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
    strncpy(curr_context->time_date, time_buf, sizeof(curr_context->time_date));
    curr_context->time_date[sizeof(curr_context->time_date) - 1] = '\0';

    FILE* err_log_file = log->log_file ? log->log_file : stderr;

    fprintf(err_log_file, COLOR_BRED "ERROR CODE %d | %s | %s:%d in %s() at %s\n" COLOR_RESET,
                errors->code, errors->description, file_name,
                line, func_name, curr_context->time_date);

    assert(errors);
    assert(file_name);
    assert(func_name);

    return;
}
// ...existing code...
void printErrors (const errorLog* log)
{
    assert(log);

    FILE* log_file = log->log_file ? log->log_file : stderr;

    fprintf(log_file, "ERROR CODE = %llu\n", log->global_mask_error);

    for (size_t index = 0; index < log->size; index++)
    {
        const storageErrors* entry = &log->entries[index];
        if (entry->has_error)
        {
            fprintf(log_file, "\n=== ERROR %zu ===\n", index);
            fprintf(log_file, "Code: %d\nDescription: %s\nContexts: %zu\n",
                        entry->code, entry->description, entry->ctx_size);

            for (size_t sec_index = 0; sec_index < entry->ctx_size; sec_index++)
            {
                const errorContext* curr_context = &entry->contexts[sec_index];
                fprintf(log_file,
                    "  [%zu] %s:%d in %s() at %s\n",
                    sec_index, curr_context->file, curr_context->line,
                    curr_context->function, curr_context->time_date);
            }
        }
    }

    assert(log);

    return;
}
