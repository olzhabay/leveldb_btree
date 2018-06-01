#include "ff_btree_iterator.h"

namespace leveldb {


bool BtreeIterator::Valid() const {
  return false;
}

void BtreeIterator::SeekToFirst() {

}

void BtreeIterator::SeekToLast() {

}

void BtreeIterator::Seek(const Slice& target) {

}

void BtreeIterator::Next() {

}

void BtreeIterator::Prev() {

}

int64_t BtreeIterator::key() const {
  return 0;
}

char* BtreeIterator::value() const {
  return nullptr;
}

} // namespace leveldb