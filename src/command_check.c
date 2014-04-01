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


