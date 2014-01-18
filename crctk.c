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

#include <assert.h>
#include <cdb.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)
#ifndef VERSION
#define VERSION "unknown"
#endif
#define COPY_DB_STATIC_BUF_LEN 255

const char *crcregex = "[[:xdigit:]]\\{8\\}";
const char *crcregex_stripper = "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?";
const char *dbiofile = "crcsums.tdb";
enum { ExitMatch = EXIT_SUCCESS, ExitNoMatch = EXIT_FAILURE,
  ExitArgumentError = 10, ExitRegexError = 11, ExitUnknownError = 12};
enum { CmdIdle, CmdCheck, CmdTag, CmdRmTag, CmdCalc, CmdCalcBatch, CmdCheckBatch,
  CmdList };
enum { TAG_ALLOW_STRIP = 1 << 0, CALC_PRINT_NUMERICAL = 1 << 1 };

static unsigned long getFileSize(const char*);
static unsigned long computeCRC32(const char*);
static int command_check(const char*);
static int command_check_batch(int, char**, int);
static int command_list_db(void);
static int command_tag(const char*,int);
static int command_calc(const char*, int);
static int command_calc_batch(int, char**, int);
static void check_access_flags(const char*, int, int);
static int check_access_flags_v(const char*, int, int);
static void compile_regex(regex_t*, const char*, int);
static char* get_basename(char*);
static char* pathcat(const char*,const char*);
static inline void helper_manage_stackheapbuf(char*, size_t*, int*, unsigned);
static char* strip_tag(const char*);
static int copy_cdb(const char*, struct cdb_make*, int);

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
        LERROR(ExitUnknownError, 0,
                "received at least one NULL argument.");
    if((ci = regcomp(regex, regexpr, cflags)) != 0) {
        regex_errbuf_size = regerror(ci, regex, NULL, 0);
        regex_errbuf = malloc(regex_errbuf_size);
        regerror(ci, regex, regex_errbuf, regex_errbuf_size);
        if(regex_errbuf == NULL)
            LERROR(ExitRegexError, 0,
                    "regex error: failed to allocate memory "
                    "for the regex error message.");
        LERROR(ExitRegexError, 0, "%s", regex_errbuf);
    }
}


int command_calc_batch(int argc, char **argv, int optind) {
  int i, fd;
  unsigned long crc;
  struct cdb_make cdbm;

  if((fd = open(dbiofile, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR)) == -1)
    LERROR(ExitUnknownError, errno, "couldn't create file");
  if((cdb_make_start(&cdbm, fd)) != 0)
    LERROR(ExitUnknownError, 0, "couldn't initialize the cdb database");
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
        &crc, sizeof(unsigned long), CDB_PUT_INSERT);
  }
  if(cdb_make_finish(&cdbm) != 0) {
    LERROR(0, 0, "cdb_make_finish() failed");
    close(fd);
    return ExitUnknownError;
  }
  close(fd);
  return EXIT_SUCCESS;
}

