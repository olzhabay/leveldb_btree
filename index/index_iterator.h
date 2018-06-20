#ifndef STORAGE_LEVELDB_INDEX_INDEX_ITERATOR_H_
#define STORAGE_LEVELDB_INDEX_INDEX_ITERATOR_H_

#include <vector>
#include "leveldb/iterator.h"
#include "leveldb/index.h"
#include "index/ff_btree_iterator.h"
#include "table/format.h"
#include "db/version_set.h"
#include "db/table_cache.h"

namespace leveldb {

class IndexIterator : public Iterator {
 public:
  IndexIterator(ReadOptions options, FFBtreeIterator* btree_iter, TableCache* table_cache);
  ~IndexIterator();

  virtual bool Valid() const;
  virtual void SeekToFirst();
  virtual void SeekToLast();
  virtual void Seek(const Slice& target);
  virtual void Next();
  virtual void Prev();
  virtual Slice key() const;
  virtual Slice value() const;
  virtual Status status() const;

 private:
  FFBtreeIterator* btree_iterator_;
  Cache* cache_;
  IndexMeta index_meta_;
  ReadOptions options_;
  TableCache* table_cache_;
  Iterator* block_iterator_;
  Status status_;

  void CacheLookup();
  void Advance();
};

}

#endif // STORAGE_LEVELDB_INDEX_INDEX_ITERATOR_H_
