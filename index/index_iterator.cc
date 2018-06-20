#include <util/coding.h>
#include "index_iterator.h"
#include "db/dbformat.h"

namespace leveldb {

static void DeleteIterator(const Slice& key, void* value) {
  Iterator* iterator = reinterpret_cast<Iterator*>(value);
  delete iterator;
}

IndexIterator::IndexIterator(ReadOptions options, FFBtreeIterator* btree_iter, TableCache* table_cache)
    : options_(options),
      btree_iterator_(btree_iter),
      table_cache_(table_cache),
      block_iterator_(NULL),
      cache_(NewLRUCache(8 << 20)){
  SeekToFirst();
}

IndexIterator::~IndexIterator() {
  delete cache_;
  delete btree_iterator_;
}

bool IndexIterator::Valid() const {
  return btree_iterator_->Valid();
}

void IndexIterator::SeekToFirst() {
  btree_iterator_->SeekToFirst();
}

void IndexIterator::SeekToLast() {
  btree_iterator_->SeekToLast();
}

void IndexIterator::Seek(const Slice& target) {
  btree_iterator_->Seek(fast_atoi(target));
  Advance();
  block_iterator_->Seek(target);
  status_ = block_iterator_->status();
}

void IndexIterator::Next() {
  btree_iterator_->Next();
  Advance();
  while (fast_atoi(block_iterator_->key()) < btree_iterator_->key()) {
    block_iterator_->Next();
  }
  if (fast_atoi(block_iterator_->key()) != btree_iterator_->key()) {
    status_ = Status::NotFound(std::to_string(btree_iterator_->key()));
  }
}

void IndexIterator::Prev() {
  btree_iterator_->Prev();
}

Slice IndexIterator::key() const {
  return block_iterator_->key();
}

Slice IndexIterator::value() const {
  return block_iterator_->value();
}

Status IndexIterator::status() const {
  if (!block_iterator_->status().ok()) return block_iterator_->status();
  return status_;
}

void IndexIterator::CacheLookup() {
  char buf[sizeof(void*)];
  EncodeFixed64(buf, (uint64_t) btree_iterator_->value());
  Slice cache_key(buf, sizeof(buf));
  Cache::Handle* handle = cache_->Lookup(cache_key);
  if (handle == nullptr) {
    status_ = table_cache_->GetBlockIterator(options_, index_meta_.file_number,
                                             index_meta_.offset, index_meta_.size, block_iterator_);
    if (!status_.ok()) return; // something went wrong
    handle = cache_->Insert(cache_key, block_iterator_, index_meta_.size ,&DeleteIterator);
  }
  block_iterator_ = reinterpret_cast<Iterator*>(cache_->Value(handle));
}

void IndexIterator::Advance() {
  if (btree_iterator_->value() != convert(index_meta_)) {
    index_meta_ = convert(btree_iterator_->value());
    CacheLookup();
  }
}

}
