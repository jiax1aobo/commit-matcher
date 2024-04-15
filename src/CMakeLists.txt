add_subdirectory(./matcher)

include_directories(./lib)

add_executable(main main.c)

set(MATCHER_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(main "${MATCHER_LIB}/libmatcher.a")

