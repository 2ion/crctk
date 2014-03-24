#include "command_remove_tag.h"

int command_remove_tag(int argc, char **argv, int optind, int flags) {
  const char *filename = argv[argc-1];
  char *str, *nstr, *p, *q;
  const char *d;

  check_access_flags(filename, F_OK | R_OK | W_OK, 1);
  str = get_basename((char*)filename);
  if((nstr = strip_tag((const char*) str)) == NULL)
    LERROR(EXIT_FAILURE, 0, "%s does not contain an hexstring",
        filename);
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
