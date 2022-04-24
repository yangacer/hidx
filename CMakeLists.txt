cmake_minimum_required(VERSION 3.12)

project(hidx VERSION 0.1.0 LANGUAGES C)

add_subdirectory(include)
add_subdirectory(src)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
  add_subdirectory(tests)
endif()
