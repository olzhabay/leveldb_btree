#ifndef STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_
#define STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_

#include "leveldb/iterator.h"
#include "index/ff_btree.h"

namespace leveldb {

class FFBtreeIterator : public Iterator {

public:
  FFBtreeIterator(char* b);

  bool Valid() const override;

  void SeekToFirst() override;

  void SeekToLast() override;

  void Seek(const Slice& target) override;

  void Next() override;

  void Prev() override;

  Slice key() const override;

  Slice value() const override;

  Status status() const override;

private:
  char *btree, *cur, *cur_page;
  int index;
  bool valid; // validity of current entry
};

} // namespace leveldb

#endif // STORAGE_LEVELDB_INDEX_FF_BTREE_ITERATOR_H_
