file(GLOB TESTS RELATIVE "${PROJECT_SOURCE_DIR}/tests/" "*.c")

foreach(test_src ${TESTS})
  string(REPLACE ".c" "" test_name ${test_src})
  add_executable(${test_name} ${test_src})
  target_link_libraries(${test_name} libhidx)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

