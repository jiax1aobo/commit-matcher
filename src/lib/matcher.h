#ifndef _MATCHER_H_
#define _MATCHER_H_

#include "api.h"

#define TRY(_expr) do { \
    if(!(_expr)) { \
        goto FINISH_POINT; \
    } \
} while(0)

#define TRY_THROW(_expr, _where) do { \
    if(!(_expr)) { \
        goto _where; \
    } \
} while(0)

#define CATCH(_where) \
    goto FINISH_POINT;\
    _where:

#define THROW(_where) goto _where

#define LAND(_where) _where:

#define FINISH goto FINISH_POINT; \
    FINISH_POINT:

/*==========================*/
typedef enum LanguageType {
    LANGUAGE_C,
    LANGUAGE_JAVA,
    LANGUAGE_PYTHON,
    LANGUAGE_MAX,
} LanguageType;

typedef const TSLanguage *(*language_func)(void);
typedef int (*comment_check_func)(TSNode node);
typedef struct LangFuncPair {
    language_func language;
    comment_check_func comment_check;
} LangFuncPair;
extern LangFuncPair language_pair_array[]; // to be declare by user

/* Tree-Sitter Language Function */
const TSLanguage *tree_sitter_c(void);
const TSLanguage *tree_sitter_java(void);
const TSLanguage *tree_sitter_python(void);

/* Comment Pick Function */
int check_comment_c(TSNode node);
int check_comment_java(TSNode node);
int check_comment_python(TSNode node);
/*==========================*/

#define SRC_READ_BUFF_SIZE (1024 * 2)
const char *default_read(void *p_fd, uint32_t b_off, TSPoint pos, uint32_t *b_rd);

#define PRINT_FORMAT "{\n" \
                     "    \"start\": %d\n" \
                     "    \"end\": %d\n" \
                     "    \"text\": \"%s\"\n" \
                     "},\n"
#define PICKUP_PIECE_SIZE (1024 * 40)
void pickup_comment_print(TSNode self, TSInput input);

/* Option Parsing */
typedef enum ParseMode {
    MODE_PARSE,
    MODE_SWAP
} ParseMode;
#define OPT_PATH_MAX 256
extern LanguageType opt_lang;
extern ParseMode opt_mode;
extern char opt_path[OPT_PATH_MAX];

void do_parse_file(ParseMode mode, LanguageType lang, const char *path);


#endif // _MATCHER_H_