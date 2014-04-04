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

