
#include "chunk.h"

#include <stdlib.h>

#include "memory.h"

void initChunk(Chunk *c) {
  c->count = 0;
  c->capacity = 0;
  c->code = NULL;
  c->lines = NULL;

  initValueArray(&c->constants);
}

void writeChunk(Chunk *c, uint8_t b, int line) {
  // not enough space
  if (c->capacity < c->count + 1) {
    int old_cap = c->capacity;
    c->capacity = GROW_CAPACITY(old_cap);
    // oh, macro!
    c->code = GROW_ARRAY(uint8_t, c->code, old_cap, c->capacity);
    c->lines = GROW_ARRAY(int, c->lines, old_cap, c->capacity);
  }

  c->code[c->count] = b;
  c->lines[c->count] = line;
  c->count++;
}

void writeConstant(Chunk *c, Value v, int line) {
  // not enough space
  if (c->capacity < c->count + 1) {
    int old_cap = c->capacity;
    c->capacity = GROW_CAPACITY(old_cap);
    // oh, macro!
    c->code = GROW_ARRAY(uint8_t, c->code, old_cap, c->capacity);
    c->lines = GROW_ARRAY(int, c->lines, old_cap, c->capacity);
  }

  // first op
  c->code[c->count] = OP_CONSTANT_LONG;
  c->lines[c->count] = line;
  c->count++;

  // then operand
  int idx = addConstants(c, v);

  writeChunk(c, (idx >> 0) & 0xff, line);
  writeChunk(c, (idx >> 8) & 0xff, line);
  writeChunk(c, (idx >> 16) & 0xff, line);
}

void freeChunk(Chunk *c) {
  FREE_ARRAY(uint8_t, c->code, c->capacity);
  FREE_ARRAY(int, c->lines, c->capacity);
  freeValueArray(&c->constants);

  initChunk(c);
}

int addConstants(Chunk *c, Value v) {
  writeValueArray(&c->constants, v);

  return c->constants.count - 1;
}
