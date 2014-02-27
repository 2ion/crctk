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

unsigned long getFileSize(const char *filename) {
    FILE *input_file;
    unsigned long file_size;

    input_file = fopen(filename, "r");
    if(input_file == NULL) { 
        LERROR(0, 0, "Could not open %s for reading", filename);
        return 0;
    }
    fseek(input_file, 0, SEEK_END);
    file_size = ftell(input_file);
    rewind(input_file);
    fclose(input_file);
    return file_size;
}

unsigned long computeCRC32(const char *filename) { 
    int input_fd;
    Bytef *file_buffer;
    unsigned long file_size;
    unsigned long chunk_size=1024;
    uInt bytes_read;
    unsigned long crc=0L;
  
    file_size = getFileSize(filename);
    if(file_size == 0 ) { 
        LERROR(0, 0, "size of %s has been calculated to be 0. "
            "Cannot calculate CRC", filename);
        return 0;
    }
    file_buffer = malloc(chunk_size);
    if(file_buffer == NULL) { 
        LERROR(0, 0, "buffer allocation error");
        return 0;
    }
    crc = crc32(0L, Z_NULL, 0); 
    input_fd = open(filename, O_RDONLY);
    if(input_fd == -1) { 
        LERROR(0, 0, "could not open %s for reading", filename);
        return 0;
    }
    while(1) {
        bytes_read = read(input_fd, file_buffer, chunk_size);
        if(bytes_read == 0)
            break;
        if(bytes_read == -1) {
            LERROR(0, errno, "read() returned -1");
            break;
        }
        crc = crc32(crc, file_buffer, bytes_read);
    }
    free(file_buffer);
    close(input_fd);
    return crc;
}

void compile_regex(regex_t *regex, const char *regexpr, int cflags) {
    int ci;
    char *regex_errbuf = NULL;
    size_t regex_errbuf_size;

    if(regex == NULL || regexpr == NULL)
        LERROR(EXIT_FAILURE, 0,
                "received at least one NULL argument.");
    if((ci = regcomp(regex, regexpr, cflags)) != 0) {
        regex_errbuf_size = regerror(ci, regex, NULL, 0);
        regex_errbuf = malloc(regex_errbuf_size);
        regerror(ci, regex, regex_errbuf, regex_errbuf_size);
        if(regex_errbuf == NULL)
            LERROR(EXIT_FAILURE, 0,
                    "regex error: failed to allocate memory "
                    "for the regex error message.");
        LERROR(EXIT_FAILURE, 0, "%s", regex_errbuf);
    }
}

int command_help(int argc, char **argv, int optind, int flags) {
  printf("crctk v" VERSION " (" __DATE__ " " __TIME__ ")\n"
          "CRC32 Hexstring Toolkit\n"
          "Copyright (C) 2014 2ion (asterisk!2ion!de)\n"
          "Upstream: https://github.com/2ion/crctk\n"
          "Usage: %s [-aCcefhnprstuVv] <file>|<file-listing>\n"
          "Options:\n"
          " -v Compute CRC32 and compare with the hexstring\n"
          "    in the supplied filename.\n"
          "    Return values: EXIT_SUCCESS: match\n"
          "                   EXIT_FAILURE: no match\n"
          " -V FILE. Read checksums and filenames from a FILE\n"
          "    created by the -C option and check if the files\n"
          "    have the listed checksums.\n"
          " -u HEXSTRING. Check if the specified file has the CRC\n"
          "    sum HEXSTRING.\n"
          " -x Supplements -V. For any tagged file, use the hexstring as\n"
          "    as the reference CRC32 instead of computing anew.\n"
          " -c Compute the CRC32 of the given file, print and exit.\n"
          " -n Supplements -c. print CRC32 in its numerical format.\n"
          " -C FILE. for multiple input files, create a checksum listing\n"
          "    for use with the -V option. Overwrites the given file.\n"
          " -a Supplements -C. Append to the given database file instead\n"
          "    of overwriting it.\n"
          " -p FILE. Print the contents of a file created by the -C\n"
          "    options to stdout.\n"
          " -t Tag file with a CRC32 hexstring. Aborts if\n"
          "    the filename does already contain a tag.\n"
          " -s Supplements -t. strip eventually existing tag\n"
          "    and compute a new CRC32 hexstring.\n"
          "    Return values: EXIT_SUCCESS: success\n"
          "                   EXIT_FAILURE: generic failure\n"
          " -r If the file is tagged, remove the tag.\n"
          " -e EXPR. Changes the regular expression used to\n"
          "    match tags when doing -s|-r to EXPR. Default:\n"
          "    [[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?\n"
          " -h Print this message and exit successfully.\n", argv[0]);
  return EXIT_SUCCESS;
}

