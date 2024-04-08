
#include "tree_sitter/api.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

const TSLanguage *tree_sitter_c(void);

#define BUFF_MAX_SIZE 1024
char g_buff[BUFF_MAX_SIZE + 1];

const char *src_read(void *payload, uint32_t byte_index,
                    TSPoint position, uint32_t *bytes_read) {
    int rdnb = 0;
    int fd = *(int *)payload;
    g_buff[0] = '\0';

    lseek(fd, byte_index, SEEK_SET);

    rdnb = read(fd, g_buff, BUFF_MAX_SIZE);

    if (rdnb == 0) {
        *bytes_read = 0;
        goto error;
    } else if (rdnb > 0) {
        assert(rdnb <= BUFF_MAX_SIZE);
        g_buff[rdnb] = '\0';
        *bytes_read = rdnb;
    } else {
        perror("src_read()");
        goto error;
    }

    return g_buff;
    
error:
    return NULL;
}

char g_piece[BUFF_MAX_SIZE * 2 + 1];

void remove_comment(TSNode self, TSInput input) {
    int fd = *(int *)input.payload;
    const char *curr_type = ts_node_type(self);
    if (strcmp(curr_type, "comment") == 0) {
        uint32_t start = ts_node_start_byte(self);
        uint32_t end = ts_node_end_byte(self);
        off_t curr_off = lseek(fd, start, SEEK_SET);
        assert(curr_off != -1);
        size_t len = end - start;
        assert(len < BUFF_MAX_SIZE * 2);
        read(fd, g_piece, len);
        g_piece[len] = '\0';
        (void)lseek(fd, curr_off, SEEK_SET);
        printf("comment : %s\n", g_piece);
    }

    int child_cnt = ts_node_child_count(self);
    for (int i = 0; i < child_cnt; i++) {
        TSNode child = ts_node_child(self, i);
        remove_comment(child, input);
    }
}

int main(int argc, char **argv) {
    TSParser *parser = ts_parser_new();

    ts_parser_set_language(parser, tree_sitter_c());

    int src_fd;
    if (argc != 2) {
        goto fatal;
    } else {
        src_fd = open(argv[1], O_RDWR);
    }

    TSInput input = {
        .payload = &src_fd,
        .read = src_read,
        .encoding = TSInputEncodingUTF8
    };

    TSTree *tree = ts_parser_parse(parser, NULL, input);

    TSNode root = ts_tree_root_node(tree);
    const char *node_type = ts_node_type(root);
    printf("root node type: %s\n", node_type);

    remove_comment(root, input);

    return 0;
    
fatal:
    return -1;
}

/*
使用解析器解析一些源代码并创建语法树。

如果您是第一次解析此文档，请为“old_tree”参数传递“NULL”。
否则，如果您已经解析了此文档的早期版本并且该文档已被编辑，
请传递先前的语法树，以便可以重用其中未更改的部分。这将节省时间和内存。
为了使其正常工作，您必须已经使用 [`ts_tree_edit`]
函数以与源代码更改完全匹配的方式编辑了旧语法树。

[`TSInput`] 参数可让您指定如何读取文本。 它有以下三个字段：
1. [`read`]：用于检索给定字节偏移量和（行、列）位置处的文本块的函数。
    该函数应返回一个指向文本的指针，并将其长度写入 [`bytes_read`] 指针。
    解析器不拥有该缓冲区的所有权； 它只是借用它直到读完为止。
    该函数应向 [`bytes_read`] 指针写入零值以指示文档结尾。
2. [`payload`]：将传递给 [`read`] 函数的每次调用的任意指针。
3. [`encoding`]：指示文本的编码方式。 “TSInputEncodingUTF8”或“TSInputEncodingUTF16”。

该函数在成功时返回语法树，在失败时返回“NULL”。 失败的可能原因有以下三种：
1. 解析器没有指定语言。 使用 [`ts_parser_language`] 函数检查这一点。
2. 由于先前调用 [`ts_parser_set_timeout_micros`] 函数设置了超时，解析被取消。
    您可以通过使用相同的参数再次调用 [`ts_parser_parse`] 从解析器遗漏的位置恢复解析。
    或者您可以通过首先调用 [`ts_parser_reset`] 从头开始解析。
3. 使用先前调用 [`ts_parser_set_cancellation_flag`] 设置的取消标志取消解析。
    您可以通过使用相同的参数再次调用 [`ts_parser_parse`] 从解析器遗漏的位置恢复解析。
*/