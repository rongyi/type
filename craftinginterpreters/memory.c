#include "memory.h"

#include <stdlib.h>

#include "object.h"
#include "vm.h"

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

static void freeObject(Obj *o) {
  switch (o->type_) {
    case OBJ_STRING: {
      ObjString *cur = (ObjString *)o;
      FREE_ARRAY(char, cur->chars_, cur->length_ + 1);
      FREE(ObjString, o);
      break;
    }
  }
}

void freeObjects() {
  Obj *o = vm.objects_head_;
  while (o != NULL) {
    Obj *next = o->next_;
    freeObject(o);
    o = next;
  }
}
