#include <math.h>

#include "stack_calculator.h"
#include "tokenizer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#ifndef M_E
#define M_E 2.71828182845904523536028747135266250
#endif

static status push(dynamic_array *stack, const double number) {
    return dynarr_append(stack, &number);
}

static status pop(dynamic_array *stack, double *out_number) {
    if (stack->size == 0) {
        return STACK_UNDERFLOW;
    }
    dynarr_copy(stack, stack->size - 1, out_number);
    stack->size--;
    return OK;
}

static status pop_last(dynamic_array *stack, double *out_number) {
    const status st = pop(stack, out_number);
    if (st != OK) {
        return st;
    }
    if (stack->size > 0) {
        return STACK_NOT_EMPTY;
    }
    return OK;
}

static status negate(dynamic_array *stack) {
    double operand;
    const status st = pop(stack, &operand);
    if (st != OK) {
        return st;
    }
    return push(stack, -operand);
}

static status add(dynamic_array *stack) {
    double operand2;
    status st = pop(stack, &operand2);
    if (st != OK) {
        return st;
    }
    double operand1;
    st = pop(stack, &operand1);
    if (st != OK) {
        return st;
    }
    return push(stack, operand1 + operand2);
}

static status subtract(dynamic_array *stack) {
    double operand2;
    status st = pop(stack, &operand2);
    if (st != OK) {
        return st;
    }
    double operand1;
    st = pop(stack, &operand1);
    if (st != OK) {
        return st;
    }
    return push(stack, operand1 - operand2);
}

static status multiply(dynamic_array *stack) {
    double operand2;
    status st = pop(stack, &operand2);
    if (st != OK) {
        return st;
    }
    double operand1;
    st = pop(stack, &operand1);
    if (st != OK) {
        return st;
    }
    return push(stack, operand1 * operand2);
}

static status divide(dynamic_array *stack) {
    double operand2;
    status st = pop(stack, &operand2);
    if (st != OK) {
        return st;
    }
    double operand1;
    st = pop(stack, &operand1);
    if (st != OK) {
        return st;
    }
    return push(stack, operand1 / operand2);
}

static status modulus(dynamic_array *stack) {
    double operand2;
    status st = pop(stack, &operand2);
    if (st != OK) {
        return st;
    }
    double operand1;
    st = pop(stack, &operand1);
    if (st != OK) {
        return st;
    }
    return push(stack, fmod(operand1, operand2));
}

static status exponentiate(dynamic_array *stack) {
    double operand2;
    status st = pop(stack, &operand2);
    if (st != OK) {
        return st;
    }
    double operand1;
    st = pop(stack, &operand1);
    if (st != OK) {
        return st;
    }
    return push(stack, pow(operand1, operand2));
}

status stack_calculate(dynamic_array *tokens, double *out_number) {
    dynamic_array *stack;
    status st = dynarr_new(sizeof(double), 1, &stack);
    if (st != OK) {
        return st;
    }
    for (int q = 0; q < tokens->size; q++) {
        token token;
        dynarr_copy(tokens, q, &token);
        if (token.type == VALUE) {
            st = push(stack, token.value);
        } else if (token.type == OPERATOR) {
            switch (token.operator) {
                case ADDITION:
                    st = add(stack);
                    break;
                case SUBTRACTION:
                    st = subtract(stack);
                    break;
                case MULTIPLICATION:
                    st = multiply(stack);
                    break;
                case DIVISION:
                    st = divide(stack);
                    break;
                case MODULUS:
                    st = modulus(stack);
                    break;
                case NEGATION:
                    st = negate(stack);
                    break;
                case EXPONENTIATION:
                    st = exponentiate(stack);
                    break;
                default:
                    st = UNHANDLED_OPERATOR;
            }
        } else if (token.type == FUNCTION) {
            double n;
            st = pop(stack, &n);
            if (st == OK) {
                switch (token.function) {
                    case ABS:
                        st = push(stack, fabs(n));
                        break;
                    case ACOS:
                        st = push(stack, acos(n));
                        break;
                    case ASIN:
                        st = push(stack, asin(n));
                        break;
                    case ATAN:
                        st = push(stack, atan(n));
                        break;
                    case COS:
                        st = push(stack, cos(n));
                        break;
                    case COSH:
                        st = push(stack, cosh(n));
                        break;
                    case EXP:
                        st = push(stack, exp(n));
                        break;
                    case LN:
                        st = push(stack, log(n));
                        break;
                    case LOG:
                        st = push(stack, log10(n));
                        break;
                    case ROUND:
                        st = push(stack, round(n));
                        break;
                    case SIN:
                        st = push(stack, sin(n));
                        break;
                    case SINH:
                        st = push(stack, sinh(n));
                        break;
                    case SQRT:
                        st = push(stack, sqrt(n));
                        break;
                    case TAN:
                        st = push(stack, tan(n));
                        break;
                    case TANH:
                        st = push(stack, tanh(n));
                        break;
                    case TRUNC:
                        st = push(stack, trunc(n));
                        break;
                    case NEG:
                        st = push(stack, -n);
                        break;
                    default:
                        st = UNHANDLED_FUNCTION;
                }
            }
        } else if (token.type == CONSTANT) {
            switch (token.constant) {
                case E:
                    st = push(stack, M_E);
                    break;
                case PI:
                    st = push(stack, M_PI);
                    break;
                default:
                    return UNKNOWN_CONSTANT;
            }
        } else {
            st = UNHANDLED_TOKEN_TYPE;
        }
    }
    if (st == OK) {
        st = pop_last(stack, out_number);
    }
    dynarr_free(stack);
    return st;
}
