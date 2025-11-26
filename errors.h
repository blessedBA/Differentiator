#ifndef ERRORS_H
#define ERRORS_H


typedef enum isError_t
{
    NO_ERRORS  = 0,
    HAVE_ERROR = 1
} isError_t;

typedef enum error_t
{
    HAVE_NO_ERRORS     = 0,
    ERR_FAIL_INIT_TREE = 1,
    ERR_INV_SIZE       = 2,
    ERR_INV_NAME_VAR   = 3
} error_t;

typedef enum code_error_t
{
    code_HAVE_NO_ERRORS     = 0 << 0,
    code_ERR_FAIL_INIT_TREE = 1 << 1,
    code_ERR_INV_SIZE       = 1 << 2,
    code_ERR_INV_NAME_VAR   = 1 << 3
} code_error_t;

typedef struct storageListErrors
{
    error_t error;
    code_error_t code_error;
    const char* description;
    bool isError;
} storageListErrors;

#endif // ERRORS_H
