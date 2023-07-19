#ifndef clox_value_h
#define clox_value_h
#include <stdbool.h>

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
  VAL_BOOL,
  VAL_NIL,
  VAL_NUMBER,
  VAL_OBJ,
} ValueType;
// typedef double Value;

typedef struct {
  ValueType type_;
  union {
    bool boolean_;
    double number_;
    Obj *obj_;
  } as_;
} Value;

#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean_ = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number_ = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number_ = value}})
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj_ = (Obj *)object}})

#define AS_BOOL(value) ((value).as_.boolean_)
#define AS_NUMBER(value) ((value).as_.number_)
#define AS_OBJ(value) ((value).as_.obj_)

#define IS_BOOL(value) ((value).type_ == VAL_BOOL)
#define IS_NIL(value) ((value).type_ == VAL_NIL)
#define IS_NUMBER(value) ((value).type_ == VAL_NUMBER)
#define IS_OBJ(value) ((value).type_ == VAL_OBJ)

typedef struct {
  int capacity;
  int count;
  Value *values;
} ValueArray;

bool valuesEqual(Value a, Value b);
void initValueArray(ValueArray *va);
void writeValueArray(ValueArray *va, Value v);
void freeValueArray(ValueArray *va);

// debug
void printValue(Value v);
void printObject(Value v);

#endif
