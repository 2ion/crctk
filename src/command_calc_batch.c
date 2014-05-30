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

#include "command_calc_batch.h"

int command_calc_batch(int argc, char **argv, int optind, int flags) {
  int i;
  int do_truncate = 1;
  int do_free_filename = 0;
  char *filename = NULL;
  uint32_t crc;
  struct DBItem first = DBITEM_NULL;
  struct DBItem *e = &first;
  int at_first = 1;
  char (*X)[9];

  if(flags & APPEND_TO_DB)
    do_truncate = 0;

  for(i = optind; i < argc; ++i) {
    filename = argv[i];
    if(check_access_flags_v(filename, F_OK | R_OK, 1) != 0) {
      printf("Ignoring inaccessible file: %s\n", filename);
      continue;
    }
    printf("*%s: <%s> ... ", dbiofile, filename);

    if(flags & CHECK_BATCH_PREFER_HEXSTRING) {
      X = get_tag(filename, crcregex);
      if(X == NULL) {
        printf("-x option does not apply: no hexstring found in filename: %s\n",
            filename);
        continue;
      } else {
        crc = strtol((const char*)X, NULL, 16);
        goto skip_crc_computation;
      }
    }

    if((crc = compute_crc32(filename)) == 0) {
      printf("Ignoring, CRC32 is zero: %s\n", filename);
      continue;
    }

skip_crc_computation:
    printf("%08X\n", crc);

    if(flags & USE_REALPATH)
      filename = get_realpath((const char*)filename, &do_free_filename);

    if(at_first == 0) {
      e->next = DB_item_alloc();
      e->next->prev = e;
      e = e->next;
      e->next = NULL;
    }
    e->kbuflen = (strlen(filename) +1)*sizeof(char);
    e->kbuf = malloc(e->kbuflen);
    if(e->kbuf==NULL) LERROR(0, EXIT_FAILURE, "memory allocation error");
    memcpy(e->kbuf, filename, e->kbuflen);
    e->crc = crc;
    if(at_first == 1) at_first = 0;
    e->next = NULL;
    if(X != NULL) { // free memory allocated by get_tag()
      free(X);
      X = NULL;
    }
    if(do_free_filename == 1) {
      free(filename);
      filename = NULL;
      do_free_filename = 0;
    }
  } // for

  if(DB_write(dbiofile, &first, do_truncate) != 0) {
    LERROR(0,0, "Failed to write the database file: %s",
        dbiofile);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
