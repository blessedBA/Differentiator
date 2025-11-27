#include "colors.h"
#include "mathExpParser.h"
#include "tree.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static int getRuleE(const char* math_exp, size_t* index);
static int getRuleN(const char* math_exp, size_t* index);
static int getRuleT(const char* math_exp, size_t* index);
static int getRuleP(const char* math_exp, size_t* index);

static void printSyntaxError(const char* math_exp, size_t* position);

int getRuleG(const char* math_exp, size_t* index)
{
    assert(math_exp);
    assert(index);

    int value = getRuleE(math_exp, index);
    if (math_exp[*index] != '$')
    {
        printSyntaxError(math_exp, index);
        return NULL;
    }

    (*index)++;

    assert(math_exp);
    assert(index);

    return value;
}

int getRuleE(const char* math_exp, size_t* index)
{
    assert(math_exp);
    assert(index);

    int value = getRuleT(math_exp, index);

    while (math_exp[*index] == '+' || math_exp[*index] == '-')
    {
        int operation = math_exp[*index];
        (*index)++;
        int value2 = getRuleT(math_exp, index);

        if (operation == '+') value += value2;
        else                  value -= value2;
    }

    assert(math_exp);
    assert(index);

    return value;
}

int getRuleN(const char* math_exp, size_t* index)
{
    assert(math_exp);
    assert(index);

    int value = 0;

    while ('0' <= math_exp[*index] && math_exp[*index] <= '9')
    {
        value = value * 10 + (math_exp[*index] - '0');
        (*index)++;
    }

    assert(math_exp);
    assert(index);

    return value;
}


int getRuleT(const char* math_exp, size_t* index)
{
    assert(math_exp);
    assert(index);

    int value = getRuleP(math_exp, index);

    while (math_exp[*index] == '*' || math_exp[*index] == '/')
    {
        int operation = math_exp[*index];
        (*index)++;
        int value2 = getRuleP(math_exp, index);

        if (operation == '*') value *= value2;
        else                  value /= value2;
    }

    assert(math_exp);
    assert(index);

    return value;
}

int getRuleP(const char* math_exp, size_t* index)
{
    assert(math_exp);
    assert(index);

    if (math_exp[*index] == '(')
    {
        (*index)++;
        int value = getRuleE(math_exp, index);
        if (math_exp[*index] != ')')
        {
            printSyntaxError(math_exp, index);
            return NULL;
        }
        (*index)++;
        return value;
    }

    assert(math_exp);
    assert(index);

    return getRuleN(math_exp, index);
}

void printSyntaxError(const char* math_exp, size_t* position)
{
    assert(math_exp);
    assert(position);

    fprintf(stderr, COLOR_BRED "ERROR: invalid symbol\n" COLOR_RESET);
    fprintf(stderr, COLOR_BGREEN "\t\"%s\"\n\t", math_exp);

    size_t index = 0;
    for ( ; index < *position; index++) fprintf(stderr, "~");

    fprintf(stderr, "^");

    for ( ; index < strlen(math_exp); index++) fprintf(stderr, "~");

    fprintf(stderr, "\n" COLOR_RESET);

    assert(math_exp);
    assert(position);

    return;
}
