#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h> // getopt_long
#include <fcntl.h> // open
#include <string.h> // strcasecmp strncpy


#include "matcher.h"

static char *optstr = ":m:l:p:h";
struct option longopt[] = {
    {"mode", required_argument, NULL, 'm'},
    {"language", required_argument, NULL, 'l'},
    {"path", required_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};

ParseMode opt_mode;
LanguageType opt_lang;
char opt_path[OPT_PATH_MAX];

int parse_options(int argc, char *argv[]);
void print_usage(FILE *fd, char *program);

int main(int argc, char *argv[]) {
    TRY_THROW(parse_options(argc, argv) == 0, ERR_USAGE);

    if (opt_mode == MODE_PARSE) {
        do_parse_file(opt_mode, opt_lang, opt_path);
    } else {
        assert(opt_mode == MODE_SWAP);
        /* TODO: implement swap action */
    }

    return 0;

    CATCH(ERR_USAGE) {
        print_usage(stderr, argv[0]);
    }

    FINISH;

    return -1;
}

int parse_options(int argc, char *argv[]) {
    int hasHelp = 0;
    int hasPath = 0;
    opt_mode = MODE_PARSE;
    opt_lang = LANGUAGE_C;

    TRY_THROW(argc > 1, FINISH_POINT);

    int ret;
    while ((ret = getopt_long(argc, argv, optstr, longopt, NULL)) != -1) {
        switch (ret) {
            case 'm':
                TRY_THROW(hasHelp == 0,  FINISH_POINT);
                if (strcasecmp(optarg, "C") == 0) {
                    opt_lang = LANGUAGE_C;
                } else if (strcasecmp(optarg, "Java") == 0) {
                    opt_lang = LANGUAGE_JAVA;
                } else if (strcasecmp(optarg, "Python") == 0) {
                    opt_lang = LANGUAGE_PYTHON;
                } else {
                    THROW(ERR_UNSUPPORTED_LANGUAGE);
                }
                break;
            case 'l':
                TRY_THROW(hasHelp == 0, FINISH_POINT);
                if (optarg[0] == 'P' || optarg[0] == 'p') {
                    opt_mode = MODE_PARSE;
                } else if (optarg[0] == 'S' || optarg[0] == 's') {
                    opt_mode = MODE_SWAP;
                } else {
                    THROW(ERR_UNSUPPORTED_MODE);
                }
                break;
            case 't':
                TRY_THROW(hasHelp == 0, FINISH_POINT);
                hasPath = 1;
                strncpy(opt_path, optarg, OPT_PATH_MAX);
                break;
            case 'h':
                TRY_THROW(argc == 2, ERR_HELP_WITH_ARGS);
                hasHelp = 1;
                print_usage(stdout, argv[0]);
                exit(EXIT_FAILURE);
                break;
            case ':':
            case '?':
                THROW(ERR_UNEXPECTED_OPT_ARGS_REQUIRED);
                break;
            default:
                assert(0);
        }
    }

    return 0;

    CATCH(ERR_UNSUPPORTED_LANGUAGE) {
        fprintf(stderr, "unsupported language\n");
    }

    CATCH(ERR_UNSUPPORTED_MODE) {
        fprintf(stderr, "unsupported mode\n");
    }

    CATCH(ERR_HELP_WITH_ARGS) {
        fprintf(stderr, "help option doesn't need argument\n");
    }

    CATCH(ERR_UNEXPECTED_OPT_ARGS_REQUIRED) {
        fprintf(stderr, "unexpected option received or argument required\n");
    }

    FINISH;

    print_usage(stderr, argv[0]);

    return -1;
}

void print_usage(FILE *fd, char *program) {
    fprintf(fd, "Usage: %s\n", program);
}