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
  printf("%g", v);
}
