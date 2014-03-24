#include "command_list.h"

extern const char *dbiofile;

int command_list_db(int argc, char **argv, int optind, int flags) {
  struct DBItem dbi = { NULL, 0, 0, NULL };
  struct DBItem *e;

  if(DB_read(dbiofile, &dbi) != 0)
    LERROR(EXIT_FAILURE, 0, "Could not read database: %s", dbiofile);
  if(dbi.kbuf == NULL)
    LERROR(EXIT_SUCCESS, 0, "Database is empty.");
  e = &dbi;
  do {
    printf("[%s] %s -> %08X\n", dbiofile, e->kbuf, e->crc);
  } while((e = e->next) != NULL);
  
  return EXIT_SUCCESS;
}
