
DIR_BIN = ./bin
DIR_SRC = ./src
DIR_DEPS = ./deps
DIR_TS = $(DIR_DEPS)/tree-sitter
DIR_TS_C = $(DIR_DEPS)/tree-sitter-c
DIR_TS_JSON = $(DIR_DEPS)/tree-sitter-json

INC = $(DIR_TS)/lib/include
LIB = $(DIR_TS)/libtree-sitter.a

PARSER_JSON = $(DIR_TS_JSON)/src/parser.c
MATCHER_JSON = $(DIR_SRC)/json-matcher.c

json-matcher: $(MATCHER_JSON) $(PARSER_JSON)
	clang -I $(INC) $^ $(LIB) -o $(DIR_BIN)/$@

PARSER_C = $(DIR_TS_C)/src/parser.c
MATCHER_C = $(DIR_SRC)/c-matcher.c

c-matcher: $(MATCHER_C) $(PARSER_C)
	clang -I $(INC) $^ $(LIB) -o $(DIR_BIN)/$@

clean:
	$(RM) -f $(DIR_BIN)/*