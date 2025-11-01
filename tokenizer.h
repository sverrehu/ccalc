#ifndef CCALC_TOKENIZER_H
#define CCALC_TOKENIZER_H

#include "dynarr.h"
#include "status.h"

typedef enum {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    MODULUS,
    NEGATION,
    EXPONENTIATION,
    LEFT_PAREN,
    RIGHT_PAREN,
    COMMA,
} operator_token;

typedef enum {
    ABS,
    ACOS,
    ASIN,
    ATAN,
    COS,
    COSH,
    EXP,
    LN,
    LOG,
    ROUND,
    SIN,
    SINH,
    SQRT,
    TAN,
    TANH,
    TRUNC,
    NEG,
} function_token;

typedef enum {
    E,
    PI,
} constant_token;

typedef enum {
    END, /* for internal tokenizer use only, will not be returned. */
    OPERATOR,
    FUNCTION,
    CONSTANT,
    VALUE,
} token_type;

typedef struct {
    token_type type;

    union {
        operator_token operator;
        function_token function;
        constant_token constant;
        double value;
    };
} token;

status tokenize(const char *expression, dynamic_array **out_token_array);

#endif
