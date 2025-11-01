#ifndef CCALC_PARSER_H
#define CCALC_PARSER_H

#include "dynarr.h"
#include "status.h"

status convert_infix_to_postfix(dynamic_array *in_tokens, dynamic_array **out_tokens);

#endif
