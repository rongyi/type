#include "memory.h"

#include <stdio.h>
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
      /*printf("free string: %s\n", cur->chars_);*/
      FREE_ARRAY(char, cur->chars_, cur->length_ + 1);
      FREE(ObjString, o);
      break;
    }
    case OBJ_FUNCTION: {
      ObjFunction *f = (ObjFunction *)o;
      freeChunk(&f->chunk_);
      FREE(ObjFunction, o);
      break;
    }
    case OBJ_NATIVE: {
      FREE(ObjNative, o);
      break;
    }
    case OBJ_CLOSURE: {
      // free upvalues array
      ObjClosure *closure = (ObjClosure *)o;
      FREE_ARRAY(ObjUpvalue *, closure->upvalues_, closure->upvalue_cnt_);

      FREE(ObjClosure, o);
      break;
    }
    case OBJ_UPVALUE: {
      FREE(ObjUpvalue, o);
      break;
    }
    default: {
      /*printf("what the fuck! %d\n", o->type_);*/
    }
  }
}

void freeObjects() {
  Obj *cur = vm.objects_head_;
  while (cur != NULL) {
    Obj *next = cur->next_;
    /*printf("free: %p\n", cur);*/
    freeObject(cur);
    cur = next;
  }
}
