#include "command_calc.h"

int command_calc(int argc, char **argv, int optind, int flags) {
  char *filename = NULL;
  uint32_t crc;
  int i = optind-1;

  while(argv[++i]) {
    filename = argv[i];
    if(check_access_flags_v(filename, F_OK | R_OK, 1) != 0) {
      printf(_("Skipping %s: not an accessible or regular file\n"), filename);
      continue;
    }
    crc = compute_crc32(filename);
    if(flags & CALC_PRINT_NUMERICAL)
      printf(_("%s: %u\n"), filename, crc);
    else
      printf(_("%s: %08X\n"), filename, crc);
  }
  return EXIT_SUCCESS;
}

