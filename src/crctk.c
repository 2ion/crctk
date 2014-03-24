/*
 * crctk - CRC32 Hexstring Toolkit
 * Copyright (C) 2014 2ion (asterisk!2ion!de)
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
 * The CRC32 computer is based on code by elektron, formerly available
 * at            
 *      http://www.linuxquestions.org/questi...hreadid=199640 and
 *      http://kremlor.net/projects/crc32
 *
 * Thanks to elektron. 
 * */

#include "crctk.h"
#include "command_calc_batch.h"
#include "command_check_batch.h"
#include "command_help.h"
#include "command_list.h"

const char *crcregex = "[[:xdigit:]]\\{8\\}";
const char *crcregex_stripper =
  "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?";
const char *dbiofile = "crcsums.tdb";
const char *hexarg = "00000000";
const char *optstring_short = "+X:xtnvV:hsrC:ce:p:a";
const struct option options_long[] = {
  { "verify", no_argument, NULL, 'v' },
  { "verify-db", required_argument, NULL, 'V' },
  { "prefer-hexstring", no_argument, NULL, 'x' },
  { "calc", no_argument, NULL, 'c' },
  { "numerical", no_argument, NULL, 'n' },
  { "create-db", required_argument, NULL, 'C' },
  { "append", no_argument, NULL, 'a' },
  { "print", required_argument, NULL, 'p' },
  { "tag", no_argument, NULL, 't' },
  { "strip-tag", no_argument, NULL, 's' },
  { "remove-tag", no_argument, NULL, 'r' },
  { "tag-regex", required_argument, NULL, 'e' },
  { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'J' },
  { 0, 0, 0, 0 }
};

//FIXME: -a + duplicates truncate database
int command_tag(int argc, char **argv, int optind, int flags) {
  const char *filename = argv[argc-1];
  char *string = NULL;
  char *newstring = NULL;
  char *workstring = NULL;
  char tagstr[11];
  char *p = NULL;
  char *q = NULL;
  char *r = NULL;
  int i = 0;
  int f_free_workstring = 0;
  regex_t regex;
  uint32_t crcsum;

  check_access_flags(filename, F_OK | R_OK | W_OK, 1);
  string = get_basename((char*)filename);
  compile_regex(&regex, crcregex, REG_ICASE | REG_NOSUB);
  if(regexec(&regex, string, 0, 0, 0) == 0) {
    if (flags & TAG_ALLOW_STRIP) {
      workstring = strip_tag(string);
      if(workstring == NULL)
        LERROR(EXIT_FAILURE, 0,
                "strip_tag() failed for unknown reasons");
      f_free_workstring = 1;
    } else {
      LERROR(EXIT_FAILURE, 0,
          "filename already contains a CRC hexstring. Specify "
          "the -s flag to allow stripping the old hexstring.");
    }
  }
  regfree(&regex);
  if(workstring == NULL)
    workstring = string;
  crcsum = compute_crc32(filename);
  if(crcsum == 0)
    LERROR(EXIT_FAILURE, 0, "The file's CRC sum is zero.");
  sprintf(tagstr, "[%08X]", crcsum);
  newstring = malloc((strlen(workstring) + 11)*sizeof(char));
  if((p = strrchr(workstring, '.')) != NULL) {
    // has suffix: insert tag in front of suffix
    for(i=0, q=workstring; q != p; ++q)
      newstring[i++] = *q;
    newstring[i] = '\0';
    strncat(newstring, tagstr, 10);
    q = workstring;
    while(*q++);
    for(i+=10; p < q; ++p)
      newstring[i++] = *p;
    newstring[i] = '\0';
  } else {
    // no suffix: append tag
    newstring[0] = '\0';
    strcat(newstring, workstring);
    strcat(newstring, tagstr);
  }
  r = strdup(dirname((char*)filename));
  p = pathcat(r, string);
  q = pathcat(r, newstring);
  if(rename((const char*) p, (const char*) q) != 0)
    LERROR(EXIT_FAILURE, errno, "failed call to rename()");
  free(p);
  free(q);
  if(f_free_workstring == 1)
    free(workstring);
  free(newstring);
  return EXIT_SUCCESS;
}

