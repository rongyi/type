#ifndef clox_value_h
#define clox_value_h

typedef double Value;

typedef struct {
  int capacity;
  int count;
  Value *values;
} ValueArray;

void initValueArray(ValueArray *va);
void writeValueArray(ValueArray *va, Value v);
void freeValueArray(ValueArray *va);



// debug
void printValue(Value v);

#endif
