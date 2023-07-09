#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"


typedef struct {
  Chunk *chunk_;
  // always point to the *next* instruction
  uint8_t *ip_;
} VM;

void initVM();
void freeVM();


typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

InterpretResult interpret(Chunk *c);

#endif
