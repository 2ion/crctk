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
#include "util.h"
#include "database.h"
#include "crc32.h"

#define _GNU_SOURCE

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
const char *optstring_short = "+X:xtnvV:hsrC:ce:p:a";
const struct option options_long[] = {
  { "verify", no_argument, NULL, 'v' },
  { "verify-db", required_argument, NULL, 'V' },
  { "prefer-hexstring", no_argument, NULL, 'x' },
  { "calc", no_argument, NULL, 'c' },
  { "numerical", no_argument, NULL, 'n' },
  { "create-db", required_argument, NULL, 'C' },
  { "append", no_argument, NULL, 'a' },
  { "print", required_argument, NULL, 'p' },
  { "tag", no_argument, NULL, 't' },
  { "strip-tag", no_argument, NULL, 's' },
  { "remove-tag", no_argument, NULL, 'r' },
  { "tag-regex", required_argument, NULL, 'e' },
  { "help", no_argument, NULL, 'h' },
  { 0, 0, 0, 0 }
};

/* TYPES */

enum {
  TAG_ALLOW_STRIP = 1 << 0,
  CALC_PRINT_NUMERICAL = 1 << 1,
  APPEND_TO_DB = 1 << 2,
  CHECK_BATCH_PREFER_HEXSTRING = 1 << 3
};

typedef int     // program exit status
(*CommandFunction)(
        int,    // argc
        char**, // argv
        int,    // optind
        int);   // cmdflags bitmask

/* PROTOTYPES */
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

static char* strip_tag(const char*);
static int tag_pos(char*, char**, char**);
static char* get_tag(char*);
static int db2array(const char*, struct DBItem*);

#endif /* CRCTK_H */
