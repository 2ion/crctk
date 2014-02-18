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

#include "../include/realpath.h"

int my_realpath(const char *path) {

  return EXIT_SUCCESS;
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
    for(q = path; q < p; ++q) ++i;
    q = path; while(*++q);
    for(j = i; p<q; ++p) ++j;
    *dir = calloc(i+1, sizeof(char));
    strncpy(*dir, path, i);
    *base = calloc(j-i, sizeof(char));
    strncpy(*base, &path[i+1], j-i-1);
  }
  return EXIT_SUCCESS;
}

