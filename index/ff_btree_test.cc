#include <iostream>
#include <unordered_map>
#include <ctime>
#include <sys/time.h>
#include "ff_btree.h"
#include "util/logging.h"
#include "util/testharness.h"
#include "ff_btree_iterator.h"

namespace leveldb {

class FFBtreeTest{};

TEST(FFBtreeTest, DuplicateKeys) {
  FFBtree btree;
 /*for (int64_t i = 1; i < 100; i++) {
    btree.Insert(i, (char*) i);
  }
  std::unordered_map<int64_t, char*> map;
  // rewrite values
  for (int64_t i = 1; i < 100; i++) {
    int64_t j = i*2;
    btree.Insert(i, (char*) j);
    map.insert({i, (char*) j});
  }
  for (int64_t i = 1; i < 100; i++) {
    uint64_t val1 = reinterpret_cast<uint64_t>(btree.Search(i));
    uint64_t val2 = reinterpret_cast<uint64_t>(map.at(i));
    ASSERT_EQ(val1, val2);
  }*/
  for (int64_t i = 1; i<1000; i++) {
	char* s = new char[to_string(i).length()+1];
	strcpy(s, to_string(i+1000).c_str());
	btree.Insert(i, s);
  }
  cout << "start itr" << endl;
  FFBtreeIterator itr = FFBtreeIterator((char*)&btree);
  for(int i=0; i<400; i++) itr.Next();
  if(itr.value().data() != NULL) cout << "[TEST]" << itr.key().data() << " " << itr.value().data() << endl;
  itr.Seek(Slice("999",3));
  if(itr.value().data() != NULL) cout << "[TEST]" << itr.key().data() << " " << itr.value().data() << endl;
  cout << "end itr" << endl;
}

TEST(FFBtreeTest, BigTest) {
  int n = 10000000;
  Random rand(time(0));
  FFBtree btree;
  std::unordered_map<int64_t, char*> map;
  for (int64_t i = 1; i < n; i++) {
    int k = rand.Next() % n;
    int v = k;
    btree.Insert(k, (char*) v);
    map.insert_or_assign(k, (char*) v);
  }
  // rewrite values
  for (int64_t i = 1; i < n; i++) {
    int k = rand.Next() % n;
    uint64_t val1 = 0, val2 = 0;
    try {
      val1 = reinterpret_cast<uint64_t>(btree.Search(k));
      val2 = reinterpret_cast<uint64_t>(map.at(k));
    } catch (std::exception& e) {
    }
    ASSERT_EQ(val1, val2);
    int v = k * 2;
    btree.Insert(k, (char*) v);
    map.insert_or_assign(k, (char*) v);
  }
  // rewrite values
  for (int64_t i = 1; i < n; i++) {
    int k = rand.Next() % n;
    uint64_t val1 = 0, val2 = 0;
    try {
      val1 = reinterpret_cast<uint64_t>(btree.Search(k));
      val2 = reinterpret_cast<uint64_t>(map.at(k));
    } catch (std::exception& e) {
    }
    ASSERT_EQ(val1, val2);
    int v = rand.Next();
    btree.Insert(k, (char*) v);
    map.insert_or_assign(k, (char*) v);
  }
  for (int64_t i = 1; i < n; i++) {
    int k = rand.Next() % n;
    uint64_t val1 = 0, val2 = 0;
    try {
      val1 = reinterpret_cast<uint64_t>(btree.Search(k));
      val2 = reinterpret_cast<uint64_t>(map.at(k));
    } catch (std::exception& e) {
    }
    ASSERT_EQ(val1, val2);
  }
}

} // namespace leveldb


int main(int argc, char** argv) {
  return leveldb::test::RunAllTests();
}
