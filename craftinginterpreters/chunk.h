#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT,
  OP_CONSTANT_LONG,
  OP_RETURN,
} OpCode;

typedef struct {
  int count;
  int capacity;
  uint8_t *code;
  int *lines;
  // constants to fetch from, just like initialized data section
  ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *c, uint8_t b, int line);
void writeConstant(Chunk *c, Value v, int line);

int addConstants(Chunk *c, Value v);
void freeChunk(Chunk *c);

#endif
