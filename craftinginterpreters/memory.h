#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity)*2)

#define GROW_ARRAY(type, pointer, old_cnt, new_cnt)     \
  (type *)reallocate(pointer, sizeof(type) * (old_cnt), \
                     sizeof(type) * (new_cnt))
#define FREE_ARRAY(type, pointer, oldCnt) \
  reallocate(pointer, sizeof(type) * (oldCnt), 0)

#define ALLOCATE(type, count) \
  (type *)reallocate(NULL, 0, sizeof(type) * (count))

void *reallocate(void *pointer, size_t old_cnt, size_t new_cnt);

#endif
