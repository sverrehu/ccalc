#ifndef CCALC_DYNARR_H
#define CCALC_DYNARR_H

#include <stddef.h>
#include "status.h"

typedef struct {
    size_t size;
    size_t element_size;
    size_t pre_alloc_size;
    size_t capacity;
    void *elements;
} dynamic_array;

status dynarr_new(size_t element_size, size_t initial_capacity, dynamic_array **out);
void dynarr_free(dynamic_array *arr);
status dynarr_append(dynamic_array *arr, const void *element);
void dynarr_copy(dynamic_array *arr, size_t idx, void *dest);

#endif
