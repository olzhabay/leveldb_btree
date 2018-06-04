#include <iostream>
#include <stdlib.h>
#include "util/coding.h"
#include "ff_btree_iterator.h"

namespace leveldb {

FFBtreeIterator::FFBtreeIterator(char* b) {
  btree = b;
  SeekToFirst();
}

bool FFBtreeIterator::Valid() const {
  return valid;
}

void FFBtreeIterator::SeekToFirst() {
	FFBtree* b = (FFBtree*)btree;
	char* r = b->root;
	Page* p = (Page*)r;
	while(p->hdr.leftmost_ptr != NULL) {
		p = p->hdr.leftmost_ptr;
	}
	cur_page = (char*)p;
	cur = (char*)&(p->records[0]);
	index = 0; 
	if(cur == NULL) valid = false;
	else valid = true;
}

void FFBtreeIterator::SeekToLast() {
	FFBtree* b = (FFBtree*)btree;
	char* r = b->root;
	Page* p = (Page*)r;
	while(p->hdr.leftmost_ptr != NULL) {
		p = p->hdr.leftmost_ptr;
	}
	while(p->hdr.sibling_ptr != NULL) {
		p = p->hdr.sibling_ptr;
	}
	cur_page = (char*)p;
	cur = (char*)&(p->records[p->count()-1]);
	index = p->count()-1;
	if(cur == NULL) valid = false;
	else valid = true; 
}

void FFBtreeIterator::Seek(const Slice& target) {
	uint64_t k = fast_atoi(target.data(),target.size());
	FFBtree* b = (FFBtree*)btree;
	Page* p = (Page*)b->root;
	while(p->hdr.leftmost_ptr!=NULL) {
		p = (Page*)p->linear_search(k); 
	}

	Page* t;
	while((t = (Page*)p->linear_search_entry(k)) == p->hdr.sibling_ptr) {
		p = t;
		if(!p) break;
	}
	cur = (char*)t;
	if(cur == NULL) valid = false;
	else valid = true;
}

void FFBtreeIterator::Next() {
	if(index == ((Page*)cur_page)->count()-1) {
		if(((Page*)cur_page)->hdr.sibling_ptr != NULL) {
			cur_page = (char*)((Page*)cur_page)->hdr.sibling_ptr;
			cur = (char*)&(((Page*)cur_page)->records[0]);
			index = 0;
		}
	} else {
		index = index+1;
		cur = (char*)&((Page*)cur_page)->records[index];
	}
}

void FFBtreeIterator::Prev() {
	if(index == 0) {
	} else {
		index = index-1;
		cur = (char*)&((Page*)cur_page)->records[index];
	}
}

Slice FFBtreeIterator::key() const {
  Slice key;
  if(Valid()) {
	Entry* c = (Entry*)cur;
	char* k = (char*)to_string(c->key).c_str();
 	key = Slice(k,strlen(k));
  }
  else {
	key = Slice(NULL,0);
  }
  return key;
}

Slice FFBtreeIterator::value() const {
  Slice val;
  if(Valid()) {
	  Entry* c = (Entry*)cur;
	  val = Slice(c->ptr,strlen(c->ptr));
  } else {
	  val = Slice(NULL, 0);
  }
  return val;
}

Status FFBtreeIterator::status() const {
	return Status();
}

}
