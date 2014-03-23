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
#include "command_help.h"

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

int command_list_db(int argc, char **argv, int optind, int flags) {
  struct cdb db;
  char statickbuf[255];
  char staticvbuf[255];
  char *kbuf = statickbuf;
  char *vbuf = staticvbuf;
  size_t kbuflen = sizeof(statickbuf)*sizeof(char);
  size_t vbuflen = sizeof(statickbuf)*sizeof(char);
  int kbuf_isstatic = 1;
  int vbuf_isstatic = 1;
  int fd;
  unsigned up, vpos, vlen, klen, kpos;

  check_access_flags(dbiofile, F_OK | R_OK, 1);
  if((fd = open(dbiofile, O_RDONLY)) == -1)
    LERROR(EXIT_FAILURE, errno, "could not open cdb file: %s",
        dbiofile);
  if(cdb_init(&db, fd) != 0)
    LERROR(EXIT_FAILURE, 0, "cdb_init() failed");
  cdb_seqinit(&up, &db);
  while(cdb_seqnext(&up, &db) > 0) {
    vpos = cdb_datapos(&db);
    vlen = cdb_datalen(&db);
    kpos = cdb_keypos(&db);
    klen = cdb_keylen(&db);
    helper_manage_stackheapbuf(kbuf, &kbuflen, &kbuf_isstatic, klen);
    helper_manage_stackheapbuf(vbuf, &vbuflen, &vbuf_isstatic, vlen);
    if(cdb_read(&db, kbuf, klen, kpos) != 0) {
      LERROR(0,0, "cdb_read(): failed to read key. Skipping entry ...");
      continue;
    }
    if(cdb_read(&db, vbuf, vlen, vpos) != 0) {
      LERROR(0,0, "cdb_read(): failed to read value. "
          "Skipping entry ...");
      continue;
    }
    printf("%s: <%s> -> %08X\n", dbiofile, kbuf,
        *(uint32_t*)vbuf);
  } // while
  cdb_free(&db);
  close(fd);
  if(kbuf_isstatic == 0)
    free(kbuf);
  if(vbuf_isstatic == 0)
    free(vbuf);
  return EXIT_SUCCESS;
}

int command_check_batch(int argc, char **argv, int optind,
    int cmdflags) {
  struct cdb db;
  int fd;
  char statickbuf[255];
  char *kbuf = statickbuf;
  size_t kbuflen = 255;
  int kbuf_isstatic = 1;
  uint32_t vbuf, crc;
  unsigned up, vpos, vlen, klen, kpos;
  int i, err;
  char *x;

  check_access_flags(dbiofile, F_OK | R_OK, 1);
  if((fd = open(dbiofile, O_RDONLY)) == -1)
    LERROR(EXIT_FAILURE, errno, "could not open cdb file: %s",
        dbiofile);
  if(cdb_init(&db, fd) != 0)
    LERROR(EXIT_FAILURE, 0, "cdb_init() failed");
  
  if(optind < argc) { for(i=optind; i < argc; ++i) {
    klen = sizeof(char) * (strlen(argv[i]) + 1);
    if((err = cdb_find(&db, argv[i], klen)) > 0) {
      printf("%s: <%s> ... ", dbiofile, argv[i]);
      vpos = cdb_datapos(&db);
      if((vlen = cdb_datalen(&db)) != sizeof(uint32_t)) {
        LERROR(0,0, "%s: invalid data value /%s", dbiofile,argv[i]);
        continue;
      }
      if(cdb_read(&db, &vbuf, vlen, vpos) != 0) {
        LERROR(0,0, "%s: cdb_read() failed on key <%s>", dbiofile,
            argv[i]);
        continue;
      }
      //
      if(cmdflags & CHECK_BATCH_PREFER_HEXSTRING) {
        x = get_tag(argv[i]);
        if(x == NULL)
          LERROR(0,0, "ERROR: option -x: argument does not contain an "
              "hexstring: %s", argv[i]);
        else {
          if((crc = strtol((const char*)x, NULL, 16)) == vbuf)
            printf("OK (%08X)[x]\n", crc);
          else
            printf("ERROR (is: %08X, db: %08X)[x]\n", crc, vbuf);
          free(x);
        }
      } else {
        //
        if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0)
          LERROR(0,0, "ERROR: file is not accessible");
        if((crc = compute_crc32(argv[i])) != 0) {
          if(crc == vbuf)
            printf("OK (%08X)\n", crc);
          else
            printf("ERROR (is: %08X, db: %08X)\n", crc, vbuf);
        } else
          printf("ERROR (CRC32 is zero)\n");
        //
      }
    } else if(err == 0) {
      printf("%s: Not entry found for path: %s\n", dbiofile, argv[i]);
    } else {
      LERROR(0,0, "cdb_find() failed: unknown error");
    } 
  }} else {
    // check all the paths from the database
    cdb_seqinit(&up, &db);
    while(cdb_seqnext(&up, &db) > 0) {
      klen = cdb_keylen(&db);
      kpos = cdb_keypos(&db);
      vlen = cdb_datalen(&db);
      vpos = cdb_datapos(&db);
      if((vlen = cdb_datalen(&db)) != sizeof(uint32_t)) {
        //FIXME: output key name
        LERROR(0,0, "%s: skipping entry: wrong data size (keypos=%u, "
            "vlen=%u)", dbiofile, kpos, vlen);
        continue;
      }
      helper_manage_stackheapbuf(kbuf, &kbuflen, &kbuf_isstatic, klen);
      if(cdb_read(&db, kbuf, klen, kpos) != 0) {
        LERROR(0,0, "cdb_read() failed for kbuf");
        continue;
      }
      if(cdb_read(&db, &vbuf, vlen, vpos) != 0) {
        LERROR(0,0, "cdb_read() failed for &vbuf");
        continue;
      }
      printf("%s: <%s> ... ", dbiofile, kbuf);
      if(check_access_flags_v(kbuf, F_OK | R_OK, 1) != 0) {
        LERROR(0,0, "ERROR: file is not accessible");
        continue;
      }
      if((crc = compute_crc32(kbuf)) != 0) {
        if(crc == vbuf)
          printf("OK (%08X)\n", crc);
        else
          printf("ERROR (is: %08X, db: %08X)\n", crc, vbuf);
      } else
        printf("ERROR (CRC32 is zero)\n");
    }
  }
  if(kbuf_isstatic == 0)
    free(kbuf);
  cdb_free(&db);
  close(fd);
  return EXIT_SUCCESS;
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

