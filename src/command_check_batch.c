#include "command_check_batch.h"

extern const char *dbiofile;

int command_check_batch_from_argv(int argc, char **argv,
    int optind, int cmdflags) {
  int i;
  uint32_t dbcrc;
  uint32_t othercrc;
  char *X;
  struct DBFinder dbf;

  if(DB_find_open(dbiofile, &dbf) != 0)
    LERROR(EXIT_FAILURE, 0, "Could not open database: %s");

  for(i = optind; i < argc; ++i) {
    if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0) {
      fprintf(stderr, "%s: skipping: file is not accessible\n",
          argv[i]);
      continue;
    }
    if(DB_find_get(&dbf, argv[i], &dbcrc) != 0) {
      fprintf(stderr, "%s: skipping: file not found in database\n", argv[i]);
      continue;
    }
    if(cmdflags & CHECK_BATCH_PREFER_HEXSTRING) {
      X = get_tag(argv[i]);
      if(X == NULL) {
        fprintf(stderr, "Option -x is ineffective: filename does "
            "not contain a hexstring: %s\n", argv[i]);
        continue;
      }
      othercrc = strtol((const char*)X, NULL, 16);
      if(othercrc = dbcrc)
        printf("%s: OK [%08X]\n", argv[i], dbcrc);
      else
        printf("%s: MISMATCH %s[%08X] -x[%08X]\n", argv[i], dbiofile,
            dbcrc, othercrc);
      continue;
    }
    othercrc = compute_crc32(argv[i]);
    if(othercrc == 0) {
      fprintf(stderr, "%s: error: CRC32 is zero\n", argv[i]);
      continue;
    }
    if(othercrc == dbcrc)
      printf("%s: OK [%08X]\n", argv[i], dbcrc);
    else
      printf("%s: MISMATCH %s[%08X] real[%08X]\n", argv[i], dbiofile,
          dbcrc, othercrc);
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
    LERROR(EXIT_FAILURE, 0, "%s: could not read database", dbiofile);
  if(dbi.kbuf == NULL)
    LERROR(EXIT_SUCCESS, 0, "%s: database is empty.", dbiofile);
  e = &dbi;
  do {
    printf("[%s] %s ... ", dbiofile, e->kbuf);
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