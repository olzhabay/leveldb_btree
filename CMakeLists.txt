cmake_minimum_required(VERSION 3.0)

project(leveldb_btree CXX)
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_CXX_STANDARD 17)

set(SNAPPY_LIBRARY "")

string(REGEX MATCH "clang" CLANG ${CMAKE_CXX_COMPILER})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(PMDK)
set(Stdcpp_LIBRARY stdc++)

if(CMAKE_COMPILER_IS_GNUCXX OR CLANG)
    find_library(Pthread_LIBRARY pthread)
    find_library(Realtime_LIBRARY rt)
    # find library can be problematic with stdc++ which is why we hardwire the link
    set(Stdcpp_LIBRARY stdc++)
else(CMAKE_COMPILER_IS_GNUCXX OR CLANG)
    set(Pthread_LIBRARY "")
    set(Realtime_LIBRARY "")
    set(Stdcpp_LIBRARY "")
endif(CMAKE_COMPILER_IS_GNUCXX OR CLANG)

include_directories(${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
        include)

if(MSVC)
    add_compile_options(
            /D_CRT_SECURE_NO_WARNINGS
            /wd4389 # signed/unsigned mismatch
            /wd4800 # constructor never returns, potential memory leak because of a singleton pattern
            /wd4722 # unreachable code because of singleton pattern
            /wd4702 # bool cast performance warning
    )
else()
    add_compile_options(
            -Wno-sign-compare
            -std=c++17
            -O2
            -DNDEBUG
    )
endif()

add_definitions(-DLEVELDB_ATOMIC_PRESENT)
add_definitions(-DOS_LINUX)
add_definitions(-DLEVELDB_PLATFORM_POSIX)
add_definitions(-fno-builtin-memcmp)
add_definitions(-Wint-to-pointer-cast)
add_definitions(-DPERF_LOG=1)

set(LEVEL_DB_FILES
        include/leveldb/c.h
        include/leveldb/cache.h
        include/leveldb/comparator.h
        include/leveldb/db.h
        include/leveldb/dumpfile.h
        include/leveldb/env.h
        include/leveldb/iterator.h
        include/leveldb/filter_policy.h
        include/leveldb/iterator.h
        include/leveldb/options.h
        include/leveldb/slice.h
        include/leveldb/status.h
        include/leveldb/table.h
        include/leveldb/table_builder.h
        include/leveldb/write_batch.h
        include/leveldb/export.h
        include/leveldb/index.h
        include/leveldb/persistant_pool.h
        db/c.cc
        db/builder.cc
        db/builder.h
        db/db_impl.cc
        db/db_impl.h
        db/db_iter.cc
        db/db_iter.h
        db/dbformat.cc
        db/dbformat.h
        db/dumpfile.cc
        db/filename.cc
        db/filename.h
        db/log_format.h
        db/log_reader.cc
        db/log_reader.h
        db/log_writer.cc
        db/log_writer.h
        db/mock_log.h
        db/skiplist.h
        db/snapshot.h
        db/memtable.cc
        db/memtable.h
        db/repair.cc
        db/table_cache.cc
        db/table_cache.h
        db/version_edit.cc
        db/version_edit.h
        db/version_set.cc
        db/version_set.h
        db/write_batch.cc
        db/write_batch_internal.h
        db/logger.cc
        db/logger.h
        table/block.cc
        table/block.h
        table/block_builder.cc
        table/block_builder.h
        table/filter_block.cc
        table/filter_block.h
        table/format.cc
        table/format.h
        table/iterator.cc
        table/iterator_wrapper.h
        table/merger.cc
        table/merger.h
        table/table.cc
        table/table_builder.cc
        table/two_level_iterator.cc
        table/two_level_iterator.h
        util/arena.cc
        util/arena.h
        util/bloom.cc
        util/cache.cc
        util/coding.cc
        util/coding.h
        util/comparator.cc
        util/crc32c.cc
        util/crc32c.h
        util/env.cc
        util/filter_policy.cc
        util/hash.cc
        util/hash.h
        util/histogram.cc
        util/histogram.h
        util/logging.cc
        util/logging.h
        util/mutexlock.h
        util/options.cc
        util/random.h
        util/status.cc
        util/persist.h
        util/testharness.h
        util/testharness.cc
        util/testutil.h
        util/testutil.cc
        util/perf_log.cc
        util/perf_log.h
        util/persistant_pool.cc
        port/port.h
        port/atomic_pointer.h
        index/btree_index.cc
        index/btree_index.h
        index/index_iterator.cc
        index/index_iterator.h
        index/ff_btree.cc
        index/ff_btree.h
        index/ff_btree_iterator.cc
        index/ff_btree_iterator.h
        index/index.cc)

if(WIN32)
    list(APPEND LEVEL_DB_FILES
            port/port_win.h
            port/port_win.cc
            util/win_logger.h
            util/win_logger.cc
            util/env_boost.cc)
else()
    list(APPEND LEVEL_DB_FILES
            port/port_posix.h
            port/port_posix.cc
            util/posix_logger.h
            util/env_posix.cc)
endif()

add_library(leveldb ${LEVEL_DB_FILES})

target_include_directories(leveldb
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )

set_target_properties(leveldb PROPERTIES PUBLIC_HEADER
        "include/leveldb/c.h;
        include/leveldb/cache.h;
        include/leveldb/comparator.h;
        include/leveldb/db.h;
        include/leveldb/dumpfile.h;
        include/leveldb/env.h;
        include/leveldb/iterator.h;
        include/leveldb/filter_policy.h;
        include/leveldb/iterator.h;
        include/leveldb/options.h;
        include/leveldb/slice.h;
        include/leveldb/status.h;
        include/leveldb/table.h;
        include/leveldb/table_builder.h;
        include/leveldb/write_batch.h;
        include/leveldb/export.h;
        include/leveldb/index.h;
        include/leveldb/persistant_pool.h")

target_link_libraries(leveldb
        PRIVATE
        ${Pthread_LIBRARY}
        ${PMEM_LIBRARY}
        ${PMEMCTO_LIBRARY}
        ${PMEMOBJ_LIBRARY}
        ${PMEMLOG_LIBRARY}
        ${VMEM_LIBRARY}
        )

INSTALL(TARGETS leveldb ARCHIVE DESTINATION /usr/local/lib PUBLIC_HEADER DESTINATION /usr/local/include/leveldb)

add_executable(db_bench db/db_bench.cc)
target_link_libraries(db_bench PUBLIC leveldb)