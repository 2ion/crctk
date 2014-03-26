#include "command_tag.h"

extern const char *crcregex;

int command_tag(int argc, char **argv, int optind, int flags) {
  const char *filename = NULL;
  char *string = NULL;
  char *newstring = NULL;
  char *workstring = NULL;
  char tagstr[11];
  char *p = NULL;
  char *q = NULL;
  char *r = NULL;
  int i = 0;
  int z = 0;
  int f_free_workstring = 0;
  regex_t regex;
  uint32_t crcsum;

  compile_regex(&regex, crcregex, REG_ICASE | REG_NOSUB);

  for(z = optind; z < argc; ++z) {
    i = 0;
    f_free_workstring = 0;
    crcsum = 0;
    p = NULL;
    q = NULL;
    r = NULL;
    workstring = NULL;
    newstring = NULL;
    string = NULL;

    filename = argv[z];
    check_access_flags(filename, F_OK | R_OK | W_OK, 1);
    string = get_basename((char*)filename);
    if(regexec(&regex, string, 0, 0, 0) == 0) {
      if (flags & TAG_ALLOW_STRIP) {
        workstring = strip_tag(string);
        if(workstring == NULL)
          LERROR(EXIT_FAILURE, 0,
                  "strip_tag() failed for unknown reasons");
        f_free_workstring = 1;
      } else {
        LERROR(EXIT_FAILURE, 0,
            "filename already contains a CRC hexstring. Specify "
            "the -s flag to allow stripping the old hexstring.");
      }
    }
    if(workstring == NULL)
      workstring = string;
    crcsum = compute_crc32(filename);
    if(crcsum == 0) {
      LERROR(0, 0, "%s: The file's CRC sum is zero.", filename);
      goto continue_with_next;
    }
    sprintf(tagstr, "[%08X]", crcsum);
    newstring = malloc((strlen(workstring) + 11)*sizeof(char));
    if((p = strrchr(workstring, '.')) != NULL) {
      // has suffix: insert tag in front of suffix
      for(i=0, q=workstring; q != p; ++q)
        newstring[i++] = *q;
      newstring[i] = '\0';
      strncat(newstring, tagstr, 10);
      q = workstring;
      while(*q++);
      for(i+=10; p < q; ++p)
        newstring[i++] = *p;
      newstring[i] = '\0';
    } else {
      // no suffix: append tag
      newstring[0] = '\0';
      strcat(newstring, workstring);
      strcat(newstring, tagstr);
    }
    r = strdup(dirname((char*)filename));
    p = pathcat(r, string);
    q = pathcat(r, newstring);
    if(rename((const char*) p, (const char*) q) != 0)
      LERROR(0, errno, "%s: failed call to rename()", filename);
    free(r);
    free(p);
    free(q);
    free(newstring);
continue_with_next:
    if(f_free_workstring == 1)
      free(workstring);
  } // for z
  regfree(&regex);
  return EXIT_SUCCESS;
}
