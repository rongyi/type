#ifndef clox_hash_table_h
#define clox_hash_table_h

#include "common.h"
#include "object.h"
#include "string.h"
#include "value.h"

typedef struct {
  ObjString *key_;
  Value value_;
} Entry;

typedef struct {
  int count_;
  int capacity_;
  Entry *entries_;
} Table;

void initTable(Table *table);
void freeTable(Table *table);

bool tableSet(Table *table, ObjString *key, Value v);
void tableAddAll(Table *from, Table *to);
bool tableGet(Table *table, ObjString *key, Value *output);
bool tableDelete(Table *table, ObjString *key);

// find string instance start from chars with len, and hash: hash
// return key
ObjString *tableFindString(Table *table, const char *chars, int len,
                           uint32_t hash);

#endif
