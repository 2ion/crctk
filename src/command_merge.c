#include "command_merge.h"

extern const char *dbiofile;

int command_merge(int argc, char **argv, int optind, int cmdflags) {
  char *dbsources[argc-optind+1];
  int i, j;
  int do_truncate = 1;

  if(cmdflags & APPEND_TO_DB)
    do_truncate = 0;

  for(i = optind, j = 0; i < argc; ++i) {
    dbsources[j++] = argv[i];
  }
  dbsources[j] = NULL;

  if(DB_merge(dbiofile, (const char**) dbsources, do_truncate) != 0)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
