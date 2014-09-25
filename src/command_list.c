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

#include "command_list.h"

int command_list_db(int argc, char **argv, int optind, int flags) {
  struct DBItem dbi = DBITEM_NULL;
  struct DBItem *e = NULL;
  char *p = NULL;
  int do_free_p = 0;
  int i = 0;

  for(i = optind; i < argc; ++i) {
    DBITEM_SET_NULL(dbi);
    e = NULL;
    if(access(argv[i], F_OK | R_OK) != 0 || DB_read(argv[i], &dbi) != 0) {
      log_failure(argv[i], "doesn't exist or isn't a valid database");
      continue;
    }
    if(dbi.kbuf == NULL) {
      log_info(argv[i], "is empty");
      continue;
    }
    e = &dbi;
    do {
      p = e->kbuf;
      if(flags & USE_REALPATH)
        p = get_realpath((const char*) e->kbuf, &do_free_p);
      log_success(argv[i], "%s -> %08X", p, e->crc);
      if(do_free_p == 1) {
        free(p);
        do_free_p = 0;
      }
    } while((e = e->next) != NULL);
    if(dbi.next != NULL)
      DB_item_free(dbi.next);
  }

  return EXIT_SUCCESS;
}
