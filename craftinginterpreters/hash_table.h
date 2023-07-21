#ifndef clox_hash_table_h
#define clox_hash_table_h

#include "common.h"
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

#endif
