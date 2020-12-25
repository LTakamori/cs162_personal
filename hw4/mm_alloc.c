/*
 * mm_alloc.c
 */

#include "mm_alloc.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void* global_base = NULL;

meta find_free_block(meta base, size_t size)
{
  meta res = NULL;
  if (base == NULL) {
    return res;
  } else {
    while(base != NULL) {
      if (base->size >= size) {
        res = base;
        break;
      } else {
        base = base->next;
      }
    }
  }
  return res + 1;
}

meta find_tail(meta base) {
  while (base != NULL) {
    base = base->next;
  }
  return base;
}

meta request_block(size_t size) {
  meta tail = find_tail(global_base);
  meta block = sbrk(0);
  sbrk(size + sizeof(struct meta_data));
  tail->next = block;
  block->next = NULL;
  block->size = size;
  block->free = 0;
  block->prev = tail;
  return block + 1;
}

void* mm_malloc(size_t size)
{
  //TODO: Implement malloc
  void* res = find_free_block(global_base, size);
  if (res != NULL) return res;
  else {
    res = request_block(size);
    return res;
  }
  return NULL;
}

meta get_meta_block(void* position) {
  return (meta)position - 1;
}

void* get_real_block(meta meta_position) {
  return meta_position + 1;
}

void* mm_realloc(void* ptr, size_t size)
{
  //TODO: Implement realloc
  void* dest = mm_malloc(size);
  void* src = get_meta_block(ptr);
  memcpy(dest, src, size + sizeof(struct meta_data));
  mm_free(ptr);
  return NULL;
}

void mm_free(void* ptr)
{
  //TODO: Implement free
  meta meta_position = get_meta_block(ptr);
  meta_position -> free = 1;
}
