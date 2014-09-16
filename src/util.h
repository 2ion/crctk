/*
 * crctk - CRC32 Hexstring Toolkit
 * Copyright (C) 2014 Jens Oliver John <dev@2ion.de>
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
 *
 * */

#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <regex.h>
#include <libgen.h>
#include <string.h>

#include "database.h"

/* ANSI color escapes */

#define ANSI_COLOR_RED      "\x1b[0;31m"
#define ANSI_COLOR_GREEN    "\x1b[0;32m"
#define ANSI_COLOR_YELLOW   "\x1b[0;33m"
#define ANSI_COLOR_BLUE     "\x1b[0;34m"
#define ANSI_COLOR_MAGENTA  "\x1b[0;35m"
#define ANSI_COLOR_CYAN     "\x1b[0;36m"
#define ANSI_COLOR_REDB     "\x1b[1;31m"
#define ANSI_COLOR_GREENB   "\x1b[1;32m"
#define ANSI_COLOR_YELLOWB  "\x1b[1;33m"
#define ANSI_COLOR_BLUEB    "\x1b[1;34m"
#define ANSI_COLOR_MAGENTAB "\x1b[1;35m"
#define ANSI_COLOR_CYANB    "\x1b[1;36m"
#define ANSI_COLOR_RESET    "\x1b[0m"
#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)

void check_access_flags(const char*, int, int);
int check_access_flags_v(const char*, int, int);
void compile_regex(regex_t*, const char*, int);
char* get_basename(char*);
char* pathcat(const char*, const char*);
char* strip_tag(const char*, const char*);
int tag_pos(const char*, char*, char**, char**);
char (*get_tag(char*, const char*))[9];
char* get_realpath(const char*, int*);
char* get_capturefilepath(void);
int LOG(const char *module, const char *format, ...);

#endif /* UTIL_H */
