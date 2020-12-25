/*
 * mm_alloc.h
 *
 * Exports a clone of the interface documented in "man 3 malloc".
 */

#pragma once

#ifndef _malloc_H_
#define _malloc_H_

#include <stdlib.h>

extern void* global_base; // How to ensure every process have unique global block and tail node?

struct meta_data
{
    size_t size;
    size_t free;
    struct meta_data *next;
    struct meta_data *prev;
};

typedef struct meta_data* meta;

void* mm_malloc(size_t size);
void* mm_realloc(void* ptr, size_t size);
void mm_free(void* ptr);
meta find_free_block(meta base, size_t size);
meta find_tail(meta base);
meta request_block(size_t size);
meta get_meta_block(void* position);
void* get_real_block(meta meta_position);

#endif
