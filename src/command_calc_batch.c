#include "command_calc_batch.h"

extern const char *crcregex;
extern const char *dbiofile;

int command_calc_batch(int argc, char **argv, int optind, int flags) {
  int i, fd;
  uint32_t crc;
  struct cdb_make cdbm;
  struct DBItem dbibuf = { NULL, 0, 0, NULL };
  struct DBItem *e = &dbibuf;

  if((flags & APPEND_TO_DB) &&
      (db2array(dbiofile, &dbibuf) == EXIT_FAILURE))
    LERROR(EXIT_FAILURE, 0, "option ineffective: append to DB "
        "(flag -a): could not load the db file");

  if((fd = open(dbiofile, O_WRONLY | O_CREAT ,
          S_IRUSR | S_IWUSR)) == -1)
    LERROR(EXIT_FAILURE, errno, "couldn't create file");
  if((cdb_make_start(&cdbm, fd)) != 0)
    LERROR(EXIT_FAILURE, 0, "couldn't initialize the cdb database");

  if(flags & APPEND_TO_DB)
    do {
      cdb_make_put(&cdbm, e->kbuf, e->kbuflen, &e->crc,
          sizeof(uint32_t), CDB_PUT_WARN);
      printf("from %s: <%s> -> %08X\n", dbiofile, e->kbuf, e->crc);
    } while((e = e->next) != NULL);

  for(i = optind; i < argc; ++i) {
    if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0) {
      LERROR(0,0, "Ignoring inaccessible file: %s", argv[i]);
      continue;
    }
    printf("*%s: <%s> ... ", dbiofile, argv[i]);
    if((crc = compute_crc32(argv[i])) == 0) {
      LERROR(0,0, "IGNORING: CRC32 is zero: %s", argv[i]);
      continue;
    }
    printf("%08X\n", crc);
    cdb_make_put(&cdbm, argv[i], (strlen(argv[i])+1)*sizeof(char),
        &crc, sizeof(uint32_t), CDB_PUT_REPLACE);
  }
  if(cdb_make_finish(&cdbm) != 0) {
    LERROR(0, 0, "cdb_make_finish() failed");
    close(fd);
    return EXIT_FAILURE;
  }
  close(fd);
  return EXIT_SUCCESS;
}
