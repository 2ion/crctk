#include "realpath.h"
#include <stdlib.h>

int main(int argc, char **argv) {
  char *base = NULL;
  size_t baselen = 0;
  char *dir = NULL;
  size_t dirlen = 0;

  my_splitpath("helloworld", &dir, &dirlen, &base, &baselen);
  printf("base: %s\ndir: %s\n", base, dir);

  my_splitpath("/a/b/c/d.2/", &dir, &dirlen, &base, &baselen);

  printf("base: %s\ndir: %s\n", base, dir);


  return 0;
}
