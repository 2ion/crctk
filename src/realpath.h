 /*
 * librealpath - fixing the broken POSIX realpath()
 * Copyright (C) 2014 2ion (asterisk!2ion!de)
 *
 * This library is part of crctk.
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
 */

#ifndef REALPATH_H
#define REALPATH_H

#include <assert.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)

extern int my_splitpath(const char *path,
    char **dir, size_t *dirlen,
    char **base, size_t *baselen);
extern char* my_realpath(const char *path);

#endif
