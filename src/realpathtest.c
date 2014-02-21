 /*
 * realpathtest - test routines for librealpath
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
 */

#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include "realpath.h"

#define TEST_SPLITPATH(path) \
  base=NULL; \
  dir=NULL; \
  baselen=0; \
  dirlen=0; \
  printf("*******\nTEST_SPLITPATH: %s\n", path); \
  if(my_splitpath(path, &dir, &dirlen, &base, &baselen)!=0)\
    LERROR(EXIT_FAILURE,0,"my_splitpath() failed!");\
  else{\
    printf("directory: %s -- base: %s\n", dir, base);\
    free(dir); free(base);\
  }

int main(int argc, char **argv) {
  char *base = NULL;
  size_t baselen = 0;
  char *dir = NULL;
  size_t dirlen = 0;

  TEST_SPLITPATH("helloworld");
  TEST_SPLITPATH("/a/b/c/d.2/");
  TEST_SPLITPATH("/");
  TEST_SPLITPATH("/a/v/c///");

  my_realpath(".");

  return 0;
}
