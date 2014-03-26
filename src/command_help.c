#include "command_help.h"

int command_help(int argc, char **argv, int optind, int flags) {
  printf("crctk - CRC32 Hexstring Toolkit\n"
"Copyright (C) 2014 2ion (dev@2ion.de)\n"
"Usage: %s [-aCcefhnprstuVv] <file>|<file-listing>\n"
"Options:\n"
"-v, --verify\n"
"  Compute CRC32 and compare with the hexstring\n"
"  in the supplied filename.\n"
"-V DB-FILE, --verify-db DB-FILE\n"
"  Check whether the files record in the database\n"
"  still have the same CRC32 checksums\n"
"-x, --prefer-hexstring\n"
"  Supplements -V. For any tagged file, use the\n"
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
" -a, --append\n"
"   Supplements -C and -m. Append to the given\n"
"   database file instead of overwriting it.\n"
" -p DB-FILE, --print DB-FILE\n"
"   Print the contents of a file created by the -C\n"
"   options to stdout.\n"
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

