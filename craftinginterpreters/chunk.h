#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
  OP_CONSTANT,
  OP_CONSTANT_LONG,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_DEFINE_GLOBAL,
  OP_GET_GLOBAL,
  OP_EQUAL,  // no <=  and >= ? why? >= -> !< and <= -> !>
  OP_GREATER,
  OP_LESS,
  OP_ADD,
  OP_SUBSTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_NEGATE,  // no operand, other instruction push to stack
  OP_PRINT,
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
