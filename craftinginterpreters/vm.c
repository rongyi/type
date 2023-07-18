#include "vm.h"

#include <stdarg.h>
#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"

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
static Value peek(int distance) { return vm.stack_top_[-1 - distance]; }
static void runtimeError(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm.ip_ - vm.chunk_->code - 1;
  int line = vm.chunk_->lines[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}

static bool isFalsey(Value v) {
  return IS_NIL(v) || (IS_BOOL(v) && !AS_BOOL(v));
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip_++)
#define READ_CONSTANT() (vm.chunk_->constants.values[READ_BYTE()])
#define BINARY_OP(valuetype, op)                      \
  do {                                                \
    if (!IS_NUMBER(peek(0)) && !IS_NUMBER(peek(1))) { \
      runtimeError("Operands mut be numbers.");       \
      return INTERPRET_RUNTIME_ERROR;                 \
    }                                                 \
    double b = AS_NUMBER(pop());                      \
    double a = AS_NUMBER(pop());                      \
    push(valuetype(a op b));                          \
  } while (false)

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
      case OP_NIL:
        push(NIL_VAL);
        break;
      case OP_TRUE:
        push(BOOL_VAL(true));
        break;
      case OP_FALSE:
        push(BOOL_VAL(false));
        break;
      case OP_EQUAL: {
        Value b = pop();
        Value a = pop();
        push(BOOL_VAL(valuesEqual(a, b)));
        break;
      }
      case OP_GREATER: {
        BINARY_OP(BOOL_VAL, >);
        break;
      }
      case OP_LESS: {
        BINARY_OP(BOOL_VAL, <);
        break;
      }
      case OP_NEGATE: {
        if (!IS_NUMBER(peek(0))) {
          runtimeError("Operand must be a number");
          return INTERPRET_RUNTIME_ERROR;
        }
        push(NUMBER_VAL(-AS_NUMBER(pop())));
        break;
      }
      case OP_ADD: {
        BINARY_OP(NUMBER_VAL, +);
        break;
      }
      case OP_SUBSTRACT: {
        BINARY_OP(NUMBER_VAL, -);
        break;
      }
      case OP_MULTIPLY: {
        BINARY_OP(NUMBER_VAL, *);
        break;
      }
      case OP_DIVIDE: {
        BINARY_OP(NUMBER_VAL, /);
        break;
      }
      case OP_NOT:
        push(BOOL_VAL(isFalsey(pop())));
        break;
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
  if (!compile(source, &c)) {
    freeChunk(&c);
    return INTERPRET_COMPILE_ERROR;
  }
  vm.chunk_ = &c;
  vm.ip_ = vm.chunk_->code;

  InterpretResult ret = run();
  freeChunk(&c);

  return ret;
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
