#include "command_calc_batch.h"

extern const char *crcregex;
extern const char *dbiofile;

int command_calc_batch(int argc, char **argv, int optind, int flags) {
  int i;
  int do_truncate = 1;
  uint32_t crc;
  struct DBItem *e = NULL;

  if(flags & APPEND_TO_DB)
    do_truncate = 0;

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
    e = DB_item_append(e, argv[i], sizeof(char)*(strlen(argv[i]+1)),
        crc);
  }
  if(e == NULL) {
    puts("Nothing to do.");
    return EXIT_SUCCESS;
  }
  e->next = NULL;
  if(DB_write(dbiofile, e, do_truncate) != 0) {
    LERROR(0,0, "Failed to write the database file: %s",
        dbiofile);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
