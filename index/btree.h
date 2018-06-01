#ifndef STORAGE_LEVELDB_INDEX_BTREE_H_
#define STORAGE_LEVELDB_INDEX_BTREE_H_

#include <cstdint>
#include "leveldb/iterator.h"

namespace leveldb {

class Btree {
  virtual void Insert(int64_t, char*) = 0;
  virtual char* Search(int64_t) = 0;
  virtual void Remove(int64_t) = 0;
};

} // namespace leveldb

#endif // STORAGE_LEVELDB_INDEX_BTREE_H_
