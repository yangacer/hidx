cmake_minimum_required(VERSION 3.12)

project(hidx_perf LANGUAGES CXX)

add_executable(hidx_perf perf.cc)
target_link_libraries(hidx_perf libhidx)
target_include_directories(hidx_perf PRIVATE ../include)
