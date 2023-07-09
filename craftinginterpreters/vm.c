#include "vm.h"

#include <stdio.h>

#include "common.h"
#include "debug.h"

VM vm;

void initVM() {}

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
    disassembleInstruction(vm.chunk_, (int)(vm.ip_ - vm.chunk_->code));
#endif
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_CONSTANT_LONG: {
        Value constant = readConstantLong();
        printValue(constant);
        printf("\n");
        break;
      }
      case OP_RETURN: {
        return INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk *c) {
  vm.chunk_ = c;
  vm.ip_ = vm.chunk_->code;

  return run();
}
