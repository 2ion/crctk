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

#ifndef CRCTK_H
#define CRCTK_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "realpath.h"
#include "util.h"
#include "database.h"
#include "crc.h"
#include "log.h"

#include "config.h"

/* GLOBALS */

extern const char *crcregex;
extern const char *crcregex_stripper;
extern const char *hexarg;
extern const char *dbiofile;
extern int dotidx;
extern int flag_use_colors;
extern int flag_be_quiet;

/* TYPES */

enum {
  TAG_ALLOW_STRIP                 = 1 << 0,
  CALC_PRINT_NUMERICAL            = 1 << 1,
  APPEND_TO_DB                    = 1 << 2,
  CHECK_BATCH_PREFER_HEXSTRING    = 1 << 3,
  USE_REALPATH                    = 1 << 4
};

typedef int     // program exit status
(*CommandFunction)(
        int,    // argc
        char**, // argv
        int,    // optind
        int);   // cmdflags bitmask

#endif /* CRCTK_H */
