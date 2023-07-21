#include "object.h"

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
  (type *)allocateObjectWithType(sizeof(type), objectType)

static Obj *allocateObjectWithType(size_t size, ObjType type) {
  // only size, no type
  Obj *ret = (Obj *)reallocate(NULL, 0, size);
  ret->type_ = type;
  // insert in header next
  ret->next_ = vm.objects_head_;

  vm.objects_head_ = ret;

  return ret;
}

static ObjString *allocateString(char *chars, int len) {
  ObjString *ret = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  ret->length_ = len;
  ret->chars_ = chars;

  return ret;
}

ObjString *copyString(const char *start, int len) {
  char *heap_chars = ALLOCATE(char, len + 1);
  memcpy(heap_chars, start, len);
  heap_chars[len] = '\0';

  return allocateString(heap_chars, len);
}

ObjString *takeString(char *chars, int len) {
  return allocateString(chars, len);
}

