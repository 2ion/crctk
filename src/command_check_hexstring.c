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

int command_check_hexstring(int argc, char **argv, int optind,
    int flags) {
  int ret = EXIT_SUCCESS;
  int i = optind-1;
  uint32_t strcrc, crc;
  long int cast;

  if(flags & CALC_PRINT_NUMERICAL)
    cast = (uint32_t) strtol(hexarg, NULL, 10);
  else
    cast = (uint32_t) strtol(hexarg, NULL, 16);
  if((cast == LONG_MIN || cast == LONG_MAX) &&
    errno == ERANGE)
    cast = 0;
  strcrc = (uint32_t) cast;

  while(argv[++i]) {
    if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0) {
      log_failure(argv[i], "not accessible, not a file or doesn't exist");
      ret = EXIT_FAILURE;
      continue;
    }
    if((crc = compute_crc32(argv[i])) == strcrc)
      log_success(argv[i], "match");
    else 
      log_failure(argv[i], "mismatch: %08X is really %08X", strcrc, crc);
  }

  return ret;
}