int db2array(const char *dbfile, struct DBItem *first) {
  assert(dbfile != NULL);
  assert(first != NULL);
  struct cdb db;
  unsigned up, kpos, klen, vpos, vlen;
  struct DBItem *cur = first;
  int atfirst = 1;
  int fd;
  cur->next = first;

  if(check_access_flags_v(dbfile, F_OK | R_OK, 1) != 0) {
    LERROR(0,0, "file not accessible: %s", dbfile);
    return EXIT_FAILURE;
  }
  if((fd = open(dbfile, O_RDONLY)) == -1) {
    LERROR(0,errno, "%s", dbiofile);
    return EXIT_FAILURE;
  }
  if(cdb_init(&db, fd) != 0) {
    LERROR(0,0, "cdb_init() failed");
    goto cleanup_error;
  }
  cdb_seqinit(&up, &db);
  while(cdb_seqnext(&up, &db) > 0) {
    if(atfirst == 0) {
      cur->next = malloc(sizeof(struct DBItem));
      if(cur->next == NULL)
        LERROR(EXIT_FAILURE, errno, "malloc() failed");
      cur = cur->next;
      cur->next = NULL;
    }
    kpos = cdb_keypos(&db);
    klen = cdb_keylen(&db);
    vpos = cdb_datapos(&db);
    vlen = cdb_datalen(&db);
    if(vlen > sizeof(uint32_t)) {
      LERROR(0,0, "Skipping entry with a data size "
          "> sizeof(uint32_t)");
      continue;
    }
    cur->kbuflen = klen;
    if((cur->kbuf = malloc(klen)) == NULL)
      LERROR(EXIT_FAILURE, 0, "malloc() failed");
    if(cdb_read(&db, cur->kbuf, klen, kpos) != 0) {
      LERROR(0,0, "cdb_read() failed. Skipping key at pos=%u", kpos);
      continue;
    }
    if(cdb_read(&db, &cur->crc, vlen, vpos) != 0) {
      LERROR(0,0, "cdb_read() failed. Skipping data at pos=%u", vpos);
      continue;
    }
    if(atfirst == 1) atfirst = 0;
    cur->next = NULL;
  } // while
  cdb_free(&db);
  close(fd);
  return EXIT_SUCCESS;
cleanup_error:
  close(fd);
  return EXIT_FAILURE;
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
