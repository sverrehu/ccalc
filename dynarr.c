#include "dynarr.h"

#include <stdlib.h>
#include <string.h>

static status dynarr_pre_alloc(dynamic_array *arr) {
    if (arr->capacity == 0) {
        arr->capacity = arr->pre_alloc_size;
        arr->elements = malloc(arr->element_size * arr->capacity);
    } else {
        arr->capacity += arr->pre_alloc_size;
        void *new_ptr = realloc(arr->elements, arr->element_size * arr->capacity);
        if (new_ptr == NULL) {
            free(arr->elements);
        }
        arr->elements = new_ptr;
    }
    if (arr->elements == NULL) {
        return OUT_OF_MEMORY;
    }
    return OK;
}

status dynarr_new(const size_t element_size, const size_t initial_capacity, dynamic_array **out) {
    *out = malloc(sizeof(dynamic_array));
    if (*out == NULL) {
        return OUT_OF_MEMORY;
    }
    (*out)->size = 0;
    (*out)->element_size = element_size;
    (*out)->pre_alloc_size = initial_capacity;
    (*out)->capacity = 0;
    const status st = dynarr_pre_alloc(*out);
    if (st != OK) {
        free(*out);
        *out = NULL;
        return st;
    }
    return OK;
}

void dynarr_free(dynamic_array *arr) {
    free(arr->elements);
    free(arr);
}

status dynarr_append(dynamic_array *arr, const void *element) {
    if (arr->size >= arr->capacity) {
        const status st = dynarr_pre_alloc(arr);
        if (st != OK) {
            return st;
        }
    }
    memcpy(arr->elements + arr->size++ * arr->element_size, element, arr->element_size);
    return OK;
}

void dynarr_copy(dynamic_array *arr, const size_t idx, void *dest) {
    memcpy(dest, arr->elements + idx * arr->element_size, arr->element_size);
}