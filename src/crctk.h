/*
 * crctk - CRC32 Hexstring Toolkit
 * Copyright (C) 2014 2ion (asterisk!2ion!de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef CRCTK_H
#define CRCTK_H

#include <assert.h>
#include <cdb.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include "realpath.h"

#define _GNU_SOURCE
#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)
#ifndef VERSION
#define VERSION "unknown"
#endif
#define COPY_DB_STATIC_BUF_LEN 255
#define DBITEM_PATHBUFLEN 255
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"

/* GLOBALS */

const char *crcregex = "[[:xdigit:]]\\{8\\}";
const char *crcregex_stripper =
  "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?";
const char *dbiofile = "crcsums.tdb";
const char *hexarg = "00000000";

/* TYPES */

enum {
  TAG_ALLOW_STRIP = 1 << 0,
  CALC_PRINT_NUMERICAL = 1 << 1,
  APPEND_TO_DB = 1 << 2,
  CHECK_BATCH_PREFER_HEXSTRING = 1 << 3
};

struct DBItem {
  char *kbuf;
  int kbuflen;
  unsigned long crc;
  struct DBItem *next;
};

typedef int     // program exit status
(*CommandFunction)(
        int,    // argc
        char**, // argv
        int,    // optind
        int);   // cmdflags bitmask

/* PROTOTYPES */

static unsigned long getFileSize(const char*);
static unsigned long computeCRC32(const char*);

// argc, argv, optind, cmdflags
static int command_calc(int, char**, int, int);
static int command_calc_batch(int, char**, int, int);
static int command_check(int, char**, int, int);
static int command_check_batch(int, char**, int, int);
static int command_check_hexstring(int, char**, int, int);
static int command_help(int, char**, int, int);
static int command_idle(int, char**, int, int);
static int command_list_db(int, char**, int, int);
static int command_remove_tag(int, char**, int, int);
static int command_tag(int, char**, int, int);

static void check_access_flags(const char*, int, int);
static int check_access_flags_v(const char*, int, int);

static void compile_regex(regex_t*, const char*, int);
static char* get_basename(char*);
static char* pathcat(const char*, const char*);
static inline void helper_manage_stackheapbuf(char*, size_t*, int*,
    unsigned);
static char* strip_tag(const char*);
static int tag_pos(char*, char**, char**);
static char* get_tag(char*);
static int db2array(const char*, struct DBItem*);

#endif /* CRCTK_H */
