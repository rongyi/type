#include "vm.h"

#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"

VM vm;

static void resetStack() { vm.stack_top_ = vm.stack_; }

void initVM() { resetStack(); }

void freeVM() {}

Value readConstantLong() {
  // ip already at operand position
  int vidx = (vm.chunk_->code[vm.ip_ - vm.chunk_->code + 2] << 16) |
             (vm.chunk_->code[vm.ip_ - vm.chunk_->code + 1] << 8) |
             (vm.chunk_->code[vm.ip_ - vm.chunk_->code + 0]);
  vm.ip_ += 3;

  return vm.chunk_->constants.values[vidx];
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip_++)
#define READ_CONSTANT() (vm.chunk_->constants.values[READ_BYTE()])
  while (true) {
    // offset is ip - start
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value *slot = vm.stack_; slot < vm.stack_top_; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm.chunk_, (int)(vm.ip_ - vm.chunk_->code));
#endif
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_CONSTANT_LONG: {
        Value constant = readConstantLong();
        push(constant);
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_NEGATE: {
        push(-pop());
        break;
      }
      case OP_ADD: {
        Value v1 = pop();
        Value v2 = pop();
        push(v1 + v2);
        break;
      }
      case OP_SUBSTRACT: {
        Value v1 = pop();
        Value v2 = pop();
        push(v2 - v1);
        break;
      }
      case OP_MULTIPLY: {
        Value v1 = pop();
        Value v2 = pop();
        push(v1 * v2);
        break;
      }
      case OP_DIVIDE: {
        Value v1 = pop();
        Value v2 = pop();
        push(v2 / v1);
        break;
      }
      case OP_RETURN: {
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(const char *source) {
  Chunk c;
  initChunk(&c);
  compile(source, &c);

  return INTERPRET_OK;
}

#if 0
InterpretResult interpret(Chunk *c) {
  vm.chunk_ = c;
  vm.ip_ = vm.chunk_->code;

  return run();
}
#endif

void push(Value v) {
  *vm.stack_top_ = v;
  vm.stack_top_++;
}

Value pop() {
  vm.stack_top_--;

  return *vm.stack_top_;
}
