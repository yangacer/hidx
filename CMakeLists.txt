cmake_minimum_required(VERSION 3.14)

project(hidx VERSION 0.1.0 LANGUAGES C)

add_subdirectory(src)
add_subdirectory(perf)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
  add_subdirectory(tests)
endif()

install(DIRECTORY hidx DESTINATION include)
