#ifndef STORAGE_LEVELDB_INDEX_NVM_BTREE_H_
#define STORAGE_LEVELDB_INDEX_NVM_BTREE_H_
#include <cassert>
#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <thread>
#include <sstream>
#include "index/btree.h"
#include "util/persist.h"

#define CAS(_p, _u, _v)  (__atomic_compare_exchange_n (_p, _u, _v, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE))
#define PAGESIZE (256)

using namespace std;

namespace leveldb {

class NVMBtree;

class Node;

struct LeafEntry {
  int64_t key;
  char* ptr;
};

struct InternalEntry {
  int64_t key;
  int32_t left;
  int32_t right;
  Node* lPtr;
  Node* rPtr;
};

class Node {
public:
  enum Type : int32_t {
    Leaf = 0, Internal = 1
  };

  Node(Type);

  Node(Type, int64_t);

  Node(Type, Node*);

  Node(Type, int64_t, Node*);

  virtual ~Node();

  void* operator new(size_t size) {
    void* ret;
    return posix_memalign(&ret, 64, size) == 0 ? ret : nullptr;
  }

  void operator delete(void* buffer) {
    free(buffer);
  }

  void print();

  void print(stringstream& ss);

#ifdef MULTITHREAD
  bool lock() {
    int32_t zero = 0;
    return CAS(&loc, &zero, 1);
  }
  bool unlock() {
    int32_t one = 1;
    return CAS(&loc, &one, 0);
  }
#endif

private:
  int64_t splitKey;
  Node* sibling;
  Type type;
#ifdef MULTITHREAD
  int32_t loc = 0;
#endif

  friend class NVMBtree;

  friend class iNode;

  friend class lNode;
};

struct Split {
  Node* original;
  Node* left;
  Node* right;
  int64_t splitKey;

  ~Split() {
    delete original;
  }
};


struct Merge {
  Node* _left; // Orignal
  Node* _right;
  Node* left;
  Node* right;
};

class lNode : public Node {
public:
  constexpr static int32_t CARDINALITY
      = (1024 - sizeof(Node)) / sizeof(LeafEntry);
  // = 2+4*10;
  // = 4;

  // Core
  lNode();

  ~lNode();

  void insert(int64_t, char*);

  void sInsert(int32_t, int64_t, char*);

  Split* split(int64_t, char*);

  Merge* merge(void);

  void remove(int64_t);

  char* search(int64_t);

  char* update(int64_t, char*);

  void* operator new(size_t size) {
    void* ret;
    return posix_memalign(&ret, 64, size) == 0 ? ret : nullptr;
  }

  void operator delete(void* buffer) {
    free(buffer);
  }


  inline LeafEntry& operator[](uint32_t idx) {
    return entry[idx];
  }

  // Helper
  bool overflow(void);

  int32_t count(void);

  // Debug
  int print();

  int print(stringstream&);

  void copy_debug(vector<int64_t>&);

  void sort();

private:
  array<LeafEntry, CARDINALITY> entry;
};

class iNode : public Node {
public:
  enum Direction : int32_t {
    None = -1, Left = 0, Right = 1
  };
  constexpr static int32_t CARDINALITY
      = (PAGESIZE - sizeof(Node) - sizeof(int32_t) * 3) / sizeof(InternalEntry);

  // Core
  iNode();

  iNode(Split*);

  ~iNode();

  bool overflow(void);

  int32_t insert(int64_t, Node*, Node*);

  int32_t sInsert(int64_t, Node*, Node*);

  Split* split(int64_t, Node*, Node*);

  void remove(int64_t, Node*);

  void remove(int32_t, int32_t, Node*, Node*);

  void update(int32_t, int32_t, Node*, Node*, Node*);

  Merge* merge(void);

  Node* search(int64_t);

  void* operator new(size_t size) {
    void* ret;
    return posix_memalign(&ret, 64, size) == 0 ? ret : nullptr;
  }

  void operator delete(void* buffer) {
    free(buffer);
  }

  // Helper
  Node* getLeftmostPtr(void);

  Node* getLeftmostPtr(int32_t);

  Node* getRightmostPtr(void);

  Node* getRightmostPtr(int32_t);

  LeafEntry* transform(int32_t&, Node*&);

  void transform(LeafEntry*, int32_t&, int32_t&, Node*&, bool);

  bool balancedInsert(LeafEntry*, int32_t, int32_t, int32_t&, Node*);

  void defragmentation(iNode* l, iNode* r, int16_t, int64_t);

  void block(int32_t, Direction);

  Node* getLeftSibling(int32_t, iNode::Direction);

  int32_t getParent(int32_t);

  int32_t getParent(Node*, Direction&);

  int32_t getCommonAncestor(int32_t);

  int32_t count(void);

  void rebalance(void);

  // Debug
  void print(void);

  void print(int32_t loc);

  void print(stringstream&);

  void print(int32_t loc, stringstream&);

  void copy_debug(vector<int64_t>&, queue<Node*>&, int32_t pos);

  int32_t getCnt(void) {
    return cnt;
  }


  // Test functions
  Node* test_getLeftSibling() {
    int32_t cur = root;
    while (entry[cur].left != -1) {
      cur = entry[cur].left;
    }
    return getLeftSibling(cur, iNode::Left);
  }

  Node* test_getLeftSibling2() {
    int32_t cur = root;
    while (entry[cur].right != -1) {
      cur = entry[cur].right;
    }
    return getLeftSibling(cur, iNode::Right);
  }

  int64_t test_getCommonAncestor() {
    int32_t cur = root;
    cur = entry[cur].right;
    cur = entry[cur].left;
    return entry[getCommonAncestor(cur)].key;
  }

  Node* test_block() {
    int32_t cur = root;
    cur = entry[cur].right;
    cur = entry[cur].left;
    block(cur, iNode::Right);
    return entry[cur].rPtr;
  }

  int64_t test_getParent() {
    int32_t cur = root;
    cur = entry[cur].left;
    cur = entry[cur].right;
    cur = entry[cur].left;
    return entry[getParent(cur)].key;
  }

  void test_remove() {
    Node* lSib = (Node*) 0x1;
    remove(10, 10, (Node*) 111, lSib);
    cout << (int64_t) lSib << endl;
    print();
    remove(9, 7, (Node*) 111, lSib);
    cout << (int64_t) lSib << endl;
    print();
  }

private:
  int32_t root;
  int32_t cnt;
  int32_t deleteCnt;
  array<InternalEntry, CARDINALITY> entry;

  friend class NVMBtree;
};

class NVMBtree : public Btree {
private:
  Node* root;
  struct timespec start, end;
  int64_t elapsed = 0;

public:
  NVMBtree();

  char* Search(int64_t);

  void Insert(int64_t, char*);

  void Remove(int64_t);

  vector<LeafEntry*> Range(int64_t, int64_t);

  // Helper
  iNode* findParent(Node*);

  Node* findLeftSibling(Node*);

  // Perf test
  void sort();

  void rebal();

  int64_t failedSearch;
};

}

#endif // STORAGE_LEVELDB_INDEX_NVM_BTREE_H_
