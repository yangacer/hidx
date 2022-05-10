cmake_minimum_required(VERSION 3.14)

project(hidx_perf LANGUAGES CXX)

# find_library(TCMALLOC libtcmalloc_minimal.so.4 PATHS /usr/lib/x86_64-linux-gnu )
include(FetchContent)

FetchContent_Declare(
  gperftools
  GIT_REPOSITORY https://github.com/gperftools/gperftools.git
  GIT_TAG gperftools-2.9
)
FetchContent_MakeAvailable(gperftools)

add_executable(hidx_perf perf.cc)
target_link_libraries(hidx_perf libhidx tcmalloc_minimal)
# target_link_libraries(hidx_perf libhidx ${TCMALLOC})

target_include_directories(hidx_perf PRIVATE ../)
