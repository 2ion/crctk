 /*
 * librealpath - fixing the broken POSIX realpath()
 * Copyright (C) 2014 2ion (asterisk!2ion!de)
 *
 * This library is part of crctk.
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
 */

#include "realpath.h"

char* my_realpath(const char *path) {
  assert(path!=NULL);
  size_t pwdlen = 0;
  char *pwd = NULL;
  size_t baselen = 0;
  char *base = NULL;
  size_t dirlen = 0;
  char *dir = NULL;
  char *r = NULL;
  size_t rlen = 0;

  // store old pwd
  do {
    pwdlen += 255;
    if(pwd == NULL)
      pwd = malloc(sizeof(char)*pwdlen);
    else
      pwd = realloc(pwd, sizeof(char)*pwdlen);
  } while(getcwd(pwd, pwdlen) == NULL);

  if(my_splitpath(path, &dir, &dirlen,
        &base, &baselen) != EXIT_SUCCESS) {
    LERROR(0, 0, "error when trying to split the path: %s",
        path);
    free(pwd);
    return NULL;
  }

  if(chdir(dir) != 0) {
    LERROR(0, errno, "chdir() into %s failed.", path);
    if(base != NULL)
      free(base);
    if(dir != NULL)
      free(dir);
    free(pwd);
    return NULL;
  }

  do {
    rlen += 255;
    if(r == NULL)
      r = malloc(sizeof(char)*rlen);
    else
      r = realloc(r, sizeof(char)*rlen);
  } while(getcwd(r, rlen) == NULL);

  if(base != NULL && strlen(base) > 0) {
    r = realloc(r, sizeof(char)*(rlen+baselen));
    strncat(r, "/", 2);
    strncat(r, (const char*)base, baselen);
  }

  if(base != NULL)
    free(base);
  free(dir);

  // restore old pwd
  if(chdir(pwd) != 0)
    LERROR(0, errno, "Failed to restore the old pwd.");
  free(pwd);

  return r;
}

int my_splitpath(const char *path,
    char **dir, size_t *dirlen,
    char **base, size_t *baselen) {
  assert(path != NULL);
  assert(dir != NULL);
  assert(base != NULL);
  assert(baselen != NULL);
  char *q = NULL;
  char *p = NULL;
  int i = 0;
  int j = 0;
  if(strcmp(path, "/") == 0) {
    *base=NULL;
    *dir=strdup(path);
    return EXIT_SUCCESS;
  }
  p=strrchr(path, '/');
  if(p == NULL) {
    *dir = NULL;
    *dirlen = 0;
    *base = strdup(path);
    *baselen = strlen(path)+1;
  } else {
    for(q = (char*)path; q<p; ++q) ++i;
    q = (char*)path; while(*++q);
    for(j = i; p<q; ++p) ++j;
    *dir = calloc(i+1, sizeof(char));
    if(*dir == NULL) return EXIT_FAILURE;
    *dirlen = (size_t) i+1;
    strncpy(*dir, path, i);
    *base = calloc(j-i, sizeof(char));
    if(*base == NULL) {
      free(dir);
      *dirlen = 0;
      return EXIT_FAILURE;
    }
    *baselen = (size_t) j-i;
    strncpy(*base, &path[i+1], j-i-1);
  }
  return EXIT_SUCCESS;
}

