#ifndef STORAGE_LEVELDB_UTIL_PERF_LOG_H
#define STORAGE_LEVELDB_UTIL_PERF_LOG_H

#include <cstdio>
#include <cstdint>
#include <sys/time.h>

namespace leveldb {

enum Type {
  INDEX = 0,
  VERSION = 1,
  BLOCK = 2
};

extern void logMicro(Type, uint64_t);

extern uint64_t NowMicros();

extern void createPerfLog();

extern void closePerfLog();

}

#endif // STORAGE_LEVELDB_UTIL_PERF_LOG_H
