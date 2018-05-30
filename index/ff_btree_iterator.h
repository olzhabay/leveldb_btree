#ifndef STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_
#define STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_

#include "leveldb/iterator.h"

namespace leveldb {

class FFBtreeIterator : public Iterator {

public:
  bool Valid() const override;

  void SeekToFirst() override;

  void SeekToLast() override;

  void Seek(const Slice& target) override;

  void Next() override;

  void Prev() override;

  Slice key() const override;

  Slice value() const override;

  Status status() const override;

};

} // namespace leveldb

#endif // STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_
