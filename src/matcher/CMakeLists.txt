add_library(matcher impl.c)

target_link_libraries(matcher
    "${TS_LIB}/libtree-sitter.a"
    "${TS_LIB}/libtree-sitter-c.a"
    "${TS_LIB}/libtree-sitter-java.a"
    "${TS_LIB}/libtree-sitter-python.a"
)