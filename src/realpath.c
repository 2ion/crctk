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

