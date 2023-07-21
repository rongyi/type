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

static ObjString *allocateString(char *chars, int len, uint32_t hash) {
  ObjString *ret = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  ret->length_ = len;
  ret->chars_ = chars;
  ret->hash_ = hash;

  return ret;
}

static uint32_t hashString(const char *key, int len) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < len; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }

  return hash;
}

// here the start is in token, we don't touch it
ObjString *copyString(const char *start, int len) {
  uint32_t hash = hashString(start, len);
  ObjString *interned = tableFindString(&vm.string_set_, start, len, hash);
  if (interned != NULL) {
    return interned;
  }

  char *heap_chars = ALLOCATE(char, len + 1);
  memcpy(heap_chars, start, len);
  heap_chars[len] = '\0';

  return allocateString(heap_chars, len, hash);
}

ObjString *takeString(char *chars, int len) {
  uint32_t hash = hashString(chars, len);
  ObjString *interned = tableFindString(&vm.string_set_, chars, len, hash);
  if (interned != NULL) {
    // we own this chars, don't use it, release it
    FREE_ARRAY(char, chars, len + 1);
    return interned;
  }

  return allocateString(chars, len, hash);
}

