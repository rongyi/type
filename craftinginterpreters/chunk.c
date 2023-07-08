
#include "chunk.h"

#include <stdlib.h>

#include "memory.h"

void initChunk(Chunk *c) {
  c->count = 0;
  c->capacity = 0;
  c->code = NULL;
  initValueArray(&c->constants);
}

void writeChunk(Chunk *c, uint8_t b) {
  // not enough space
  if (c->capacity < c->count + 1) {
    int old_cap = c->capacity;
    c->capacity = GROW_CAPACITY(old_cap);
    // oh, macro!
    c->code = GROW_ARRAY(uint8_t, c->code, old_cap, c->capacity);
  }

  c->code[c->count] = b;
  c->count++;
}

void freeChunk(Chunk *c) {
  FREE_ARRAY(uint8_t, c->code, c->capacity);
  freeValueArray(&c->constants);

  initChunk(c);
}

int addConstants(Chunk *c, Value v) {
  writeValueArray(&c->constants, v);

  return c->constants.count - 1;
}