//FIXME: -a + duplicates truncate database
int command_calc_batch(int argc, char **argv, int optind, int flags) {
  int i, fd;
  unsigned long crc;
  struct cdb_make cdbm;
  struct DBItem dbibuf = { NULL, 0, 0, NULL };
  struct DBItem *e = &dbibuf;

  if((flags & APPEND_TO_DB) &&
      (db2array(dbiofile, &dbibuf) == EXIT_FAILURE))
    LERROR(EXIT_FAILURE, 0, "option ineffective: append to DB "
        "(flag -a): could not load the db file");

  if((fd = open(dbiofile, O_WRONLY | O_CREAT ,
          S_IRUSR | S_IWUSR)) == -1)
    LERROR(EXIT_FAILURE, errno, "couldn't create file");
  if((cdb_make_start(&cdbm, fd)) != 0)
    LERROR(EXIT_FAILURE, 0, "couldn't initialize the cdb database");

  if(flags & APPEND_TO_DB)
    do {
      cdb_make_put(&cdbm, e->kbuf, e->kbuflen, &e->crc,
          sizeof(unsigned long), CDB_PUT_WARN);
      printf("from %s: <%s> -> %08lX\n", dbiofile, e->kbuf, e->crc);
    } while((e = e->next) != NULL);

  for(i = optind; i < argc; ++i) {
    if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0) {
      LERROR(0,0, "Ignoring inaccessible file: %s", argv[i]);
      continue;
    }
    printf("*%s: <%s> ... ", dbiofile, argv[i]);
    if((crc = computeCRC32(argv[i])) == 0) {
      LERROR(0,0, "IGNORING: CRC32 is zero: %s", argv[i]);
      continue;
    }
    printf("%08lX\n", crc);
    cdb_make_put(&cdbm, argv[i], (strlen(argv[i])+1)*sizeof(char),
        &crc, sizeof(unsigned long), CDB_PUT_REPLACE);
  }
  if(cdb_make_finish(&cdbm) != 0) {
    LERROR(0, 0, "cdb_make_finish() failed");
    close(fd);
    return EXIT_FAILURE;
  }
  close(fd);
  return EXIT_SUCCESS;
}

