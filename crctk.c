/*
 * crctk - CRC32 Hexstring Toolkit
 * Written by Jens Oliver John <asterisk!2ion!de>
 *
 * The CRC32 computer is based on code by elektron, formerly available
 * at            
 *      http://www.linuxquestions.org/questi...hreadid=199640 and
 *      http://kremlor.net/projects/crc32
 *
 * Thanks to elektron. 
 * */

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <getopt.h>
#include <regex.h>

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)
#ifndef VERSION
#define VERSION "unknown"
#endif

const char *crcregex = "[[:xdigit:]]\\{8\\}";
enum { ExitMatch = EXIT_SUCCESS, ExitNoMatch = EXIT_FAILURE,
    ExitArgumentError = 10, ExitRegexError = 11, ExitUnknownError = 12};
enum { CmdIdle, CmdCheck, CmdTag, CmdRmTag };
enum { TAG_ALLOW_STRIP = 1 << 0 };

static unsigned long getFileSize(const char*);
static unsigned long computeCRC32(const char*);
static int command_check(const char*);
static int command_tag(const char*,int);
static void check_access_flags(const char*, int, int);
static void compile_regex(regex_t*, const char*, int);
static char* get_basename(char*);
static char* pathcat(const char*,const char*);

unsigned long getFileSize(const char *filename) {
  FILE *input_file;
  unsigned long file_size;
  
  input_file = fopen(filename, "r");
  if(input_file == NULL) { 
    LERROR(0, 0, "Could not open %s for reading", filename);
    return 0;
  }
  
  fseek(input_file, 0, SEEK_END);
  file_size = ftell(input_file);
  rewind(input_file);
  
  fclose(input_file);
  return file_size;
}

unsigned long computeCRC32( const char *filename )
{ 
  int input_fd;
  Bytef *file_buffer;
  unsigned long file_size;
  unsigned long chunk_size=1024;
  uInt bytes_read;
  unsigned long crc=0L;
  
  file_size = getFileSize(filename);
  
  if(file_size == 0 ) { 
    LERROR(0, 0, "size of %s has been calculated to be 0. "
            "Cannot calculate CRC", filename);
    return 0;
  }
  file_buffer = malloc(chunk_size);
  if(file_buffer == NULL ) { 
    LERROR(0, 0, "buffer allocation error");
    return 0;
  }
  crc = crc32(0L, Z_NULL, 0);
  
  input_fd = open(filename, O_RDONLY);
  if(input_fd == -1 ) { 
    LERROR(0, 0, "could not open %s for reading", filename);
    return 0;
  }
    while(1) {
        bytes_read = read(input_fd, file_buffer, chunk_size);
        if(bytes_read == 0)
            break;
        if(bytes_read == -1) {
            LERROR(0, errno, "read() returned -1");
            break;
        }
        crc = crc32(crc, file_buffer, bytes_read);
    }
  
  free(file_buffer);
  close(input_fd);
  return crc;
}

void compile_regex(regex_t *regex, const char *regexpr, int cflags) {
    int ci;
    char *regex_errbuf = NULL;
    size_t regex_errbuf_size;

    if(regex == NULL || regexpr == NULL)
        LERROR(ExitUnknownError, 0,
                "received at least one NULL argument.");
    if((ci = regcomp(regex, regexpr, cflags)) != 0) {
        regex_errbuf_size = regerror(ci, regex, NULL, 0);
        regex_errbuf = malloc(regex_errbuf_size);
        regerror(ci, regex, regex_errbuf, regex_errbuf_size);
        if(regex_errbuf == NULL)
            LERROR(ExitRegexError, 0,
                    "regex error: failed to allocate memory "
                    "for the regex error message.");
        LERROR(ExitRegexError, 0, "%s", regex_errbuf);
    }
}

