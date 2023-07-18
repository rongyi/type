#include "value.h"

#include <stdio.h>

#include "memory.h"

void initValueArray(ValueArray *va) {
  va->capacity = 0;
  va->count = 0;
  va->values = NULL;
}

void writeValueArray(ValueArray *va, Value v) {
  if (va->capacity < va->count + 1) {
    int old_cap = va->capacity;
    va->capacity = GROW_CAPACITY(old_cap);
    va->values = GROW_ARRAY(Value, va->values, old_cap, va->capacity);
  }

  va->values[va->count] = v;
  va->count++;
}

void freeValueArray(ValueArray *va) {
  FREE_ARRAY(Value, va->values, va->capacity);
  initValueArray(va);
}

void printValue(Value v) {
  switch (v.type_) {
    case VAL_BOOL:
      printf(AS_BOOL(v) ? "true" : "false");
      break;
    case VAL_NIL:
      printf("nil");
      break;
    case VAL_NUMBER:
      printf("%g", AS_NUMBER(v));
      break;
  }
}

bool valuesEqual(Value a, Value b) {
  if (a.type_ != b.type_) {
    return false;
  }
  switch (a.type_) {
    case VAL_BOOL:
      return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:
      return true;
    case VAL_NUMBER:
      return AS_NUMBER(a) == AS_NUMBER(b);
    default:
      return false;
  }
}
