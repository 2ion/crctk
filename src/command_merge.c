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

#include "command_merge.h"

extern const char *dbiofile;

int command_merge(int argc, char **argv, int optind, int cmdflags) {
  char *dbsources[argc-optind+1];
  int i, j;
  int do_truncate = 1;

  if(cmdflags & APPEND_TO_DB)
    do_truncate = 0;

  for(i = optind, j = 0; i < argc; ++i) {
    dbsources[j++] = argv[i];
  }
  dbsources[j] = NULL;

  if(DB_merge(dbiofile, (const char**) dbsources, do_truncate) != 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
