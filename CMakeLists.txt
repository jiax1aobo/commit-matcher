cmake_minimum_required(VERSION 3.22)

project("Comment Matcher"
        VERSION 1.0
        DESCRIPTION "This is a comment matcher by tree-sitter"
        LANGUAGES C
)

add_subdirectory(src)

set(TREE_SITTER_INC "${CMAKE_SOURCE_DIR}/deps/tree-sitter/lib/include")

set(TREE_SITTER "${CMAKE_SOURCE_DIR}/deps/tree-sitter/libtree-sitter.a")
set(TREE_SITTER_C "${CMAKE_SOURCE_DIR}/deps/tree-sitter-c/libtree-sitter-c.a")
set(TREE_SITTER_JAVA "${CMAKE_SOURCE_DIR}/deps/tree-sitter-java/libtree-sitter-java.a")
set(TREE_SITTER_PY "${CMAKE_SOURCE_DIR}/deps/tree-sitter-python/libtree-sitter-python.a")


