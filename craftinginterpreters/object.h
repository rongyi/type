#ifndef clox_object_h
#define clox_object_h

#include "chunk.h"
#include "common.h"
#include "value.h"

typedef enum {
  OBJ_STRING,
  OBJ_FUNCTION,
} ObjType;

struct Obj {
  ObjType type_;
  struct Obj *next_;
};

typedef struct {
  Obj base_;
  int arity_;
  Chunk chunk_;
  ObjString *name_;  // function name, global has no name field
} ObjFunction;

struct ObjString {
  Obj base_;
  int length_;
  char *chars_;
  uint32_t hash_;  // hash cache for current string, when string as key of as
                   // hash table
};

#define OBJ_TYPE(value) (AS_OBJ(value)->type_)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars_)

static inline bool isObjType(Value v, ObjType expect) {
  return IS_OBJ(v) && AS_OBJ(v)->type_ == expect;
}

ObjString *copyString(const char *start, int len);
ObjString *takeString(char *chars, int len);
ObjFunction *newFuction();

#endif