int command_tag(int argc, char **argv, int optind, int flags) {
  const char *filename = argv[argc-1];
  char *string = NULL;
  char *newstring = NULL;
  char *workstring = NULL;
  char tagstr[11];
  char *p, *q, *r;
  int i;
  int f_free_workstring = 0;
  regex_t regex;
  unsigned long crcsum;

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
  crcsum = computeCRC32(filename);
  if(crcsum == 0)
    LERROR(EXIT_FAILURE, 0, "The file's CRC sum is zero.");
  sprintf(tagstr, "[%08lX]", crcsum);
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
  unsigned long strcrc, crc;
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
  strcrc = (unsigned long) strtol(hexarg, NULL, 16);
  if((crc = computeCRC32(filename)) == strcrc) {
    printf("match: computed(%08lX) == hexarg(%08lX)\n",
        crc, strcrc);
    return EXIT_SUCCESS;
  }
  else {
    printf("mismatch: computed(%08lX) != hexarg(%08lX)\n",
            crc, strcrc);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int command_check(int argc, char **argv, int optind, int flags) {
  const char *filename = argv[argc-1];
  int  ci, ti;                        
  unsigned long compcrc;              
  unsigned long matchcrc;             
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
  compcrc = computeCRC32(filename);
  matchcrc = (unsigned long) strtol(results, NULL, 16);
  if(compcrc != matchcrc) {
    printf("mismatch: filename(%08lX) != computed(%08lX)\n",
            matchcrc, compcrc);
    return EXIT_FAILURE;
  } else {
    printf("match: filename(%08lX) == computed(%08lX)\n",
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
    printf("%s: <%s> -> %08lX\n", dbiofile, kbuf,
        *(unsigned long*)vbuf);
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
  unsigned long vbuf, crc;
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
      if((vlen = cdb_datalen(&db)) != sizeof(unsigned long)) {
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
            printf("OK (%08lX)[x]\n", crc);
          else
            printf("ERROR (is: %08lX, db: %08lX)[x]\n", crc, vbuf);
          free(x);
        }
      } else {
        //
        if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0)
          LERROR(0,0, "ERROR: file is not accessible");
        if((crc = computeCRC32(argv[i])) != 0) {
          if(crc == vbuf)
            printf("OK (%08lX)\n", crc);
          else
            printf("ERROR (is: %08lX, db: %08lX)\n", crc, vbuf);
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
      if((vlen = cdb_datalen(&db)) != sizeof(unsigned long)) {
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
      if((crc = computeCRC32(kbuf)) != 0) {
        if(crc == vbuf)
          printf("OK (%08lX)\n", crc);
        else
          printf("ERROR (is: %08lX, db: %08lX)\n", crc, vbuf);
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

void check_access_flags(const char *path, int access_flags,
    int notdir) {
    struct stat stbuf;

    if(access(path, access_flags) != 0)
        LERROR(EXIT_FAILURE, errno, "%s", path);
    if(notdir == 1) {
        if(stat(path, &stbuf) != 0)
            LERROR(EXIT_FAILURE, errno, "%s", path);
        else
            if(S_ISDIR(stbuf.st_mode))
                LERROR(EXIT_FAILURE, 0, "%s is a directory.",
                    path);
    }
}

int check_access_flags_v(const char *path, int access_flags,
    int notadir) {
  struct stat stbuf;
  
  if(access(path, access_flags) != 0)
    return -1;

  if(notadir == 1) {
    if(stat(path, &stbuf) != 0) {
      LERROR(EXIT_FAILURE, errno,  "(unclean exit)");
    } else
      if(S_ISDIR(stbuf.st_mode))
        return -1;
  }
  return 0;
}

char* strip_tag(const char *str) {
  regex_t regex;
  regmatch_t rm;
  const char *p, *q;
  char *rstr;
  int i;
  
  compile_regex(&regex, crcregex_stripper, REG_ICASE);
  if(regexec(&regex, str, 1, &rm, 0) == REG_NOMATCH) {
    // no tag in filename
    regfree(&regex);
    return NULL;
  }
  rstr = malloc((strlen(str)+1-(rm.rm_eo - rm.rm_so)) * sizeof(char));
  if(rstr == NULL)
    LERROR(EXIT_FAILURE, errno, "memory allocation error");
  for(p = str, q = &str[rm.rm_so], i=0; p < q; ++p)
    rstr[i++] = *p;
  for(p = &str[rm.rm_eo]; *p; ++p)
    rstr[i++] = *p;
  rstr[i] = '\0';
  return rstr;
}

char* get_tag(char *str) {
  char *r = NULL;
  char *p = NULL;
  char *q = NULL;

  if(tag_pos(str, &p, &q) != 0)
    return NULL;
  r = malloc(sizeof(char)*9);
  assert(r != NULL);
  strncpy(r, (const char*)p, 8);
  r[8] = '\0';
  return r;
}

int tag_pos(char *str, char **p, char **q) {
  assert(str != NULL);
  regex_t regex;
  regmatch_t rm;

  compile_regex(&regex, crcregex, REG_ICASE);
  if(regexec(&regex, str, 1, &rm, 0) == REG_NOMATCH) {
    regfree(&regex);
    return -1;
  }
  *p = &str[rm.rm_so];
  *q = &str[rm.rm_eo]-1;
  regfree(&regex);
  return 0;
}

char* get_basename(char *path) {
    char *r;

    if((r = basename(path)) == NULL)
        LERROR(EXIT_FAILURE, 0,
            "could not extract specified path's basename.");
    return r;
}

char* pathcat(const char *p, const char *s) {
    char *r;

    r = calloc(strlen(p) + strlen(s) + 2, sizeof(char));
    if(r == NULL)
        LERROR(EXIT_FAILURE, errno, "memory allocation error");
    strcat(r, p);
    strcat(r, "/");
    strcat(r, s);
    return r;
}

int command_calc(int argc, char **argv, int optind, int flags) {
  const char *filename = argv[argc-1];
  unsigned long crc;

  check_access_flags(filename, F_OK | R_OK, 1);
  crc = computeCRC32(filename);
  if(flags & CALC_PRINT_NUMERICAL)
    printf("%s: %lu\n", filename, crc);
  else
    printf("%s: %08lX\n", filename, crc);
  return EXIT_SUCCESS;
}

int command_idle(int argc, char **argv, int optind, int flags) {
  return EXIT_SUCCESS;
}

void helper_manage_stackheapbuf(char *buf, size_t *buflen,
    int *buf_isstatic, unsigned datalen) {
  assert(buf != NULL);
  assert(buflen != NULL);
  assert(buf_isstatic != NULL);
  if(*buflen >= datalen)
    return; // buffer is large enough
  if(*buf_isstatic == 1) {
    // allocate new dynamic buffer
    if((buf = calloc(1, datalen)) == NULL)
      LERROR(EXIT_FAILURE, errno, "call to calloc failed");
    *buf_isstatic = 0;
  } else {
    // re-allocate buffer
    if((buf = realloc(buf, datalen)) == NULL)
      LERROR(EXIT_FAILURE, errno, "call to realloc failed");
  }
  *buflen = datalen;
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
    if(vlen > sizeof(unsigned long)) {
      LERROR(0,0, "Skipping entry with a data size "
          "> sizeof(unsigned long)");
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
