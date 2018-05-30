#include "ff_btree_iterator.h"

namespace leveldb {


bool FFBtreeIterator::Valid() const {
  return false;
}

void FFBtreeIterator::SeekToFirst() {

}

void FFBtreeIterator::SeekToLast() {

}

void FFBtreeIterator::Seek(const Slice& target) {

}

void FFBtreeIterator::Next() {

}

void FFBtreeIterator::Prev() {

}

Slice FFBtreeIterator::key() const {
  return Slice();
}

Slice FFBtreeIterator::value() const {
  return Slice();
}

Status FFBtreeIterator::status() const {
  return Status();
}

} // namespace leveldb