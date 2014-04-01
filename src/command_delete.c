#include "command_delete.h"

extern const char *dbiofile;

int command_delete(int argc, char **argv, int optind, int cmdflags) {
  int i = 0;
  struct DBFinder dbf = DBFINDER_NULL;

  if(DB_find_open(dbiofile, &dbf) != 0) {
    printf(_("could not open database: %s\n"), dbiofile);
    return EXIT_FAILURE;
  }

  for(i = optind; i < argc; ++i) {
    switch(DB_find_remove(&dbf, (const char*)argv[i])) {
      case 0:
        printf(_("[%s] deleted key: %s\n"), dbiofile, argv[i]);
        break;
      case -1:
        printf(_("[%s] no such key: %s\n"), dbiofile, argv[i]);
        break;
      case -2:
        printf(_("[%s] failed to delete key: %s\n"), dbiofile, argv[i]);
        break;
      default:
        break;
    }
  }

  DB_find_close(&dbf);

  return EXIT_SUCCESS;
}
