#include <cstdio>
#include <sys/time.h>
#include <cstdint>
#include "perf_log.h"

namespace leveldb {

static FILE* version_log;
static FILE* block_log;
static FILE* reader_f_log;
static FILE* reader_m_log;

void logMicro(Type type, uint64_t micro) {
  switch (type) {
    case VERSION:
      fprintf(version_log, "%lu\n", micro);
      break;
    case BLOCK:
      fprintf(block_log, "%lu\n", micro);
      break;
    case READER_F:
      fprintf(reader_f_log, "%lu\n", micro);
      break;
    case READER_M:
      fprintf(reader_m_log, "%lu\n", micro);
      break;
  }
}

uint64_t NowMicros() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

void createPerfLog() {
  version_log = fopen("version_log", "w");
  block_log = fopen("block_log", "w");
  reader_f_log = fopen("reader_f_log", "w");
  reader_m_log = fopen("reader_m_log", "w");
}


void closePerfLog() {
  fclose(version_log);
  fclose(block_log);
  fclose(reader_f_log);
  fclose(reader_m_log);
}


}
