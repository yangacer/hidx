file(GLOB HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")

add_library(libhidx bucket.c hidx.c mhidx.c)

target_include_directories(libhidx PUBLIC ../include)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADERS})

install(TARGETS libhidx DESTINATION lib)
