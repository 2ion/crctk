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
#include <basedir.h>
#include <errno.h>
#include <error.h>
#include <libgen.h>
#include <regex.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "database.h"
#include "crctk.h"

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)
#define CAPTURE_DIR "crctk"
#define CAPTURE_FILE "stash.crctk"

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

#endif /* UTIL_H */
