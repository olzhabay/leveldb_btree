#ifndef STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_
#define STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_

#include "leveldb/iterator.h"

namespace leveldb {

class BtreeIterator {

public:
  bool Valid() const ;

  void SeekToFirst();

  void SeekToLast();

  void Seek(const Slice& target);

  void Next();

  void Prev();

  // return entry key
  int64_t key() const;

  // return entry pointer
  char* value() const;

};

} // namespace leveldb

#endif // STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_
