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

#include "log.h"
#include "config.h"

/* GLOBALS */

/* regex for extracting tags from filenames */
extern const char *crcregex;

/* regex for stripping tags from filenames [CLI] */
extern const char *crcregex_stripper; 

/* stores CRC32 hexstrings from [CLI] */
extern const char *hexarg;

/* stores database target filenames [CLI] */
extern const char *dbiofile;

/* stores the index of the dot in front of which a new tag should be
 * inserted [CLI] */
extern int dotidx;

/* stores the flag from --colors [CLI] */
extern int flag_use_colors;

/* stores the flag from --quiet [CLI] */
extern int flag_be_quiet;

/* TYPES */

enum { /* CLI option flags, XORed into arg #4 of command functions */
  TAG_ALLOW_STRIP                 = 1 << 0, // -s
  CALC_PRINT_NUMERICAL            = 1 << 1, // -n
  APPEND_TO_DB                    = 1 << 2, // -a
  CHECK_BATCH_PREFER_HEXSTRING    = 1 << 3, // -x
  USE_REALPATH                    = 1 << 4  // -R
};

typedef int     // program exit status
(*CommandFunction)(
        int,    // argc
        char**, // argv
        int,    // optind (see getopt_long(3))
        int);   // cmdflags bitmask

#endif /* CRCTK_H */
