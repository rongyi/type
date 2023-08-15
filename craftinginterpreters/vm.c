#include "vm.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "value.h"

VM vm;

static Value clockNative(int arg_cnt, Value *args) {
  return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

static void defineNative(const char *name, NativeFn f) {
  push(OBJ_VAL(copyString(name, (int)strlen(name))));
  push(OBJ_VAL(newNative(f)));
  tableSet(&vm.globals_, AS_STRING(vm.stack_[0]), vm.stack_[1]);
  pop();
  pop();
}

static void resetStack() {
  vm.stack_top_ = vm.stack_;
  vm.frame_count_ = 0;
}

void initVM() {
  resetStack();
  vm.objects_head_ = NULL;
  initTable(&vm.string_set_);
  initTable(&vm.globals_);

  defineNative("clock", clockNative);
}

void freeVM() {
  freeTable(&vm.string_set_);
  freeTable(&vm.globals_);
  freeObjects();
}

Value readConstantLong() {
  // ip already at operand position
  CallFrame *frame = &vm.frames_[vm.frame_count_ - 1];
  Chunk *chunk = &frame->closure_->function_->chunk_;
  int vidx = (chunk->code[frame->ip_ - chunk->code + 2] << 16) |
             (chunk->code[frame->ip_ - chunk->code + 1] << 8) |
             (chunk->code[frame->ip_ - chunk->code + 0]);
  frame->ip_ += 3;

  return chunk->constants.values[vidx];
}
static Value peek(int distance) { return vm.stack_top_[-1 - distance]; }
static void runtimeError(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  for (int i = vm.frame_count_ - 1; i >= 0; i--) {
    CallFrame *frame = &vm.frames_[i];
    ObjFunction *f = frame->closure_->function_;
    size_t instruction = frame->ip_ - f->chunk_.code - 1;
    fprintf(stderr, "[line %d] in ", f->chunk_.lines[instruction]);

    if (f->name_ == NULL) {
      fprintf(stderr, "script\n");
    } else {
      fprintf(stderr, "%s()\n", f->name_->chars_);
    }
  }

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

static bool call(ObjClosure *closure, int arg_cnt) {
  if (arg_cnt != closure->function_->arity_) {
    runtimeError("Expect %d arguments but got %d.", closure->function_->arity_,
                 arg_cnt);
    return false;
  }
  if (vm.frame_count_ == FRAMES_MAX) {
    runtimeError("Stack overflow.");
    return false;
  }

  CallFrame *frame = &vm.frames_[vm.frame_count_++];
  frame->closure_ = closure;
  frame->ip_ = closure->function_->chunk_.code;
  frame->slots_ = vm.stack_top_ - arg_cnt - 1;

  return true;
}

static bool callValue(Value callee, int arg_cnt) {
  if (IS_OBJ(callee)) {
    switch (OBJ_TYPE(callee)) {
      case OBJ_CLOSURE:
        return call(AS_CLOSURE(callee), arg_cnt);
      case OBJ_NATIVE: {
        NativeFn native = AS_NATIVE(callee);
        Value ret = native(arg_cnt, vm.stack_top_ - arg_cnt);
        vm.stack_top_ -= arg_cnt + 1;
        push(ret);
        return true;
      }

      default:
        // fall as error
        break;
    }
  }
  runtimeError("Can only call functions and clauses.");
  return false;
}

static ObjUpvalue *captureUpvalue(Value *local) {
  ObjUpvalue *created_upvalue = newUpvalue(local);

  return created_upvalue;
}

static InterpretResult run() {
  // the last frame
  CallFrame *frame = &vm.frames_[vm.frame_count_ - 1];

#define READ_BYTE() (*frame->ip_++)
#define READ_CONSTANT() \
  (frame->closure_->function_->chunk_.constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define READ_SHORT() \
  (frame->ip_ += 2, (uint16_t)((frame->ip_[-1] << 8) | (frame->ip_[-2])))
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
    disassembleInstruction(&frame->function_->chunk_,
                           (int)(frame->ip_ - frame->function_->chunk_.code));
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
        push(frame->slots_[slot]);
        break;
      }
      case OP_SET_LOCAL: {
        uint8_t slot = READ_BYTE();
        // name are just for index, they don't have name to search
        // just the idx
        /*vm.stack_[slot] = peek(0);*/
        frame->slots_[slot] = peek(0);
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
      case OP_GET_UPVALUE: {
        uint8_t slot = READ_BYTE();
        push(*frame->closure_->upvalues_[slot]->location_);
        break;
      }
      case OP_SET_UPVALUE: {
        uint8_t slot = READ_BYTE();
        *frame->closure_->upvalues_[slot]->location_ = peek(0);
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
          /*vm.ip_ += offset;*/
          frame->ip_ += offset;
        }
        break;
      }
      case OP_JUMP: {
        uint16_t offset = READ_SHORT();
        /*vm.ip_ += offset;*/
        frame->ip_ += offset;
        break;
      }
      case OP_LOOP: {
        uint16_t back = READ_SHORT();
        /*vm.ip_ -= back;*/
        frame->ip_ -= back;
        break;
      }

      case OP_CALL: {
        int arg_cnt = READ_BYTE();
        if (!callValue(peek(arg_cnt), arg_cnt)) {
          return INTERPRET_RUNTIME_ERROR;
        }
        // after evaluation
        frame = &vm.frames_[vm.frame_count_ - 1];
        break;
      }
      case OP_CLOSURE: {
        ObjFunction *f = AS_FUNCTION(READ_CONSTANT());
        ObjClosure *closure = newClosure(f);
        push(OBJ_VAL(closure));
        for (int i = 0; i < closure->upvalue_cnt_; i++) {
          uint8_t is_local = READ_BYTE();
          uint8_t index = READ_BYTE();
          if (is_local) {
            // An OP_CLOSURE instruction is emitted at the end of a function
            // declaration. At the moment that we are executing that
            // declaration, the current function is the surrounding one. That
            // means the current function’s closure is stored in the CallFrame
            // at the top of the callstack. So, to grab an upvalue from the
            // enclosing function, we can read it right from the frame local
            // variable, which caches a reference to that CallFrame.
            //
            // only call can affect the frame shrink or expand
            // OP_CLOSURE is to get this function and push to stack
            // but no call yet, so there's no frame incresing
            // so the top frame is the enclosing one
            closure->upvalues_[i] = captureUpvalue(frame->slots_ + index);
          } else {
            closure->upvalues_[i] = frame->closure_->upvalues_[index];
          }
        }
        break;
      }

      case OP_RETURN: {
        // goodbye, return, we have print now
        /*printValue(pop());*/
        /*printf("\n");*/
        Value ret = pop();
        vm.frame_count_--;
        if (vm.frame_count_ == 0) {
          // the script
          pop();
          return INTERPRET_OK;
        }
        vm.stack_top_ = frame->slots_;
        push(ret);
        frame = &vm.frames_[vm.frame_count_ - 1];
        break;
      }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_STRING
#undef READ_SHORT
}

InterpretResult interpret(const char *source) {
// legacy code
#if 0
  Chunk c;
  initChunk(&c);
  if (!compile(source, &c)) {
    freeChunk(&c);
    return INTERPRET_COMPILE_ERROR;
  }
  vm.chunk_ = &c;
  vm.ip_ = vm.chunk_->code;
#endif
  ObjFunction *f = compile(source);
  if (f == NULL) {
    return INTERPRET_COMPILE_ERROR;
  }
  push(OBJ_VAL(f));
  ObjClosure *closure = newClosure(f);
  pop();
  push(OBJ_VAL(closure));
  call(closure, 0);

  // a new frame
  /*CallFrame *frame = &vm.frames_[vm.frame_count_++];*/
  /*frame->closure_->function_ = f;*/
  /*frame->ip_ = f->chunk_.code;*/
  /*frame->slots_ = vm.stack_;  // this line mark the base pointer*/

  return run();
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
