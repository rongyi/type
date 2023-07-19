#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"

typedef enum {
  OBJ_STRING,
} ObjType;

struct Obj {
  ObjType type_;
};

struct ObjString {
  ObjType type_;
  int length_;
  char *chars_;
};

#define OBJ_TYPE(value) (AS_OBJ(value)->type_)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars_)

static inline bool isObjType(Value v, ObjType expect) {
  return IS_OBJ(v) && AS_OBJ(v)->type_ == expect;
}

ObjString *copyString(const char *start, int len);

#endif
