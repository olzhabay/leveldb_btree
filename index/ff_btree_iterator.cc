#include "ff_btree_iterator.h"
#include "util/coding.h"

namespace leveldb {

FFBtreeIterator::FFBtreeIterator(FFBtree* b) {
  btree = b;
  SeekToFirst();
}

bool FFBtreeIterator::Valid() const {
  return valid;
}

void FFBtreeIterator::SeekToFirst() {
	Page* page = (Page*)btree->root;
	while(page->hdr.leftmost_ptr != NULL) {
		page = page->hdr.leftmost_ptr;
	}
	cur_page = page;
	cur = &(page->records[0]);
	index = 0;
  valid = cur != NULL;
}

void FFBtreeIterator::SeekToLast() {
	Page* page = (Page*)btree->root;
	while(page->hdr.leftmost_ptr != NULL) {
		page = page->hdr.leftmost_ptr;
	}
	while(page->hdr.sibling_ptr != NULL) {
		page = page->hdr.sibling_ptr;
	}
	cur_page = page;
	cur = &(cur_page->records[cur_page->count()-1]);
	index = cur_page->count()-1;
  valid = cur != NULL;
}

void FFBtreeIterator::Seek(const Slice& target) {
	uint64_t k = fast_atoi(target.data(),target.size());
	Page* page = (Page*)btree->root;
	// search until leaf node
	while(page->hdr.leftmost_ptr!=NULL) {
		page = (Page*)page->linear_search(k);
	}
	// TODO: Fix
	Page* t;
	while((t = (Page*)page->linear_search_entry(k)) == page->hdr.sibling_ptr) {
		page = t;
		if(!page) break;
	}
	cur_page = t;
  valid = cur != nullptr;
}

void FFBtreeIterator::Next() {
	if(index == cur_page->count()-1) {
		if(cur_page->hdr.sibling_ptr != NULL) {
			cur_page = cur_page->hdr.sibling_ptr;
			cur = &(cur_page->records[0]);
			index = 0;
		}
	} else {
		index = index+1;
		cur = &(cur_page->records[index]);
	}
}

void FFBtreeIterator::Prev() {
	if(index == 0) {
	} else {
		index = index-1;
		cur = &(cur_page->records[index]);
	}
}

int64_t FFBtreeIterator::key() const {
  if(Valid()) {
    return cur->key;
  } else {
    return -1;
  }
}

void* FFBtreeIterator::value() const {
  if(Valid()) {
	  return cur->ptr;
  } else {
	  return nullptr;
  }
}

} // namespace leveldb