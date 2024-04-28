# include_directories(${TREE_SITTER_INC})
# message(STATUS ${TREE_SITTER_INC})
include_directories(/Users/jiaxiaobo/Downloads/match-comment/deps/tree-sitter/lib/include)
add_executable(main main.c)

target_link_libraries(main
    ${TREE_SITTER}
    ${TREE_SITTER_C}
    ${TREE_SITTER_JAVA}
    ${TREE_SITTER_PY}
)

