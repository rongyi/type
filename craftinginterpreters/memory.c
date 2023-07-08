#include "memory.h"
#include <stdlib.h>


void *reallocate(void *pointer, size_t old_sz, size_t new_sz) {
  if (new_sz == 0) {
    free(pointer);
    return NULL;
  }
  void *ret = realloc(pointer, new_sz);
  // no memory!
  if (ret == NULL) {
    exit(1);
  }

  return ret;
}
