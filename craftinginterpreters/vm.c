#include "vm.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "value.h"

VM vm;

static void resetStack() { vm.stack_top_ = vm.stack_; }

void initVM() {
  resetStack();
  vm.objects_head_ = NULL;
  initTable(&vm.string_set_);
  initTable(&vm.globals_);
}

void freeVM() {
  freeTable(&vm.string_set_);
  freeTable(&vm.globals_);
  freeObjects();
}

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
static void concatenate() {
  ObjString *b = AS_STRING(pop());
  ObjString *a = AS_STRING(pop());
  int len = a->length_ + b->length_;
  char *chars = ALLOCATE(char, len + 1);
  memcpy(chars, a->chars_, a->length_);
  memcpy(chars + a->length_, b->chars_, b->length_);
  chars[len] = '\0';
  ObjString *ret = takeString(chars, len);

  push(OBJ_VAL(ret));
}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip_++)
#define READ_CONSTANT() (vm.chunk_->constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define READ_SHORT() (vm.ip_ += 2, (uint16_t)((vm.ip_[-1] << 8) | (vm.ip_[-2])))
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
        /*printValue(constant);*/
        /*printf("\n");*/
        break;
      }
      case OP_CONSTANT_LONG: {
        Value constant = readConstantLong();
        push(constant);
        /*printValue(constant);*/
        /*printf("\n");*/
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
      case OP_POP:
        pop();
        break;
      case OP_GET_GLOBAL: {
        // constant idx put in code array
        ObjString *key = READ_STRING();
        Value v;
        if (!tableGet(&vm.globals_, key, &v)) {
          runtimeError("Undefined variable '%s'.", key->chars_);
          return INTERPRET_RUNTIME_ERROR;
        }
        push(v);
        break;
      }
      case OP_SET_GLOBAL: {
        ObjString *key = READ_STRING();
        if (tableSet(&vm.globals_, key, peek(0))) {
          tableDelete(&vm.globals_, key);
          runtimeError("Undefined variable '%s'.", key->chars_);
          return INTERPRET_RUNTIME_ERROR;
        }
        break;
      }
      case OP_GET_LOCAL: {
        uint8_t slot = READ_BYTE();
        push(vm.stack_[slot]);
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t slot = READ_BYTE();
        vm.stack_[slot] = peek(0);
        /* Note that it doesn’t pop the value from the stack. Remember,
         * assignment is an expression, and every expression produces a value.
         * The value of an assignment expression is the assigned value itself,
         * so the VM just leaves the value on the stack.*/
        break;
      }

      case OP_DEFINE_GLOBAL: {
        ObjString *name = READ_STRING();
        tableSet(&vm.globals_, name, peek(0));
        // the value for bind to this variable
        pop();
        break;
      }
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
        if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
          concatenate();
        } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
          double b = AS_NUMBER(pop());
          double a = AS_NUMBER(pop());
          push(NUMBER_VAL(a + b));
        } else {
          runtimeError("Operands must be two numbers or two strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
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
      case OP_PRINT:
        printValue(pop());
        printf("\n");
        break;
      case OP_JUMP_IF_FALSE: {
        uint16_t offset = READ_SHORT();
        if (isFalsey(peek(0))) {
          vm.ip_ += offset;
        }
        break;
      }
      case OP_JUMP: {
        uint16_t offset = READ_SHORT();
        vm.ip_ += offset;
        break;
      }
      case OP_LOOP: {
        uint16_t back = READ_SHORT();
        vm.ip_ -= back;
        break;
      }

      case OP_RETURN: {
        // goodbye, return, we have print now
        /*printValue(pop());*/
        /*printf("\n");*/
        return INTERPRET_OK;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_STRING
#undef READ_SHORT
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
