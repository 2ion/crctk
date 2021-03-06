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

#include "util.h"

void check_access_flags(const char *path, int access_flags, int notdir) {
  struct stat stbuf;

  if(access(path, access_flags) != 0) {
    log_failure(path, "%s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  if(notdir == 1) {
    /* bad error */
    if(stat(path, &stbuf) != 0)
        LERROR(EXIT_FAILURE, errno, "%s", path);

    if(S_ISDIR(stbuf.st_mode))
        log_failure(path, "is a directory");
        exit(EXIT_FAILURE);
  }
}

int check_access_flags_v(const char *path, int access_flags, int notadir) {
  struct stat stbuf;
  
  if(access(path, access_flags) != 0)
    return -1;

  if(notadir == 1) {
    /* bad error */
    if(stat(path, &stbuf) != 0)
      LERROR(EXIT_FAILURE, errno,  "%s", path);

    if(S_ISDIR(stbuf.st_mode))
      return -1;
  }

  return 0;
}

void compile_regex(regex_t *regex, const char *regexpr, int cflags) {
    int ci;
    char *regex_errbuf;
    size_t regex_errbuf_size;

    if(regex == NULL || regexpr == NULL)
        LERROR(EXIT_FAILURE, 0, "received at least one NULL argument.");

    if((ci = regcomp(regex, regexpr, cflags)) != 0) {
        regex_errbuf_size = regerror(ci, regex, NULL, 0);

        if((regex_errbuf = malloc(regex_errbuf_size)) == NULL)
          MLCERROR();

        regerror(ci, regex, regex_errbuf, regex_errbuf_size);
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

  if((r = calloc(strlen(p) + strlen(s) + 2, 1)) == NULL)
    MLCERROR();

  strcat(r, p);
  strcat(r, "/");
  strcat(r, s);

  return r;
}

char* strip_tag(const char *str, const char *crcregex_stripper) {
  char *rstr;
  const char *p, *q;
  int i;
  regex_t regex;
  regmatch_t rm;
  
  compile_regex(&regex, crcregex_stripper, REG_ICASE);

  if(regexec(&regex, str, 1, &rm, 0) == REG_NOMATCH) {
    /* no tag in filename */
    regfree(&regex);
    return NULL;
  }

  if((rstr = malloc((strlen(str)+1-(rm.rm_eo - rm.rm_so)))) == NULL)
    MLCERROR();

  for(p = str, q = &str[rm.rm_so], i=0; p < q; ++p)
    rstr[i++] = *p;

  for(p = &str[rm.rm_eo]; *p; ++p)
    rstr[i++] = *p;

  rstr[i] = '\0';

  return rstr;
}

char (*get_tag(char *str, const char *crcregex))[9] {
  char (*r)[9] = NULL;
  char *p = NULL;
  char *q = NULL;

  if(tag_pos(crcregex, str, &p, &q) != 0)
    return NULL;

  if((r = malloc(sizeof *r)) == NULL)
    MLCERROR();

  strncpy(*r, (const char*)p, 8);
  *r[8] = '\0';

  return r;
}

int tag_pos(const char *crcregex, char *str, char **p, char **q) {
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

char* get_realpath(const char *path, int *do_free_flag) {
  char *p = realpath((const char*)path, NULL);

  if(p == NULL)
    LERROR(EXIT_FAILURE, errno, "memory allocation error in realpath()");

  *do_free_flag = 1;

  return p;
}

