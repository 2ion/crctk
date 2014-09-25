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

#include "command_check_batch.h"

int command_check_batch_from_argv(int argc, char **argv,
    int optind, int cmdflags) {
  int i;
  uint32_t dbcrc;
  uint32_t othercrc;
  char *X;
  struct DBFinder dbf;

  if(DB_find_open(dbiofile, &dbf) != 0) {
    log_failure(dbiofile, "could not open database");
    return EXIT_FAILURE;
  }

  for(i = optind; i < argc; ++i) {
    if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0) {
      log_failure(dbiofile, "skipping: file is not accessible: %s",
          argv[i]);
      continue;
    }
    if(DB_find_get(&dbf, argv[i], &dbcrc) != 0) {
      log_info(dbiofile, "skipping: file not in database: %s",
          argv[i]);
      continue;
    }
    if(cmdflags & CHECK_BATCH_PREFER_HEXSTRING) {
      X = get_tag(argv[i], crcregex);
      if(X == NULL) {
        log_info("--hexstring", "option is ineffective: filename does "
            "not contain a hexstring: %s", argv[i]);
        continue;
      }
      othercrc = strtol((const char*)X, NULL, 16);
      if(othercrc = dbcrc)
        log_success(dbiofile, "%s -> OK [%08X]", argv[i], dbcrc);
      else
        log_failure(dbiofile, "%s -> MISMATCH [%08X] vs. -x[%08X]\n",
            argv[i], dbcrc, othercrc);
      free(X);
      continue;
    }
    othercrc = compute_crc32(argv[i]);
    if(othercrc == 0) {
      fprintf(stderr, "%s: error: CRC32 is zero\n", argv[i]);
      continue;
    }
    if(othercrc == dbcrc)
      printf("[%s] %s -> OK [%08X]\n", argv[i], dbcrc);
    else
      printf("[%s] %s -> MISMATCH [%08X] vs. [%08X]\n",
          dbiofile, argv[i], dbcrc, othercrc);
  }
  DB_find_close(&dbf);
  return EXIT_SUCCESS;
}

int command_check_batch_from_db(int argc, char **argv,
    int optind, int cmdflags) {
  uint32_t crc;
  struct DBItem dbi = DBITEM_NULL;
  struct DBItem *e;

  if(DB_read(dbiofile, &dbi) != 0)
    LERROR(EXIT_FAILURE, 0, "[%s] file not readable", dbiofile);
  if(dbi.kbuf == NULL)
    LERROR(EXIT_SUCCESS, 0, "[%s] is empty", dbiofile);
  e = &dbi;
  do {
    printf("[%s] %s -> ", dbiofile, e->kbuf);
    if(check_access_flags_v(e->kbuf, F_OK | R_OK, 1) != 0) {
      printf("ERROR: NO ACCESS\n");
      continue;
    }
    crc = compute_crc32(e->kbuf);
    if(crc == 0) {
      printf("ERROR: CRC32 is zero\n", e->kbuf);
      continue;
    }
    if(crc == e->crc)
      printf("OK [%08X]\n", crc);
    else
      printf("MISMATCH [%08X is really %08X]\n", e->crc, crc);
  } while((e = e->next) != NULL);

  if(dbi.next != NULL)
    DB_item_free(dbi.next);

  return EXIT_SUCCESS;
}

int command_check_batch(int argc, char **argv, int optind,
    int cmdflags) {
  if(optind < argc)
    return command_check_batch_from_argv(argc, argv, optind, cmdflags);
  else
    return command_check_batch_from_db(argc, argv, optind, cmdflags);
  return EXIT_SUCCESS;
}
