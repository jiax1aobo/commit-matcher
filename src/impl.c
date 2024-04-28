#include <assert.h> // assert
#include <stdint.h>
#include <stdio.h> // perror()
#include <unistd.h> // lseek, read
#include <fcntl.h> // open
#include "matcher.h"

/**
 * Default Read Function
 */

static char src_read_buff[SRC_READ_BUFF_SIZE];
const char *default_read(void *p_fd, uint32_t b_off, TSPoint pos, uint32_t *b_rd) {
    int rdnb = 0;
    int fd = *(int *)p_fd;
    src_read_buff[0] = '\0';

    lseek(fd, b_off, SEEK_SET);

    rdnb = read(fd, src_read_buff, SRC_READ_BUFF_SIZE);

    if (rdnb == 0) {
        *b_rd = 0;
        THROW(error);
    } else if (rdnb > 0) {
        assert(rdnb <= SRC_READ_BUFF_SIZE);
        src_read_buff[rdnb] = '\0';
        *b_rd = rdnb;
    } else {
        perror("src_read()");
        THROW(error);
    }

    return src_read_buff;
    
    LAND(error);

    return NULL;
}

/**
 * Check Comment Function 
 */

int check_comment_dummy(TSNode self) { return 0; }

int check_comment_c(TSNode self) {
    return (ts_node_symbol(self) == 154) ? 1 : 0;
}

int check_comment_cpp(TSNode self) {
    return (ts_node_symbol(self) == 170) ? 1 : 0;
}

int check_comment_java(TSNode self) {
    TSSymbol sym = ts_node_symbol(self);
    return (sym == 136 || sym == 137) ? 1 : 0;
}

int check_comment_python(TSNode self) {
    TSSymbol sym = ts_node_symbol(self);
    if (sym == 99) {
        return 1;
    } else if (sym == 122) {
        TSNode sub_node = ts_node_named_child(self, 0);
        TSSymbol sub_sym = ts_node_symbol(sub_node);
        if (sub_sym == 230) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

/**
 * Pickup Function 
 */

static char pickup_piece_buff[PICKUP_PIECE_SIZE];
void pickup_comment_print(TSNode self, TSInput input) {
    int fd = *(int *)input.payload;

    if (language_pair_array[opt_lang].comment_check(self) == 1) {
        uint32_t start = ts_node_start_byte(self);
        uint32_t end = ts_node_end_byte(self);
        ssize_t len = end - start;
        off_t curr_off = lseek(fd, 0, SEEK_CUR);
        lseek(fd, start, SEEK_SET);
        read(fd, pickup_piece_buff, len);
        lseek(fd, curr_off, SEEK_SET);
        pickup_piece_buff[len] = '\0';
        fprintf(stdout, PRINT_FORMAT, start, end, pickup_piece_buff);
    }

    int c_cnt = ts_node_named_child_count(self);
    int c_idx;
    for (c_idx = 0; c_idx < c_cnt; c_idx++) {
        TSNode child = ts_node_named_child(self, c_idx);
        pickup_comment_print(child, input);
    }
}

/**
 * Parse File Function
 */

void do_parse_file(ParseMode mode, LanguageType lang, const char *path) {
    int stat = 0;
    int fd = open(path, O_RDONLY);
    TRY_THROW(fd > 0, ERR_CALL_OPEN);
    stat = 1;

    TSParser *parser = ts_parser_new();
    TRY_THROW(parser != NULL, ERR_CALL_PARSE);
    stat = 2;

    TRY_THROW(ts_parser_set_language(parser,
        language_pair_array[opt_lang].language()),
        ERR_SET_LANGUAGE);

    TSInput input = {
        .payload = &fd,
        .read = default_read,
        .encoding = TSInputEncodingUTF8
    };

    TSTree *tree = ts_parser_parse(parser, NULL, input);
    TRY_THROW(tree != NULL, ERR_PARSE_TREE);
    stat = 3;

    TSNode root = ts_tree_root_node(tree);

    pickup_comment_print(root, input);

    CATCH(ERR_CALL_OPEN) {
        fprintf(stderr, "error opening file\n");
    }

    CATCH(ERR_CALL_PARSE) {
        fprintf(stderr, "error creating parser\n");
    }

    CATCH(ERR_SET_LANGUAGE) {
        fprintf(stderr, "error setting language\n");
    }

    CATCH(ERR_PARSE_TREE) {
        fprintf(stderr, "error getting parse tree\n");
    }

    FINISH;

    switch (stat) {
        case 3:
            ts_tree_delete(tree);
        case 2:
            ts_parser_delete(parser);
        case 1:
            close(fd);
        case 0:
            break;
        default:
            assert(0);
    }
}

#define SINGLE_SPACE_STRING " "
#define DOUBLE_SPACE_STRING SINGLE_SPACE_STRING SINGLE_SPACE_STRING
#define DEFAULT_INDENT_PLACEHOLDER DOUBLE_SPACE_STRING DOUBLE_SPACE_STRING 
#define PUT_INDENT(_indent_num) do { \
    int indent_idx; \
    for (indent_idx = 0; indent_idx < _indent_num; indent_idx++) { \
        printf("%s", DEFAULT_INDENT_PLACEHOLDER); \
    } \
} while(0)