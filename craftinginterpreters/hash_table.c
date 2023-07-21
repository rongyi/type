#include "hash_table.h"

#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"

#define TABLE_LOAD_FACTOR 0.75

void initTable(Table *table) {
  table->count_ = 0;
  table->capacity_ = 0;
  table->entries_ = NULL;
}

void freeTable(Table *table) {
  FREE_ARRAY(Entry, table->entries_, table->capacity_);
  initTable(table);
}

// find the first empty slot or key equals search key
static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
  uint32_t idx = key->hash_ % capacity;
  while (true) {
    Entry *cur = &entries[idx];
    if (cur->key_ == key || cur->key_ == NULL) {
      return cur;
    }
    idx = (idx + 1) % capacity;
  }
}

static void adjustCapacity(Table *table, int newcap) {
  Entry *entries = ALLOCATE(Entry, newcap);
  for (int i = 0; i < newcap; i++) {
    entries[i].key_ = NULL;
    entries[i].value_ = NIL_VAL;
  }

  // "memcpy" for hash table
  for (int i = 0; i < table->capacity_; i++) {
    Entry *cur = &table->entries_[i];
    if (cur->key_ == NULL) {
      continue;
    }
    Entry *dest = findEntry(entries, newcap, cur->key_);
    dest->key_ = cur->key_;
    dest->value_ = cur->value_;
  }

  FREE_ARRAY(Entry, table->entries_, table->capacity_);
  table->entries_ = entries;
  table->capacity_ = newcap;
}

// tableSet insert {k, v} return true if key is not in cache
bool tableSet(Table *table, ObjString *key, Value v) {
  if (table->count_ + 1 > table->capacity_ * TABLE_LOAD_FACTOR) {
    int capacity = GROW_CAPACITY(table->capacity_);
    adjustCapacity(table, capacity);
  }
  Entry *entry = findEntry(table->entries_, table->capacity_, key);
  bool is_new = entry->key_ == NULL;
  if (is_new) {
    table->count_++;
  }

  entry->key_ = key;
  entry->value_ = v;

  return is_new;
}

void tableAddAll(Table *from, Table *to) {
  for (int i = 0; i < from->capacity_; i++) {
    Entry *cur = &from->entries_[i];
    if (cur->key_ != NULL) {
      tableSet(to, cur->key_, cur->value_);
    }
  }
}
