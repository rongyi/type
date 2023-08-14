#ifndef clox_object_h
#define clox_object_h

#include "chunk.h"
#include "common.h"
#include "value.h"

typedef enum {
  OBJ_STRING,
  OBJ_FUNCTION,
  OBJ_NATIVE,
  OBJ_CLOSURE,
  OBJ_UPVALUE,
} ObjType;

struct Obj {
  ObjType type_;
  struct Obj *next_;
};

typedef struct {
  Obj base_;
  int arity_;
  int upvalue_cnt_;
  Chunk chunk_;
  ObjString *name_;  // function name, global has no name field
} ObjFunction;

typedef struct ObjUpvalue {
  Obj base_;
  Value *location_;
} ObjUpvalue;

typedef struct {
  Obj base_;
  ObjFunction *function_;
  ObjUpvalue **upvalues_;  // array
  int upvalue_cnt_;
} ObjClosure;

struct ObjString {
  Obj base_;
  int length_;
  char *chars_;
  uint32_t hash_;  // hash cache for current string, when string as key of as
                   // hash table
};

typedef Value (*NativeFn)(int arg_cnt, Value *args);

typedef struct {
  Obj base_;
  NativeFn function_;

} ObjNative;

#define OBJ_TYPE(value) (AS_OBJ(value)->type_)
#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)

#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars_)
#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative *)AS_OBJ(value))->function_)
#define AS_CLOSURE(value) ((ObjClosure *)AS_OBJ(value))

static inline bool isObjType(Value v, ObjType expect) {
  return IS_OBJ(v) && AS_OBJ(v)->type_ == expect;
}

ObjString *copyString(const char *start, int len);
ObjString *takeString(char *chars, int len);
ObjFunction *newFuction();
ObjNative *newNative(NativeFn f);
ObjClosure *newClosure(ObjFunction *f);
ObjUpvalue *newUpvalue(Value *slot);

#endif