int command_check(const char *filename) {
    int  ci, ti;                        
    unsigned long compcrc;              
    unsigned long matchcrc;             
    char *string;                       
    char results[9];                    
    regmatch_t rmatch;                  
    regex_t regex;                      

    check_access_flags(filename, F_OK | R_OK, 1);
    if((string = basename((char*)filename)) == NULL)
        LERROR(ExitArgumentError, 0,
                "could not extract specified path's basename.");

    /* compile regex */

    compile_regex(&regex, crcregex, REG_ICASE);

    /* match CRC32 hexstrings */

    switch(regexec((const regex_t*) &regex, string, 1, &rmatch, 0)) {
        case 0:
            for(ci = rmatch.rm_so, ti = 0; ci < rmatch.rm_eo; ++ci)
                results[ti++] = string[ci];
            results[ti] = '\0';
            break;
        case REG_NOMATCH:
            LERROR(ExitNoMatch, 0,
                    "the filename does not contain a CRC32 hexstring.");
            return ExitNoMatch; // Not reached
    }
    regfree(&regex);

    /* eval */

    compcrc = computeCRC32(filename);
    matchcrc = (unsigned long) strtol(results, NULL, 16);
    if(compcrc != matchcrc) {
        printf("mismatch: filename(%08lX) != computed(%08lX)\n",
                matchcrc, compcrc);
        return ExitNoMatch;
    } else {
        printf("match: filename(%08lX) == computed(%08lX)\n",
                matchcrc, compcrc);
        return ExitMatch;
    }
}

void check_access_flags(const char *path, int access_flags, int notdir) {
    struct stat stbuf;

    if(access(path, access_flags) != 0)
        LERROR(ExitArgumentError, errno, "%s", path);
    if(notdir == 1) {
        if(stat(path, &stbuf) != 0)
            LERROR(ExitUnknownError, errno, "%s", path);
        else
            if(S_ISDIR(stbuf.st_mode))
                LERROR(ExitArgumentError, 0, "%s is a directory.", path);
    }
}

char* strip_tag(const char *str) {
    regex_t regex;
    regmatch_t rm;
    const char *p, *q;
    char *rstr;
    int i;
    
    compile_regex(&regex, "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?",
            REG_ICASE);
    if(regexec(&regex, str, 1, &rm, 0) == REG_NOMATCH) {
        // no tag in filename
        regfree(&regex);
        return NULL;
    }
    rstr = malloc((strlen(str)+1-(rm.rm_eo - rm.rm_so)) * sizeof(char));
    if(rstr == NULL)
        LERROR(ExitUnknownError, errno, "memory allocation error");
    for(p = str, q = &str[rm.rm_so], i=0; p < q; ++p)
        rstr[i++] = *p;
    for(p = &str[rm.rm_eo]; *p; ++p)
        rstr[i++] = *p;
    rstr[i] = '\0';
    return rstr;
}

char* get_basename(char *path) {
    char *r;
    if((r = basename(path)) == NULL)
        LERROR(ExitArgumentError, 0,
            "could not extract specified path's basename.");
    return r;
}

char* pathcat(const char *p, const char *s) {
    char *r;
    r = calloc(strlen(p) + strlen(s) + 2, sizeof(char));
    if(r == NULL)
        LERROR(ExitUnknownError, errno, "memory allocation error");
    strcat(r, p);
    strcat(r, "/");
    strcat(r, s);
    return r;
}

