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
  Entry *tomestone = NULL;
  while (true) {
    Entry *cur = &entries[idx];
    if (cur->key_ == NULL) {
      if (IS_NIL(cur->value_)) {
        if (tomestone != NULL) {
          return tomestone;
        }
        return cur;
      } else {
        // a tomestone in this slot, use first occurance
        if (tomestone == NULL) {
          tomestone = cur;
        }
      }
    } else if (cur->key_ == key) {
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
  table->count_ = 0;

  // "memcpy" for hash table
  for (int i = 0; i < table->capacity_; i++) {
    Entry *cur = &table->entries_[i];
    if (cur->key_ == NULL) {
      continue;
    }
    Entry *dest = findEntry(entries, newcap, cur->key_);
    dest->key_ = cur->key_;
    dest->value_ = cur->value_;
    table->count_++;
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
  // consider tomestone to be full buckets
  // reuse don't increse count
  // only the fresh slot is taken, we increase the count
  if (is_new && IS_NIL(entry->value_)) {
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

bool tableGet(Table *table, ObjString *key, Value *out) {
  if (table->count_ == 0) {
    return false;
  }
  Entry *entry = findEntry(table->entries_, table->capacity_, key);
  if (entry->key_ == NULL) {
    return false;
  }
  *out = entry->value_;

  return true;
}

bool tableDelete(Table *table, ObjString *key) {
  if (table->count_ == 0) {
    return false;
  }
  Entry *entry = findEntry(table->entries_, table->capacity_, key);
  if (entry->key_ == NULL) {
    return false;
  }
  entry->key_ = NULL;
  // a tomestone indicator
  entry->value_ = BOOL_VAL(true);

  return true;
}

ObjString *tableFindString(Table *table, const char *chars, int len,
                           uint32_t hash) {
  if (table->count_ == 0) {
    return NULL;
  }

  uint32_t idx = hash % table->capacity_;
  while (true) {
    Entry *cur = &table->entries_[idx];
    if (cur->key_ == NULL) {
      if (IS_NIL(cur->value_)) {
        return NULL;
      }
    } else if (cur->key_->length_ == len && cur->key_->hash_ == hash &&
               memcmp(cur->key_->chars_, chars, len) == 0) {
      return cur->key_;
    }

    idx = (idx + 1) % table->capacity_;
  }
}
