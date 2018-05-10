#include <cstdio>
#include <sys/time.h>
#include <cstdint>
#include "perf_log.h"

namespace leveldb {

static FILE* index_log;
static FILE* version_log;
static FILE* block_log;

void logMicro(Type type, uint64_t micro) {
  switch (type) {
    case INDEX:
      fprintf(index_log, "%llu\n", micro);
      break;
    case VERSION:
      fprintf(version_log, "%llu\n", micro);
      break;
    case BLOCK:
      fprintf(block_log, "%llu\n", micro);
      break;
  }
}

uint64_t NowMicros() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

void createPerfLog() {
  index_log = fopen("index_log", "w");
  version_log = fopen("version_log", "w");
  block_log = fopen("block_log", "w");
}


void closePerfLog() {
  fclose(index_log);
  fclose(version_log);
  fclose(block_log);
}


}
