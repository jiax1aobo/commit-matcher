#include <fcntl.h> // open
#include <stdio.h> // printf
#include <unistd.h> // close

#include "matcher.h"
#include "tree_sitter/api.h"



LangFuncPair language_pair_array[LANGUAGE_MAX] = {
	[LANGUAGE_C] = {tree_sitter_c, check_comment_c},
	[LANGUAGE_JAVA] = {tree_sitter_java, check_comment_java},
	[LANGUAGE_PYTHON] = {tree_sitter_python, check_comment_python},
};

void pickup_node(TSNode node, int indent) {
	PUT_INDENT(indent);
	printf("is named node: %d\n", ts_node_is_named(node));
	PUT_INDENT(indent);
	printf("node symbol: %d\n", ts_node_symbol(node));
	PUT_INDENT(indent);
	printf("node type: %s\n", ts_node_type(node));
	
	int c_idx;
	// int c_cnt = ts_node_child_count(node);
	int c_cnt = ts_node_named_child_count(node);
	for (c_idx = 0; c_idx < c_cnt; c_idx++) {
		PUT_INDENT(indent);
		printf("field name of child (%d): %s\n", c_idx,
			ts_node_field_name_for_child(node, c_idx));
		// TSNode child = ts_node_child(node, c_idx);
		TSNode child = ts_node_named_child(node, c_idx);
		pickup_node(child, indent + 1);
	}
}

const TSLanguage *tree_sitter_c(void);
const TSLanguage *tree_sitter_cpp(void);
const TSLanguage *tree_sitter_java(void);
const TSLanguage *tree_sitter_python(void);

int main(int argc, char *argv[]) {
	TSParser *parser = ts_parser_new();
	// ts_parser_set_language(parser, tree_sitter_c());
	// ts_parser_set_language(parser, tree_sitter_cpp());
	// ts_parser_set_language(parser, tree_sitter_java());
	ts_parser_set_language(parser, tree_sitter_python());

	int fd = open(argv[1], 'r');

	TSInput input = {
		.payload = &fd,
		.read = default_read,
		.encoding = TSInputEncodingUTF8
	};

	TSTree *tree = ts_parser_parse(parser, NULL, input);
	TSNode root = ts_tree_root_node(tree);

	pickup_node(root, 0);
	
	close(fd);
	ts_parser_delete(parser);

	return 0;
}