#include "command_check_hexstring.h"

extern const char *crcregex;
extern const char *hexarg;

int command_check_hexstring(int argc, char **argv, int optind,
    int flags) {
  const char *filename = argv[argc-1];
  uint32_t strcrc, crc;
  regex_t regex;

  check_access_flags(filename, F_OK | R_OK, 1);
  compile_regex(&regex, crcregex, REG_ICASE | REG_NOSUB);
  switch(regexec((const regex_t*)&regex, hexarg, 0, NULL, 0)) {
    case REG_NOMATCH:
      LERROR(EXIT_FAILURE, 0, "parameter to option -u (%s) "
          "is not a valid hexarg.", hexarg);
    case 0:
      regfree(&regex);
      break;
  }
  strcrc = (uint32_t) strtol(hexarg, NULL, 16);
  if((crc = compute_crc32(filename)) == strcrc) {
    printf("match: computed(%08X) == hexarg(%08X)\n",
        crc, strcrc);
    return EXIT_SUCCESS;
  }
  else {
    printf("mismatch: computed(%08X) != hexarg(%08X)\n",
            crc, strcrc);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
