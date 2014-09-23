#include "command_to_realpaths.h"

int command_to_realpaths(int argc, char **argv, int optind, int cmdflags) {
  int i = 0;
  for(i = optind; i < argc; ++i) {
    if(DB_make_paths_absolute(argv[i]) != 0)
      log_failure(argv[i], "Path conversion failed");
  }
  return EXIT_SUCCESS;
}