int command_tag(const char *filename, int flags) {
    char *string;
    char *newstring;
    char *workstring = NULL;
    char tagstr[11];
    char *p, *q, *r;
    int i;
    regex_t regex;
    unsigned long crcsum;

    check_access_flags(filename, F_OK | R_OK | W_OK, 1);
    string = get_basename((char*)filename);
    compile_regex(&regex, crcregex, REG_ICASE | REG_NOSUB);
    if(regexec(&regex, string, 0, 0, 0) == 0) {
        if ((flags & TAG_ALLOW_STRIP) == TAG_ALLOW_STRIP) {
            workstring = strip_tag(string);
            if(workstring == NULL)
                LERROR(ExitUnknownError, 0,
                        "strip_tag() failed for unknown reasons");
        } else {
            LERROR(EXIT_FAILURE, 0,
                "filename already contains a CRC hexstring. Specify "
                "the -s flag to allow stripping the old hexstring.");
        }
    }
    regfree(&regex);

    if(workstring == NULL)
        workstring = string;
    crcsum = computeCRC32(filename);
    if(crcsum == 0)
        LERROR(ExitUnknownError, 0, "The file's CRC sum is zero.");
    sprintf(tagstr, "[%08lX]", crcsum);
    newstring = malloc(strlen(workstring) + 12*sizeof(char));
    if((p = strrchr(workstring, '.')) != NULL) {
        // has suffix
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
        // no suffix
        strcat(newstring, workstring);
        strcat(newstring, tagstr);
    }
    r = strdup(dirname((char*)filename));
    p = pathcat(r, string);
    q = pathcat(r, newstring);
    if(rename((const char*) p, (const char*) q) != 0)
        LERROR(EXIT_FAILURE, errno, "failed call to rename()");
    free(p);
    free(q);
    free(workstring);
    free(newstring);
    return EXIT_SUCCESS;
}

int command_remove_tag(const char *filename) {
    char *str, *nstr, *p, *q;
    const char *d;

    check_access_flags(filename, F_OK | R_OK | W_OK, 1);
    str = get_basename((char*)filename);
    if((nstr = strip_tag((const char*) str)) == NULL)
        LERROR(ExitArgumentError, 0, "%s does not contain an hexstring", filename);
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

int main(int argc, char **argv) {
    int opt;
    int cmd = CmdIdle;
    int cmd_tag_flags = 0;

    while((opt = getopt(argc, argv, "+tchsr")) != -1) {
        switch(opt) {
            case 'r':
                cmd = CmdRmTag;
                break;
            case 's':
                cmd_tag_flags |= TAG_ALLOW_STRIP;
                break;
            case 'c':
                cmd = CmdCheck;
                break;
            case 't':
                cmd = CmdTag;
                break;
            case 'h':
                puts("crctk v" VERSION "\n"
                        "CRC32 Hexstring Toolkit\n"
                        "Copyright (C) 2014 2ion (asterisk!2ion!de)\n"
                        "Upstream: https://github.com/2ion/crctk\n"
                        "Usage: crctk [-hstc] <file>\n"
                        "Options:\n"
                        " -c Compute CRC32 and compare with the hexstring\n"
                        "    in the supplied filename.\n"
                        "    Return values: EXIT_SUCCESS: match\n"
                        "                   EXIT_FAILURE: no match\n"
                        "                   0xA: invalid argument\n"
                        "                   0xB: regex compilation error\n"
                        "                   0xC: unknown error\n"
                        " -t Tag file with a CRC32 hexstring. Aborts if\n"
                        "    the filename does already contain a tag.\n"
                        " -s Supplements -t: strip eventually existing tag\n"
                        "    and compute a new CRC32 hexstring.\n"
                        "    Return values: EXIT_SUCCESS: success\n"
                        "                   EXIT_FAILURE: generic failure\n"
                        "                   Rest as above.\n"
                        " -r If the file is tagged, remove the tag.\n"
                        " -h Print this message and exit successfully.");
                return EXIT_SUCCESS;
            default:
                LERROR(ExitArgumentError, 0, "unknown option: %c", opt);
        }
    }
    if(optind >= argc)
        LERROR(ExitArgumentError, 0,
                "too few arguments. Use the -h flag "
                "to obtain usage information.");
    switch(cmd) {
        case CmdIdle:
            puts("No command flag set.");
            break;
        case CmdCheck:
            return command_check(argv[argc-1]);
        case CmdTag:
            return command_tag(argv[argc-1], cmd_tag_flags);
        case CmdRmTag:
            return command_remove_tag(argv[argc-1]);
    }
    return EXIT_SUCCESS;
}
