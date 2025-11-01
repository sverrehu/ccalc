#include "parser.h"
#include "tokenizer.h"

#include <stdbool.h>

typedef struct {
    dynamic_array *in_tokens;
    int idx;
    token token;
    dynamic_array *out_tokens;
} parser_state;

static status parse_expression(parser_state *state);

static bool eof(const parser_state *state) {
    return state->token.type == END;
}

static void next(parser_state *state) {
    if (state->idx >= state->in_tokens->size) {
        state->token.type = END;
    } else {
        dynarr_copy(state->in_tokens, state->idx, &state->token);
        state->idx++;
    }
}

static status next_check_eof(parser_state *state) {
    next(state);
    if (eof(state)) {
        return UNEXPECTED_END_OF_INPUT;
    }
    return OK;
}

static bool is_operator_match(const parser_state *state, const operator_token ot) {
    if (eof(state)) {
        return false;
    }
    return state->token.type == OPERATOR && state->token.operator == ot;
}

static status add_out_token(const parser_state *state, const token token) {
    return dynarr_append(state->out_tokens, &token);
}

static status parse_function_expression(parser_state *state) {
    const function_token ft = state->token.function;
    next(state);
    if (!is_operator_match(state, LEFT_PAREN)) {
        return MISSING_LEFT_PARENTHESIS;
    }
    status st = next_check_eof(state);
    if (st != OK) {
        return st;
    }
    while (!is_operator_match(state, RIGHT_PAREN)) {
        st = parse_expression(state);
        if (st != OK) {
            return st;
        }
        if (is_operator_match(state, COMMA)) {
            st = next_check_eof(state);
            if (st != OK) {
                return st;
            }
            if (is_operator_match(state, RIGHT_PAREN)) {
                return MISSING_FUNCTION_ARGUMENT;
            }
        }
    }
    next(state);
    token token;
    token.type = FUNCTION;
    token.function = ft;
    add_out_token(state, token);
    return OK;
}

static status parse_primary_expression(parser_state *state) {
    status st;
    if (state->token.type == VALUE || state->token.type == CONSTANT) {
        st = add_out_token(state, state->token);
        if (st != OK) {
            return st;
        }
        next(state);
        return OK;
    }
    if (state->token.type == FUNCTION) {
        return parse_function_expression(state);
    }
    if (is_operator_match(state, LEFT_PAREN)) {
        st = next_check_eof(state);
        if (st != OK) {
            return st;
        }
        st = parse_expression(state);
        if (st != OK) {
            return st;
        }
        if (is_operator_match(state, RIGHT_PAREN)) {
            return UNMATCHED_PARENTHESIS;
        }
        next(state);
        return OK;
    }
    return UNEXPECTED_OPERATOR;
}

static status parse_unary_expression(parser_state *state) {
    status st;
    bool negate = false;
    if (is_operator_match(state, SUBTRACTION)) {
        negate = true;
        st = next_check_eof(state);
        if (st != OK) {
            return st;
        }
    } else if (is_operator_match(state, ADDITION)) {
        st = next_check_eof(state);
        if (st != OK) {
            return st;
        }
    }
    st = parse_primary_expression(state);
    if (st != OK) {
        return st;
    }
    if (negate) {
        token ot;
        ot.type = OPERATOR;
        ot.operator = NEGATION;
        st = add_out_token(state, ot);
        if (st != OK) {
            return st;
        }
    }
    return OK;
}

static status parse_exponential_expression(parser_state *state) {
    status st = parse_unary_expression(state);
    if (st != OK) {
        return st;
    }
    int count = 0;
    while (is_operator_match(state, EXPONENTIATION)) {
        st = next_check_eof(state);
        if (st != OK) {
            return st;
        }
        st = parse_unary_expression(state);
        if (st != OK) {
            return st;
        }
        ++count;
    }
    for (int q = 0; q < count; q++) {
        token ot;
        ot.type = OPERATOR;
        ot.operator = EXPONENTIATION;
        st = add_out_token(state, ot);
        if (st != OK) {
            return st;
        }
    }
    return OK;
}

static status parse_multiplicative_expression(parser_state *state) {
    status st = parse_exponential_expression(state);
    if (st != OK) {
        return st;
    }
    while (is_operator_match(state, MULTIPLICATION) || is_operator_match(state, DIVISION) || is_operator_match(state, MODULUS)) {
        token ot;
        ot.type = OPERATOR;
        ot.operator = state->token.operator;
        st = next_check_eof(state);
        if (st != OK) {
            return st;
        }
        st = parse_exponential_expression(state);
        if (st != OK) {
            return st;
        }
        st = add_out_token(state, ot);
        if (st != OK) {
            return st;
        }
    }
    return OK;
}

static status parse_additive_expression(parser_state *state) {
    status st = parse_multiplicative_expression(state);
    if (st != OK) {
        return st;
    }
    while (is_operator_match(state, ADDITION) || is_operator_match(state, SUBTRACTION)) {
        token ot;
        ot.type = OPERATOR;
        ot.operator = state->token.operator;
        st = next_check_eof(state);
        if (st != OK) {
            return st;
        }
        st = parse_multiplicative_expression(state);
        if (st != OK) {
            return st;
        }
        st = add_out_token(state, ot);
        if (st != OK) {
            return st;
        }
    }
    return OK;
}

static status parse_expression(parser_state *state) {
    return parse_additive_expression(state);
}

status convert_infix_to_postfix(dynamic_array *in_tokens, dynamic_array **out_tokens) {
    status st = dynarr_new(sizeof(token), 10, out_tokens);
    if (st != OK) {
        return st;
    }
    parser_state state;
    state.in_tokens = in_tokens;
    state.idx = 0;
    state.token.type = END;
    state.out_tokens = *out_tokens;
    st = next_check_eof(&state);
    if (st != OK) {
        goto end;
    }
    st = parse_expression(&state);
    if (st != OK) {
        goto end;
    }
    if (!eof(&state)) {
        st = UNEXPECTED_TEXT_AT_END;
    }
end:
    if (st != OK) {
        dynarr_free(*out_tokens);
    }
    return st;
}
