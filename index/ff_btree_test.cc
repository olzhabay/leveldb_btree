#include <unordered_map>
#include "ff_btree.h"
#include "util/logging.h"
#include "util/testharness.h"

namespace leveldb {

class FFBtreeTest{};

TEST(FFBtreeTest, DuplicateKeys) {
  FFBtree btree;
  for (int64_t i = 1; i < 100; i++) {
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
    char* j = btree.Search(i);
    ASSERT_EQ(j, map.at(i));
  }
}

} // namespace leveldb


int main(int argc, char** argv) {
  return leveldb::test::RunAllTests();
}