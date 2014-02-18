#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include "../include/realpath.h"

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)

#define TEST_SPLITPATH(path) \
  base=NULL; \
  dir=NULL; \
  baselen=0; \
  dirlen=0; \
  printf("*** test: my_splitpath: %s ***\n", path); \
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

  return 0;
}
