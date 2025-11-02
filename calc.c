#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "status.h"
#include "stack_calculator.h"
#include "tokenizer.h"

static void help(void) {
    printf("%s\n",
           "calc -- a simple command-line calculator\n"
           "\n"
           "usage: calc [options] expression\n"
           "\n"
           "Options:\n"
           "\n"
           "  -h, --help  show this help\n"
           "  -r, --rpn   use \"Reverse Polish Notation\" (postfix)\n"
           "\n"
           "Operators: - * / % ^\n"
           "Functions: abs, acos, asin, atan, cos, cosh, exp, ln, log, neg,\n"
           "           round, sin, sinh, sqrt, tan, tanh, trunc\n"
           "Constants: e, pi\n"
           "\n"
           "For default infix expressions, function arguments must be given\n"
           "in parenthesis. For RPN, parenthesis are illegal.\n"
           "\n"
           "Examples:\n"
           "  calc \"sin(3.1415926)\"\n"
           "  calc \"(5 3) * 7\"\n"
           "  calc \"2^3\"\n"
           "  calc -r \"pi sin\"\n"
           "  calc -r \"5 3 7 *\"\n"
           "  calc -r \"2 3 ^\"\n"
           "  for Unix sh: A=`calc \"3+1\"`; B=`calc \"$A*4\"`\n");
}

static status add_to_string(char **expression, const char *str) {
    if (*expression == NULL) {
        *expression = malloc(strlen(str) + 1);
        if (*expression == NULL) {
            return OUT_OF_MEMORY;
        }
        **expression = '\0';
    } else {
        *expression = realloc(*expression, strlen(*expression) + strlen(str) + 1);
        if (*expression == NULL) {
            return OUT_OF_MEMORY;
        }
    }
    strcat(*expression, str);
    return OK;
}

static void print_error(const status st) {
    printf("error: %s\n", status_messages[st]);
}

static status read_from_stdin(char **s) {
    int c;
    char cs[] = {'\0', '\0'};
    while ((c = fgetc(stdin)) != EOF) {
        cs[0] = (char) c;
        const status st = add_to_string(s, cs);
        if (st != OK) {
            return st;
        }
    }
    return OK;
}

static status calculate(const char *expression, const int rpn, double *out) {
    status st = OK;
    dynamic_array *tokens = nullptr;
    st = tokenize(expression, &tokens);
    if (st != OK) {
        goto end;
    }
    if (!rpn) {
        dynamic_array *out_tokens = nullptr;
        st = convert_infix_to_postfix(tokens, &out_tokens);
        if (st != OK) {
            goto end;
        }
        dynarr_free(tokens);
        tokens = out_tokens;
    }
    st = stack_calculate(tokens, out);
end:
    dynarr_free(tokens);
    return st;
}

int main(const int argc, const char *argv[]) {
    status st = OK;
    int rpn = false;
    char *expression = nullptr;

    for (int q = 1; q < argc; q++) {
        const char *arg = argv[q];
        if (strcmp(arg, "-r") == 0 || strcmp(arg, "--rpn") == 0) {
            rpn = true;
        } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            help();
            return 0;
        } else {
            st = add_to_string(&expression, " ");
            if (st != OK) {
                goto end;
            }
            st = add_to_string(&expression, arg);
            if (st != OK) {
                goto end;
            }
        }
    }
    if (expression == NULL || expression[0] == '\0') {
        st = read_from_stdin(&expression);
        if (st != OK) {
            goto end;
        }
    }
    double result = NAN;
    st = calculate(expression, rpn, &result);
end:
    if (st == OK) {
        printf("%.15G\n", result);
    } else {
        print_error(st);
    }
    free(expression);
    return 0;
}
