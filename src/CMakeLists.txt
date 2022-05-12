file(GLOB HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/hidx/*.h")

add_library(libhidx bucket.c hidx.c )#mhidx.c)

target_include_directories(libhidx PUBLIC ${PROJECT_SOURCE_DIR})

source_group(
  TREE "${PROJECT_SOURCE_DIR}/hidx"
  PREFIX "Header Files"
  FILES ${HEADERS})

install(TARGETS libhidx DESTINATION lib)
