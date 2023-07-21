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

ObjString *copyString(const char *start, int len) {
  char *heap_chars = ALLOCATE(char, len + 1);
  memcpy(heap_chars, start, len);
  heap_chars[len] = '\0';

  uint32_t hash = hashString(start, len);
  return allocateString(heap_chars, len, hash);
}

ObjString *takeString(char *chars, int len) {
  uint32_t hash = hashString(chars, len);
  return allocateString(chars, len, hash);
}

