#include "command_calc_batch.h"

extern const char *crcregex;
extern const char *dbiofile;

int command_calc_batch(int argc, char **argv, int optind, int flags) {
  int i;
  int do_truncate = 1;
  uint32_t crc;
  struct DBItem first = { NULL, 0, 0, NULL };
  struct DBItem *e = &first;
  int at_first = 1;

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

    if(at_first == 0) {
      e->next = DB_item_alloc();
      e = e->next;
      e->next = NULL;
    }
    e->kbuflen = (strlen(argv[i]) +1)*sizeof(char);
    e->kbuf = malloc(e->kbuflen);
    if(e->kbuf==NULL) LERROR(0, EXIT_FAILURE, "memory allocation error");
    memcpy(e->kbuf, argv[i], e->kbuflen);
    e->crc = crc;
    if(at_first == 1) at_first = 0;
    e->next = NULL;
  }
  LERROR(0,0, "ITERATE OVER ENTRIES FOR TESTING!");
  e = &first;
  do {
    printf("%s -> %08X\n", e->kbuf, e->crc);
  } while((e = e->next) != NULL);
  if(DB_write(dbiofile, &first, do_truncate) != 0) {
    LERROR(0,0, "Failed to write the database file: %s",
        dbiofile);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
