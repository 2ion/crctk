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
      fprintf(stderr, 0, "[%s] error when reading the file\n", argv[i]);
      continue;
    }
    if(dbi.kbuf == NULL) {
      printf("[%s] database is empty\n", argv[i]);
      continue;
    }
    e = &dbi;
    do {
      printf("[%s] %s -> %08X\n", argv[i], e->kbuf, e->crc);
    } while((e = e->next) != NULL);
    //FIXME: free the linked list e!!
  }

  return EXIT_SUCCESS;
}
