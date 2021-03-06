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

#include "command_delete.h"

int command_delete(int argc, char **argv, int optind, int cmdflags) {
  int i = 0;
  struct DBFinder dbf = DBFINDER_NULL;

  if(DB_find_open(dbiofile, &dbf) != 0) {
    log_failure(dbiofile, "could not open the database");
    return EXIT_FAILURE;
  }

  for(i = optind; i < argc; ++i) {
    switch(DB_find_remove(&dbf, (const char*)argv[i])) {
      case 0:
        log_success(dbiofile, "deleted key: %s", argv[i]);
        break;
      case -1:
        log_info(dbiofile, "no such key: %s", argv[i]);
        break;
      case -2:
        log_failure(dbiofile, "failed to delete key: %s", argv[i]);
        break;
      default:
        break;
    }
  }

  DB_find_close(&dbf);

  return EXIT_SUCCESS;
}
