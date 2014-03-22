#include "util.h"

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