int command_tag(const char *filename, int flags) {
    char *string;
    char *newstring;
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
        if ((flags & TAG_ALLOW_STRIP) == TAG_ALLOW_STRIP) {
            workstring = strip_tag(string);
            if(workstring == NULL)
                LERROR(ExitUnknownError, 0,
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
        LERROR(ExitUnknownError, 0, "The file's CRC sum is zero.");
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

int command_remove_tag(const char *filename) {
    char *str, *nstr, *p, *q;
    const char *d;

    check_access_flags(filename, F_OK | R_OK | W_OK, 1);
    str = get_basename((char*)filename);
    if((nstr = strip_tag((const char*) str)) == NULL)
        LERROR(ExitArgumentError, 0, "%s does not contain an hexstring", filename);
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



int command_check(const char *filename) {
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
            LERROR(ExitNoMatch, 0,
                    "the filename does not contain a CRC32 hexstring.");
            return ExitNoMatch; // Not reached
    }
    regfree(&regex);
    compcrc = computeCRC32(filename);
    matchcrc = (unsigned long) strtol(results, NULL, 16);
    if(compcrc != matchcrc) {
        printf("mismatch: filename(%08lX) != computed(%08lX)\n",
                matchcrc, compcrc);
        return ExitNoMatch;
    } else {
        printf("match: filename(%08lX) == computed(%08lX)\n",
                matchcrc, compcrc);
        return ExitMatch;
    }
}
int command_list_db(void) {
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
    LERROR(EXIT_FAILURE, errno, "could not open cdb file: %s", dbiofile);
  if(cdb_init(&db, fd) != 0)
    LERROR(EXIT_FAILURE, 0, "cdb_init() failed");
  cdb_seqinit(&up, &db);
  while(cdb_seqnext(&up, &db) > 0) {
    puts("enter read loop");
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
      LERROR(0,0, "cdb_read(): failed to read value. Skipping entry ...");
      continue;
    }
    printf("buf sizes: kbuf=%lu klen=%lu vbuf=%lu vlen=%lu\n", kbuflen, klen, vbuflen, vlen);
    printf("%s: <%s> -> %08lX\n", dbiofile, kbuf, *(unsigned long*)vbuf);
  } // while
  cdb_free(&db);
  close(fd);
  if(kbuf_isstatic == 0)
    free(kbuf);
  if(vbuf_isstatic == 0)
    free(vbuf);
  return EXIT_SUCCESS;
}

int command_check_batch(int argc, char **argv, int optind) {
  struct cdb db;
  int fd;
  char kbuf[PATH_MAX];
  size_t kbuf_len = PATH_MAX;
  char *wkbuf = kbuf;
  char *bnamedb;
  char *bnameargv;
  int wkbuf_isstatic = 1;
  int i, iDone;
  unsigned long vbuf, crc;
  unsigned up, vpos, vlen, klen, kpos;

  check_access_flags(dbiofile, F_OK | R_OK, 1);
  if((fd = open(dbiofile, O_RDONLY)) == -1)
    LERROR(EXIT_FAILURE, errno, "could not open cdb file: %s", dbiofile);
  if(cdb_init(&db, fd) != 0)
    LERROR(EXIT_FAILURE, 0, "cdb_init() failed");
  cdb_seqinit(&up, &db);
  while(cdb_seqnext(&up, &db) > 0) {
    vpos = cdb_datapos(&db);
    vlen = cdb_datalen(&db);
    kpos = cdb_keypos(&db);
    klen = cdb_keylen(&db);
    if(vlen > sizeof(unsigned long)) {
      LERROR(0,0, "Skipping record with value size > sizeof(unsigned long)");
      continue;
    }
    helper_manage_stackheapbuf(wkbuf, &kbuf_len, &wkbuf_isstatic, klen);
    if(cdb_read(&db, wkbuf, klen, kpos) != 0) {
      LERROR(0,0, "Skipping current record because I failed to read the key");
      continue;
    }
    if(cdb_read(&db, &vbuf, vlen, vpos) != 0) {
      LERROR(0,0, "Skipping current record because I failed to read the data");
      continue;
    }
    if(optind < argc) { // we have arguments in argv to check the db entr against
      bnamedb = basename(wkbuf);
      for(iDone = 0, i = optind; i < argc; ++i) {
        bnameargv = basename(argv[i]);
        if(strcmp((const char*)bnamedb, (const char*)bnameargv) == 0) {
          iDone = 1;
          printf("%s[%08lX] pathspec matches argument #%d: %s ... ",
              dbiofile, vbuf, i, bnameargv);
          if(check_access_flags_v(bnameargv, F_OK | R_OK, 1) != 0) {
            printf("FAILED (no read access)\n");
            iDone = 0;
            break;
          }
          if((crc = computeCRC32(bnameargv)) == vbuf) {
            printf("OK\n");
            break;
          }
          else {
            printf("FAILED (real: %08lX)\n", crc);
            iDone = 1;
            break;
          }
        } // if
      } // for
    if(iDone == 0)
      printf("%s: no matching argument in argv for database entry: %s\n",
          dbiofile, wkbuf);
    } else {
    // check if the stored path is still available
      if(check_access_flags_v(wkbuf, F_OK | R_OK, 1) != 0) {
        LERROR(0,0, "(%s) file %s doesn't exist or is inaccessible.",
        dbiofile, wkbuf);
        continue;
    }
    printf("%s[%08lX]: %s ... ", dbiofile, vbuf, wkbuf);
    if((crc = computeCRC32(wkbuf)) == vbuf)
      printf("OK\n");
    else
      printf("FAILED (real: %08lX)\n", crc);
    }
  } // while
  cdb_free(&db);
  if(wkbuf_isstatic == 0) // throws -Wfree-nonheap-object, but we're safe 'cause we track
    free(wkbuf);
  close(fd);
  return EXIT_SUCCESS;
}

void check_access_flags(const char *path, int access_flags, int notdir) {
    struct stat stbuf;

    if(access(path, access_flags) != 0)
        LERROR(ExitArgumentError, errno, "%s", path);
    if(notdir == 1) {
        if(stat(path, &stbuf) != 0)
            LERROR(ExitUnknownError, errno, "%s", path);
        else
            if(S_ISDIR(stbuf.st_mode))
                LERROR(ExitArgumentError, 0, "%s is a directory.", path);
    }
}

int check_access_flags_v(const char *path, int access_flags, int notadir) {
  struct stat stbuf;
  
  if(access(path, access_flags) != 0)
    return -1;

  if(notadir == 1) {
    if(stat(path, &stbuf) != 0) {
      LERROR(ExitUnknownError, errno,  "(unclean exit)");
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
        LERROR(ExitUnknownError, errno, "memory allocation error");
    for(p = str, q = &str[rm.rm_so], i=0; p < q; ++p)
        rstr[i++] = *p;
    for(p = &str[rm.rm_eo]; *p; ++p)
        rstr[i++] = *p;
    rstr[i] = '\0';
    return rstr;
}

char* get_basename(char *path) {
    char *r;
    if((r = basename(path)) == NULL)
        LERROR(ExitArgumentError, 0,
            "could not extract specified path's basename.");
    return r;
}

char* pathcat(const char *p, const char *s) {
    char *r;
    r = calloc(strlen(p) + strlen(s) + 2, sizeof(char));
    if(r == NULL)
        LERROR(ExitUnknownError, errno, "memory allocation error");
    strcat(r, p);
    strcat(r, "/");
    strcat(r, s);
    return r;
}

int command_calc(const char *filename, int flags) {
    unsigned long crc;

    check_access_flags(filename, F_OK | R_OK, 1);
    crc = computeCRC32(filename);
    if((flags & CALC_PRINT_NUMERICAL) == CALC_PRINT_NUMERICAL)
      printf("%s: %lu\n", filename, crc);
    else
      printf("%s: %08lX\n", filename, crc);
    return EXIT_SUCCESS;
}

void helper_manage_stackheapbuf(char *buf, size_t *buflen, int *buf_isstatic, unsigned datalen) {
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

int copy_cdb(const char *srcdb, struct cdb_make *target_db, int tfd) {
  struct cdb source_db;
  int sfd;
  unsigned up, klen, kpos, vlen, vpos;
  char kbufstack[COPY_DB_STATIC_BUF_LEN];
  char vbufstack[COPY_DB_STATIC_BUF_LEN];
  size_t vbuflen = COPY_DB_STATIC_BUF_LEN;
  size_t kbuflen = COPY_DB_STATIC_BUF_LEN;
  char *kbuf = kbufstack;
  char *vbuf = vbufstack;
  int vbuf_isstatic = 1;
  int kbuf_isstatic = 1;

  if((sfd = open(srcdb, O_RDONLY, 0)) == -1) {
    LERROR(0, errno, "open() on source db failed");
    return -1;
  }
  if((cdb_make_start(target_db, tfd)) != 0) {
    LERROR(0,0, "cdb_make_start() on target db failed");
    return -1;
  }
  if((cdb_init(&source_db, sfd)) != 0) {
    LERROR(0,0, "cdb_init() on source db failed");
    return -1;
  }
  cdb_seqinit(&up, &source_db);
  while(cdb_seqnext(&up, &source_db) > 0) {
    kpos = cdb_keypos(&source_db);
    klen = cdb_keylen(&source_db);
    vpos = cdb_datapos(&source_db);
    vlen = cdb_datalen(&source_db);
    helper_manage_stackheapbuf(vbuf, &vbuflen, &vbuf_isstatic, vlen);
    helper_manage_stackheapbuf(kbuf, &kbuflen, &kbuf_isstatic, klen);
  } // while
  if(vbuf_isstatic == 0) // throws -Wfree-nonheap-object but we're safe
    free(vbuf);
  if(kbuf_isstatic == 0) // throws -W-Wfree-nonheap-object but we're safe
    free(kbuf);
}
int main(int argc, char **argv) {
    int opt;
    int cmd = CmdIdle;
    int cmdflags = 0;

    while((opt = getopt(argc, argv, "+tnvV:hsrC:ce:p:")) != -1) {
        switch(opt) {
            case 'n':
                cmdflags |= CALC_PRINT_NUMERICAL; 
                break;
            case 'p':
                dbiofile = strdup(optarg);
                cmd = CmdList;
                break;
            case 'V':
                dbiofile = strdup(optarg);
                cmd = CmdCheckBatch;
                break;
            case 'e':
                crcregex_stripper = strdup(optarg);
                break;
            case 'c':
                cmd = CmdCalc;
                break;
            case 'C':
                dbiofile = strdup(optarg);
                cmd = CmdCalcBatch;
                break;
            case 'r':
                cmd = CmdRmTag;
                break;
            case 's':
                cmdflags |= TAG_ALLOW_STRIP;
                break;
            case 'v':
                cmd = CmdCheck;
                break;
            case 't':
                cmd = CmdTag;
                break;
            case 'h':
                puts("crctk v" VERSION " (" __DATE__ " " __TIME__ ")\n"
                        "CRC32 Hexstring Toolkit\n"
                        "Copyright (C) 2014 2ion (asterisk!2ion!de)\n"
                        "Upstream: https://github.com/2ion/crctk\n"
                        "Usage: crctk [-vVcCoptsreh] <file>|<file-list>\n"
                        "Options:\n"
                        " -v Compute CRC32 and compare with the hexstring\n"
                        "    in the supplied filename.\n"
                        "    Return values: EXIT_SUCCESS: match\n"
                        "                   EXIT_FAILURE: no match\n"
                        "                   0xA: invalid argument\n"
                        "                   0xB: regex compilation error\n"
                        "                   0xC: unknown error\n"
                        " -V FILE. Read checksums and filenames from a FILE\n"
                        "    created by the -C option and check if the files\n"
                        "    have the listed checksums.\n"
                        " -c Compute the CRC32 of the given file, print and exit.\n"
                        " -n Supplements -c: print CRC32 in its numerical format.\n"
                        " -C for multiple input files, create a checksum listing\n"
                        "    for use with the -V option.\n"
                        " -p FILE. Print the contents of a file created by the -C\n"
                        "    options to stdout.\n"
                        " -t Tag file with a CRC32 hexstring. Aborts if\n"
                        "    the filename does already contain a tag.\n"
                        " -s Supplements -t: strip eventually existing tag\n"
                        "    and compute a new CRC32 hexstring.\n"
                        "    Return values: EXIT_SUCCESS: success\n"
                        "                   EXIT_FAILURE: generic failure\n"
                        "                   Rest as above.\n"
                        " -r If the file is tagged, remove the tag.\n"
                        " -e EXPR. Changes the regular expression used to\n"
                        "    match tags when doing -s|-r to EXPR. Default:\n"
                        "    [[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?\n"
                        " -h Print this message and exit successfully.");
                return EXIT_SUCCESS;
            default:
                return ExitArgumentError;
        }
    }
    if(optind >= argc && cmd != CmdCheckBatch && cmd != CmdList)
        LERROR(ExitArgumentError, 0,
                "too few arguments. Use the -h flag "
                "to obtain usage information.");
    switch(cmd) {
        case CmdList:
            return command_list_db();
        case CmdCalc:
            return command_calc(argv[argc-1], cmdflags);
        case CmdCalcBatch:
            srand(time(NULL));
            return command_calc_batch(argc, argv, optind);
        case CmdCheckBatch:
            return command_check_batch(argc, argv, optind);
        case CmdIdle:
            break;
        case CmdCheck:
            return command_check(argv[argc-1]);
        case CmdTag:
            return command_tag(argv[argc-1], cmdflags);
        case CmdRmTag:
            return command_remove_tag(argv[argc-1]);
    }
    return EXIT_SUCCESS;
}
