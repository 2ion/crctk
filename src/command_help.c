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
"Usage: %s [-aCcefhmnPpRrstuVv] <file>|<file-listing>\n"
"Options:\n"
"-v, --verify\n"
"  Compute CRC32 and compare with the hexstring\n"
"  in the supplied filename.\n"
"-V DB-FILE, --verify-db DB-FILE\n"
"  Check whether the files record in the database\n"
"  still have the same CRC32 checksums\n"
"-x, --prefer-hexstring\n"
"  Supplements -V, -C. For any tagged file, use the\n"
"  hexstring as the reference CRC32\n"
"-c, --calc\n"
"  Compute the CRC32 for the given file and print\n"
"  it, then exit\n"
"-n, --numerical\n"
"  Supplements -c. Print the CRC32 in decimal\n"
"-C DB-FILE, --create-db DB-FILE\n"
"   For multiple input files, store a checksum DB\n"
"   in DB-FILE The target file will be overwritten\n"
"   if it exists.\n"
"-a, --append\n"
"   Supplements -C. Append to DB-FILE.\n"
"-m DB-FILE, --merge DB-FILE\n"
"   Given a list of database files as arguments,\n"
"   all of this files' contents will be merged\n"
"   into DB-FILE. The listed databases are not\n"
"   modified. Records will identical keys will\n"
"   replace each other in the order the database\n"
"   files were specified.\n"
" -C DB-FILE, --create-db DB-FILE\n"
"   For multiple input files, create a checksum\n"
"   listing for use with the -V option. Overwrites\n"
"   the given file.\n"
" -d DB-FILE, --delete DB-FILE\n"
"   Delete all files given as arguments from the\n"
"   database DB-FILE if such a record exists.\n"
" -a, --append\n"
"   Supplements -C and -m. Append to the given\n"
"   database file instead of overwriting it.\n"
" -p, --print\n"
"   For each database in the argument list,\n"
"   print its contents to stdout.\n"
" -P, --to-realpath\n"
"   For each database in the argument list,\n"
"   convert all recorded paths from relative to\n"
"   absolute paths\n"
" -t, --tag\n"
"   Tag file with a CRC32 hexstring. Aborts if\n"
"   the filename does already contain a tag.\n"
" -s, --strip-tag\n"
"   Supplements -t. strip eventually existing tag\n"
"   and compute a new CRC32 hexstring.\n"
" -r, --remove-tag\n"
"   If the file is tagged, remove the tag.\n"
" -e EXPR, --tag-regex EXPR\n"
"   Changes the regular expression used to\n"
"   match tags when doing -s|-r to EXPR. Default:\n"
"   [[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?\n"
" -h, --help\n"
"   Print this message and exit successfully.\n"
"--version\n"
"   Prints version and compilation information.\n" ,
  argv[0], EXIT_SUCCESS, EXIT_FAILURE);
  return EXIT_SUCCESS;
}

