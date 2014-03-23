#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <regex.h>
#include <libgen.h>
#include <string.h>

#include <cdb.h>

#include "database.h"

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)

void check_access_flags(const char*, int, int);
int check_access_flags_v(const char*, int, int);
void compile_regex(regex_t*, const char*, int);
char* get_basename(char*);
char* pathcat(const char*, const char*);
void helper_manage_stackheapbuf(char*, size_t*, int*,
    unsigned);
char* strip_tag(const char*);
int tag_pos(char*, char**, char**);
char* get_tag(char*);

#endif /* UTIL_H */
