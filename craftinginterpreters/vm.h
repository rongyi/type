#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "hash_table.h"
#include "object.h"
#include "value.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
  ObjFunction *function_;
  uint8_t *ip_;
  Value *slots_;
} CallFrame;

typedef struct {
  // Chunk *chunk_;
  //// always point to the *next* instruction
  // uint8_t *ip_;
  CallFrame frames_[FRAMES_MAX];
  int frame_count_;

  Value stack_[STACK_MAX];
  // point ot next place to write!
  Value *stack_top_;
  Obj *objects_head_;
  Table string_set_;
  Table globals_;
} VM;

extern VM vm;

void initVM();
void freeVM();

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

// InterpretResult interpret(Chunk *c);
InterpretResult interpret(const char *source);

void push(Value v);
Value pop();

#endif