int command_remove_tag(int argc, char **argv, int optind, int flags) {
  const char *filename = argv[argc-1];
  char *str, *nstr, *p, *q;
  const char *d;

  check_access_flags(filename, F_OK | R_OK | W_OK, 1);
  str = get_basename((char*)filename);
  if((nstr = strip_tag((const char*) str)) == NULL)
    LERROR(EXIT_FAILURE, 0, "%s does not contain an hexstring",
        filename);
  d = (const char*) dirname((char*)filename);
  p = pathcat(d, (const char*)str);
  q = pathcat(d, (const char*)nstr);
  if(rename((const char*) p, (const char*) q) != 0)
    LERROR(EXIT_FAILURE, errno, "failed call to rename()");
  free(p);
  free(q);
  free(nstr);
  return EXIT_SUCCESS;
}

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
      LERROR(EXIT_FAILURE, 0,
              "the filename does not contain a CRC32 hexstring.");
      return EXIT_FAILURE; // Not reached
  }
  regfree(&regex);
  compcrc = compute_crc32(filename);
  matchcrc = (uint32_t) strtol(results, NULL, 16);
  if(compcrc != matchcrc) {
    printf("mismatch: filename(%08X) != computed(%08X)\n",
            matchcrc, compcrc);
    return EXIT_FAILURE;
  } else {
    printf("match: filename(%08X) == computed(%08X)\n",
            matchcrc, compcrc);
    return EXIT_FAILURE;
  }
}


int command_calc(int argc, char **argv, int optind, int flags) {
  char *filename = NULL;
  uint32_t crc;
  int i = optind-1;

  while(argv[++i]) {
    filename = argv[i];
    if(check_access_flags_v(filename, F_OK | R_OK, 1) != 0) {
      printf("%s: Skipping, not a regular file or inaccessible ...\n",
          filename);
      continue;
    }
    crc = compute_crc32(filename);
    if(flags & CALC_PRINT_NUMERICAL)
      printf("%s: %u\n", filename, crc);
    else
      printf("%s: %08X\n", filename, crc);
  }
  return EXIT_SUCCESS;
}

int command_idle(int argc, char **argv, int optind, int flags) {
  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  int opt = 0;
  int cmdflags = 0;
  CommandFunction cmd = command_idle;

  while((opt = getopt_long(argc, argv, optstring_short,
         options_long, NULL)) != -1)
    switch(opt) {
      case 's': cmdflags |= TAG_ALLOW_STRIP; break;
      case 'x': cmdflags |= CHECK_BATCH_PREFER_HEXSTRING; break;
      case 'a': cmdflags |= APPEND_TO_DB; break;
      case 'n': cmdflags |= CALC_PRINT_NUMERICAL; break;
      case 'p': dbiofile = strdup(optarg); cmd = command_list_db; break;
      case 'V': dbiofile = strdup(optarg); cmd = command_check_batch; break;
      case 'e': crcregex_stripper = strdup(optarg); break;
      case 'c': cmd = command_calc; break;
      case 'C': dbiofile = strdup(optarg); cmd = command_calc_batch; break;
      case 'r': cmd = command_remove_tag; break;
      case 'v': cmd = command_check; break;
      case 'X': hexarg = strdup(optarg); cmd = command_check_hexstring; break;
      case 't': cmd = command_tag; break;
      case 'h': cmd = command_help; break;
      case 'J': puts("crctk version: " VERSION "\n" "Compiled on: " __DATE__ " " __TIME__); return EXIT_SUCCESS; break;
      default:  return EXIT_FAILURE;
    } // switch
  if(optind >= argc &&
      cmd != command_check_batch &&
      cmd != command_list_db && 
      cmd != command_help)
    LERROR(EXIT_FAILURE, 0,
            "Too few arguments. Use the -h flag "
            "to view usage information.");
  return cmd(argc, argv, optind, cmdflags);
}
