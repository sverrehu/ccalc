#include "tokenizer.h"

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *s;
    int idx;
    size_t max_identifier_length;
} tokenizer_state;

static char curr_char(const tokenizer_state *state) {
    if (state->idx >= strlen(state->s)) {
        return '\0';
    }
    return state->s[state->idx];
}

static char next_char(tokenizer_state *state) {
    state->idx++;
    if (state->idx >= strlen(state->s)) {
        return '\0';
    }
    return state->s[state->idx];
}

static void skip_whitespace(tokenizer_state *state) {
    for (;;) {
        const char c = curr_char(state);
        if (!isspace(c)) {
            break;
        }
        next_char(state);
    }
}

static status scan_number(tokenizer_state *state, double *out_number) {
    double number = 0.0;
    double divider = 0.1;
    bool dot_seen = false;
    for (;;) {
        char c = curr_char(state);
        if (c == '\0') {
            break;
        }
        if (c == '.') {
            dot_seen = true;
        } else if (c == 'e' || c == 'E') {
            c = next_char(state);
            if (c == '\0') {
                return INVALID_EXPONENT;
            }
            double sign = 1.0;
            if (c == '-') {
                sign = -1.0;
                c = next_char(state);
                if (c == '\0') {
                    return INVALID_EXPONENT;
                }
            } else if (c == '+') {
                c = next_char(state);
                if (c == '\0') {
                    return INVALID_EXPONENT;
                }
            }
            double exp;
            const status st = scan_number(state, &exp);
            if (st != OK) {
                return st;
            }
            number *= pow(10.0, sign * exp);
            break;
        } else if (c >= '0' && c <= '9') {
            const int digit = c - '0';
            if (dot_seen) {
                number += digit * divider;
                divider /= 10.0;
            } else {
                number = number * 10.0 + digit;
            }
        } else {
            break;
        }
        next_char(state);
    }
    *out_number = number;
    return OK;
}

static void scan_identifier(tokenizer_state *state, char *identifier) {
    int idx = 0;
    char c = curr_char(state);
    while (c != '\0' && (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')) {
        identifier[idx++] = c;
        c = next_char(state);
    }
    identifier[idx] = '\0';
}

static status to_function_or_constant_token(const char *identifier, token *token) {
    if (strcasecmp(identifier, "ABS") == 0) {
        token->type = FUNCTION;
        token->function = ABS;
        return OK;
    }
    if (strcasecmp(identifier, "ACOS") == 0) {
        token->type = FUNCTION;
        token->function = ACOS;
        return OK;
    }
    if (strcasecmp(identifier, "ASIN") == 0) {
        token->type = FUNCTION;
        token->function = ASIN;
        return OK;
    }
    if (strcasecmp(identifier, "ATAN") == 0) {
        token->type = FUNCTION;
        token->function = ATAN;
        return OK;
    }
    if (strcasecmp(identifier, "COS") == 0) {
        token->type = FUNCTION;
        token->function = COS;
        return OK;
    }
    if (strcasecmp(identifier, "COSH") == 0) {
        token->type = FUNCTION;
        token->function = COSH;
        return OK;
    }
    if (strcasecmp(identifier, "EXP") == 0) {
        token->type = FUNCTION;
        token->function = EXP;
        return OK;
    }
    if (strcasecmp(identifier, "LN") == 0) {
        token->type = FUNCTION;
        token->function = LN;
        return OK;
    }
    if (strcasecmp(identifier, "LOG") == 0) {
        token->type = FUNCTION;
        token->function = LOG;
        return OK;
    }
    if (strcasecmp(identifier, "ROUND") == 0) {
        token->type = FUNCTION;
        token->function = ROUND;
        return OK;
    }
    if (strcasecmp(identifier, "SIN") == 0) {
        token->type = FUNCTION;
        token->function = SIN;
        return OK;
    }
    if (strcasecmp(identifier, "SINH") == 0) {
        token->type = FUNCTION;
        token->function = SINH;
        return OK;
    }
    if (strcasecmp(identifier, "SQRT") == 0) {
        token->type = FUNCTION;
        token->function = SQRT;
        return OK;
    }
    if (strcasecmp(identifier, "TAN") == 0) {
        token->type = FUNCTION;
        token->function = TAN;
        return OK;
    }
    if (strcasecmp(identifier, "TANH") == 0) {
        token->type = FUNCTION;
        token->function = TANH;
        return OK;
    }
    if (strcasecmp(identifier, "TRUNC") == 0) {
        token->type = FUNCTION;
        token->function = TRUNC;
        return OK;
    }
    if (strcasecmp(identifier, "NEG") == 0) {
        token->type = FUNCTION;
        token->function = NEG;
        return OK;
    }
    if (strcasecmp(identifier, "E") == 0) {
        token->type = CONSTANT;
        token->constant = E;
        return OK;
    }
    if (strcasecmp(identifier, "PI") == 0) {
        token->type = CONSTANT;
        token->constant = PI;
        return OK;
    }
    return UNKNOWN_FUNCTION_OR_CONSTANT;
}

static status next_token(tokenizer_state *state, token *out_token) {
    status st;
    skip_whitespace(state);
    const char c = curr_char(state);
    if (c == '\0') {
        out_token->type = END;
        return OK;
    }
    if (c == '.' || c >= '0' && c <= '9') {
        double number;
        st = scan_number(state, &number);
        if (st != OK) {
            return st;
        }
        out_token->type = VALUE;
        out_token->value = number;
        return OK;
    }
    if (c == '_' || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
        char *identifier = malloc(state->max_identifier_length + 1);
        if (identifier == NULL) {
            return OUT_OF_MEMORY;
        }
        scan_identifier(state, identifier);
        st = to_function_or_constant_token(identifier, out_token);
        free(identifier);
        if (st != OK) {
            return st;
        }
        return OK;
    }
    next_char(state);
    out_token->type = OPERATOR;
    switch (c) {
        case '+':
            out_token->operator = ADDITION;
            return OK;
        case '-':
            out_token->operator = SUBTRACTION;
            return OK;
        case '*':
            out_token->operator = MULTIPLICATION;
            return OK;
        case '/':
            out_token->operator = DIVISION;
            return OK;
        case '%':
            out_token->operator = MODULUS;
            return OK;
        case '^':
            out_token->operator = EXPONENTIATION;
            return OK;
        case '(':
            out_token->operator = LEFT_PAREN;
            return OK;
        case ')':
            out_token->operator = RIGHT_PAREN;
            return OK;
        case ',':
            out_token->operator = COMMA;
            return OK;
        default:
            return UNEXPECTED_CHARACTER;
    }
}

status tokenize(const char *expression, dynamic_array **out_token_array) {
    status st = dynarr_new(sizeof(token), 10, out_token_array);
    if (st != OK) {
        return st;
    }
    tokenizer_state state;
    state.s = expression;
    state.idx = 0;
    state.max_identifier_length = strlen(expression);
    for (;;) {
        token token;
        st = next_token(&state, &token);
        if (st != OK) {
            return st;
        }
        if (token.type == END) {
            break;
        }
        st = dynarr_append(*out_token_array, &token);
        if (st != OK) {
            return st;
        }
    }
    return OK;
}
