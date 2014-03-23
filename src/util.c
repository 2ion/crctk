#include "util.h"

extern const char *crcregex;
extern const char *crcregex_stripper;
extern const char *dbiofile;

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

char* get_basename(char *path) {
  char *r = NULL;

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


