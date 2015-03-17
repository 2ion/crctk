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

int command_check(int argc, char **argv, int optind, int flags) {
  int ret = EXIT_SUCCESS;
  int i = optind-1;
  int  ci, ti;
  uint32_t compcrc, matchcrc;
  char *string;
  char results[9];
  regmatch_t rmatch;
  regex_t regex;

  while(argv[++i]) {
    if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0) {
      log_info(argv[i], "Inaccessbile file, skipping.");
      continue;
    }
    string = get_basename((char*)argv[i]);
    compile_regex(&regex, crcregex, REG_ICASE);
    switch(regexec((const regex_t*) &regex, string, 1, &rmatch, 0)) {
      case 0:
        for(ci = rmatch.rm_so, ti = 0; ci < rmatch.rm_eo; ++ci)
            results[ti++] = string[ci];
        results[ti] = '\0';
        break;
      case REG_NOMATCH:
        log_info(argv[i], "Does not contain a hexstring, ignoring.");
        continue;
    }
    regfree(&regex);
    compcrc = compute_crc32(argv[i]);
    matchcrc = (uint32_t) strtol(results, NULL, 16);
    if(compcrc == matchcrc)
      log_success(argv[i], "OK");
    else {
      log_failure(argv[i], "Mismatch: %08X is really %08X", matchcrc, compcrc);
      ret = EXIT_FAILURE;
    }
  } /* while */

  return ret;
}


