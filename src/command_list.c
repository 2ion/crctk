#include "command_list.h"

extern const char *dbiofile;

int command_list_db(int argc, char **argv, int optind, int flags) {
  struct DBItem dbi = DBITEM_NULL;
  struct DBItem *e = NULL;
  int i = 0;

  for(i = optind; i < argc; ++i) {
    DBITEM_SET_NULL(dbi);
    e = NULL;
    if(DB_read(argv[i], &dbi) != 0) {
      fprintf(stderr, _("[%s] not a valid database\n"), argv[i]);
      continue;
    }
    if(dbi.kbuf == NULL) {
      printf(_("[%s] database is empty\n"), argv[i]);
      continue;
    }
    e = &dbi;
    do {
      printf(_("[%s] %s -> %08X\n"), argv[i], e->kbuf, e->crc);
    } while((e = e->next) != NULL);
    if(dbi.next != NULL)
      DB_item_free(dbi.next);
  }

  return EXIT_SUCCESS;
}
