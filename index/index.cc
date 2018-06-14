#include <stdlib.h>
#include "util/coding.h"
#include "leveldb/slice.h"
#include "leveldb/index.h"
#include "index_iterator.h"

namespace leveldb {

Index::Index()
    : condvar_(port::CondVar(&mutex_))  {
  free_ = true;
  bgstarted_ = false;
}

const IndexMeta* Index::Get(const Slice& key) {
  auto result = tree_.Search(fast_atoi(key.data(), key.size()));
  return reinterpret_cast<const IndexMeta *>(result);
}

void Index::Insert(const uint32_t& key, IndexMeta* meta) {
  clflush((char *) meta, sizeof(IndexMeta));
  tree_.Insert(key, (char*) meta);
}

Iterator* Index::Range(const uint32_t& begin, const uint32_t& end, void* ptr) {
  // will be changed to iterator based range query
//  std::vector<LeafEntry*> entries = tree_.Range(begin, end);
//  Iterator* iter = new IndexIterator(entries, ptr);
  return nullptr;
}

void Index::AsyncInsert(const KeyAndMeta& key_and_meta) {
  mutex_.Lock();
  if (!bgstarted_) {
    bgstarted_ = true;
    port::PthreadCall("create thread", pthread_create(&thread_, NULL, &Index::ThreadWrapper, this));
  }
  if (queue_.empty()) {
    condvar_.Signal();
  }
  queue_.push_back(key_and_meta);
  mutex_.Unlock();
}

void Index::Runner() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
  for (;;) {
    mutex_.Lock();
    for (;queue_.empty();) {
      condvar_.Wait();
    }
    assert(queue_.size() > 0);
    for (;!queue_.empty();) {
      auto key = queue_.front().key;
      auto value = queue_.front().meta;
      queue_.pop_front();
      Insert(key, value);
    }
    assert(queue_.size() == 0);
    mutex_.Unlock();
  }
#pragma clang diagnostic pop
}

void* Index::ThreadWrapper(void* index) {
  reinterpret_cast<Index*>(index)->Runner();
  return NULL;
}
void Index::AddQueue(std::deque<KeyAndMeta>& queue) {
  mutex_.Lock();
  assert(queue_.size() == 0);
  queue_.swap(queue);
  if (!bgstarted_) {
    bgstarted_ = true;
    port::PthreadCall("create thread", pthread_create(&thread_, NULL, &Index::ThreadWrapper, this));
  }
  condvar_.Signal();
  mutex_.Unlock();
}

} // namespace leveldb