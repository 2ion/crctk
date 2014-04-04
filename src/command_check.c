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

#include "command_check.h"

extern const char *crcregex;

int command_check(int argc, char **argv, int optind, int flags) {
  const char *filename = argv[argc-1];
  int  ci, ti;                        
  uint32_t compcrc;              
  uint32_t matchcrc;             
  char *string;                       
  char results[9];                    
  regmatch_t rmatch;                  
  regex_t regex;                      

  check_access_flags(filename, F_OK | R_OK, 1);
  string = get_basename((char*)filename);
  compile_regex(&regex, crcregex, REG_ICASE);
  switch(regexec((const regex_t*) &regex, string, 1, &rmatch, 0)) {
    case 0:
      for(ci = rmatch.rm_so, ti = 0; ci < rmatch.rm_eo; ++ci)
          results[ti++] = string[ci];
      results[ti] = '\0';
      break;
    case REG_NOMATCH:
      printf(_("The filename does not contain a hexstring: %s\n"), filename);
      return EXIT_FAILURE; // Not reached
  }
  regfree(&regex);
  compcrc = compute_crc32(filename);
  matchcrc = (uint32_t) strtol(results, NULL, 16);
  if(compcrc != matchcrc) {
    printf(_("%s: mismatch: %08X is really %08X\n"),
        matchcrc, compcrc);
    return EXIT_FAILURE;
  } else {
    printf(_("%s: match: %08X\n"), matchcrc);
    return EXIT_FAILURE;
  }
}


