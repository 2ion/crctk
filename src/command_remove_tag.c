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

#include "command_remove_tag.h"

int command_remove_tag(int argc, char **argv, int optind, int flags) {
  const char *filename = argv[argc-1];
  char *str, *nstr, *p, *q;
  const char *d;
  int i;

  for(i = optind; i < argc; ++i) {
    check_access_flags(argv[i], F_OK | R_OK | W_OK, 1);
    str = get_basename((char*)argv[i]);
    if((nstr = strip_tag((const char*) str, crcregex_stripper)) == NULL) {
      printf("filename does not contain a hexstring: %s\n", argv[i]);
      return EXIT_FAILURE;
    }
    d = (const char*) dirname((char*)argv[i]);
    p = pathcat(d, (const char*)str);
    q = pathcat(d, (const char*)nstr);
    if(rename((const char*) p, (const char*) q) != 0)
      LERROR(EXIT_FAILURE, errno, "failed call to rename()");
    free(p);
    free(q);
    free(nstr);
  }
  return EXIT_SUCCESS;
}
