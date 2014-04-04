/*
 * crctk - CRC32 Hexstring Toolkit
 * Copyright (C) 2014 Jens Oliver John <dev@2ion.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * */

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
      printf(_("parameter to option -u is not a valid hexstring: %s\n"), hexarg);
      return EXIT_FAILURE;
    case 0:
      regfree(&regex);
      break;
  }
  strcrc = (uint32_t) strtol(hexarg, NULL, 16);
  if((crc = compute_crc32(filename)) == strcrc) {
    printf("match: computed(%08X) == hexarg(%08X)\n",
        crc, strcrc);
    printf(_("%s: match: %08X\n"), crc);
    return EXIT_SUCCESS;
  }
  else {
    printf(_("%s: mismatch: %08X is really %08X\n"),
          strcrc, crc);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
