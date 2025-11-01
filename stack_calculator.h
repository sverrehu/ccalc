#ifndef CCALC_STACK_CALCULATOR_H
#define CCALC_STACK_CALCULATOR_H

#include "dynarr.h"
#include "status.h"

status stack_calculate(dynamic_array *tokens, double *out_number);

#endif
