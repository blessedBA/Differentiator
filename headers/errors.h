#ifndef ERRORS_H
#define ERRORS_H

#include <time.h>

#include <stdio.h>

#define LOG_ERROR(log, code, byte_code_error, desc) addError((log), (code), (byte_code_error), (desc))
#define ADD_CONTEXT(log) addErrorContext((log), __FILE__, __func__, __LINE__)

typedef enum isError_t
{
    NO_ERRORS  = 0,
    HAVE_ERROR = 1
} isError_t;

typedef enum error_t
{
    HAVE_NO_ERRORS                      = 0,
    ERR_FAIL_INIT_TREE                  = 1,
    ERR_INV_SIZE                        = 2,
    ERR_INV_NAME_VAR                    = 3,
    ERR_FAIL_INIT_DIFF_TREE             = 4,
    ERR_FAIL_INIT_ROOT_DIFF_TREE        = 5,
    ERR_INVALID_CODE_OPER               = 6,
    ERR_INVALID_TYPE_NODE               = 7,
    ERR_INVALID_END_MATH_EXP            = 8,
    ERR_FAIL_INIT_NODE                  = 9,
    ERR_FAIL_INIT_NAME_OPER             = 10,
    ERR_NO_MATCH_PARENT_AND_CHILD       = 11,
    ERR_DIVISION_BY_ZERO                = 12,
    UNEXPECTED_ERROR                    = 13,
    ERR_INVALID_SIDE_NODE               = 14,
    ERR_INVALID_CHILD_OF_NODE           = 15,
    ERR_UNEXPECTED_ENUM_VALUE           = 16

} error_t;

typedef enum code_error_t // TODO delete this enum, it is useless
{
    code_HAVE_NO_ERRORS                 = 0 << 0,
    code_ERR_FAIL_INIT_TREE             = 1 << 1,
    code_ERR_INV_SIZE                   = 1 << 2,
    code_ERR_INV_NAME_VAR               = 1 << 3,
    code_ERR_FAIL_INIT_DIFF_TREE        = 1 << 4,
    code_ERR_FAIL_INIT_ROOT_DIFF_TREE   = 1 << 5,
    code_ERR_INVALID_CODE_OPER          = 1 << 6,
    code_ERR_INVALID_TYPE_NODE          = 1 << 7,
    code_ERR_INVALID_END_MATH_EXP       = 1 << 8,
    code_ERR_FAIL_INIT_NODE             = 1 << 9,
    code_ERR_FAIL_INIT_NAME_OPER        = 1 << 10,
    code_ERR_NO_MATCH_PARENT_AND_CHILD  = 1 << 11,
    code_ERR_DIVISION_BY_ZERO           = 1 << 12,
    code_UNEXPECTED_ERROR               = 1 << 13,
    code_ERR_INVALID_SIDE_NODE          = 1 << 14,
    code_ERR_INVALID_CHILD_OF_NODE      = 1 << 15,
    code_ERR_UNEXPECTED_ENUM_VALUE      = 1 << 16
} code_error_t;

typedef struct errorContext
{
    const char* file;
    const char* function;
    int line;
    time_t timestamp;
    char time_date[32];
} errorContext;

typedef struct storageErrors
{
    error_t code;
    code_error_t byte_code_error;
    char description[256];
    bool has_error;

    errorContext* contexts;
    size_t ctx_size;
    size_t ctx_capacity;
} storageErrors;

typedef struct errorLog
{
    storageErrors* entries;
    unsigned long long global_mask_error;
    FILE* log_file;
    const char* log_filename;
    size_t size;
    size_t capacity;
} errorLog;

storageErrors* addError (errorLog* log, error_t code, code_error_t byte_code_error, const char* desc);
void initErrorLog (errorLog* log);
void setErrorLogFile (errorLog* log, FILE** log_file, const char* file_name = nullptr);
void addErrorContext (errorLog* global_error_log, const char* file_name, const char* func_name, int line);
void printErrors (const errorLog* log);
void freeErrorLog (errorLog* log);

extern errorLog global_error_log;

#endif // ERRORS_H
