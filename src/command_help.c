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

#include "command_help.h"

int command_help(int argc, char **argv, int optind, int flags) {
  printf("crctk - CRC32 Hexstring Toolkit\n"
"Copyright (C) 2014 Jens Oliver John (dev@2ion.de)\n"
"Usage: %s [-acCdDehmnpPrRstvVx] <file>|<file-listing>\n"
"Options:\n"
" -a, --append      Supplements -C, -m. Append to DB-FILE\n"
" -c, --calc        Compute the CRC32 for the given file\n"
" -C, --create-db   DB-FILE. For multiple input files, create a checksum\n"
"                   database overwriting any existing file\n"
" -d, --delete      DB-FILE. Delete all given paths from the database\n"
" -D, --dot         INT. Supplements -t. Insert the tag in front of the INTth\n"
"                   dot in the filename. Negative indices address the INTth last dot.\n"
" -e, --tag-regex   EXPR. Affects -s, -r. Set the regex for matching tags\n"
" -m, --merge       DB-FILE. Merges all given databases into DB-FILE.\n"
"                   Duplicate records subsequently overwrite each other\n"
" -n, --numerical   Supplements -c. Prints a decimal CRC32 checksum\n"
" -p, --print       Print the contents of any given database\n"
" -P, --to-realpath Convert paths in given databases to absolute paths\n"
" -r, --remove-tag  Remove any tags from the given files\n"
" -R, --realpath    Supplements -C, -m. Use absolute file paths\n"
" -s, --strip-tag   Supplements -t. Replace any existing tag\n"
" -t, --tag         Add a CRC32 hexstring tag to each given filename\n"
" -v, --verify      Compute CRC32 and compare with the hexstring\n"
"                   in the supplied filename\n"
" -V, --verify-db   DB-FILE. Check whether the files record in the\n"
"                   database still have the same CRC32 checksums\n"
" -x, --hexstring   Supplements -V, -C. For any tagged file, use the\n"
"                   hexstring as the reference CRC32\n"
"\n"
" -h, --help        Print this message and exit\n"
" --version         Print the program version and compilation date\n",
  argv[0], EXIT_SUCCESS, EXIT_FAILURE);
  return EXIT_SUCCESS;
}

