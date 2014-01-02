/*
 * crctk - tool for managing files tagged with a CRC32 hexstring
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

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), (__func__), (__LINE__), __VA_ARGS__)

const char *crcregex = "[[:xdigit:]]\\{8\\}";
enum { ExitMatch = EXIT_SUCCESS, ExitNoMatch = EXIT_FAILURE, ExitArgumentError = 10,
    ExitRegexError = 11, ExitUnknownError = 12};
enum { CmdCheck, CmdTag };
enum { TAG_ALLOW_STRIP = 1 << 0 };

static unsigned long getFileSize(const char*);
static unsigned long computeCRC32(const char*);
static int command_check(const char*);
static int command_tag(const char*,int);
static inline void compile_regex(regex_t*, const char*, int);
void usage(void);

void usage(void) {
    puts("Usage: ");
}

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
    LERROR(0, 0, "size of %s has been calculated to be 0. Cannot calculate CRC", filename);
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
        LERROR(ExitUnknownError, 0, "received at least one NULL argument.");
    if((ci = regcomp(regex, regexpr, cflags)) != 0) {
        regex_errbuf_size = regerror(ci, regex, NULL, 0);
        regex_errbuf = malloc(regex_errbuf_size);
        regerror(ci, regex, regex_errbuf, regex_errbuf_size);
        if(regex_errbuf == NULL)
            LERROR(ExitRegexError, 0, "regex error: failed to allocate memory for the regex error message.");
        LERROR(ExitRegexError, 0, "%s", regex_errbuf);
    }
}

int command_check(const char *filename) {
    int  ci, ti;                        
    unsigned long compcrc;              
    unsigned long matchcrc;             
    char *string;                       
    char results[9];                    
    struct stat stbuf;
    regmatch_t rmatch;                  
    regex_t regex;                      

    if(access(filename, F_OK | R_OK) != 0)
        LERROR(ExitArgumentError, errno, "%s", filename);
    if(stat(filename, &stbuf) != 0)
        LERROR(ExitArgumentError, errno, "%s", filename);
    if(S_ISDIR(stbuf.st_mode))
        LERROR(ExitArgumentError, 0, "%s is a directory", filename);
    if((string = basename((char*)filename)) == NULL)
        LERROR(ExitArgumentError, 0, "could not extract specified path's basename.");

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
            LERROR(ExitNoMatch, 0, "the filename does not contain a CRC32 hexstring.");
            return ExitNoMatch; // Not reached
    }
    regfree(&regex);

    /* eval */

    compcrc = computeCRC32(filename);
    matchcrc = (unsigned long) strtol(results, NULL, 16);
    if(compcrc != matchcrc) {
        printf("mismatch: filename(%08lX) != computed(%08lX)\n", matchcrc, compcrc);
        return ExitNoMatch;
    } else {
        printf("match: filename(%08lX) == computed(%08lX)\n", matchcrc, compcrc);
        return ExitMatch;
    }
}

int command_tag(const char *filename, int flags) {
    char *string;
    char *newstring;
    char *workstring = NULL;
    char tagstr[11];
    char *p, *q, *r;
    int i;
    regex_t regex;
    regmatch_t rmatch;
    unsigned long crcsum;
    struct stat stbuf;

    if(access(filename, F_OK | R_OK) != 0)
        LERROR(ExitArgumentError, errno, "%s", filename);
    if(stat(filename, &stbuf) != 0)
        LERROR(ExitArgumentError, errno, "%s", filename);
    if(S_ISDIR(stbuf.st_mode))
        LERROR(ExitArgumentError, 0, "%s is a directory", filename);
    if((string = basename((char*)filename)) == NULL)
        LERROR(ExitArgumentError, 0, "could not extract specified path's basename.");

    compile_regex(&regex, crcregex, REG_ICASE | REG_NOSUB);
    if(regexec(&regex, string, 0, 0, 0) == 0) {
        if ((flags & TAG_ALLOW_STRIP) == TAG_ALLOW_STRIP) {
            // strip old CRC hexstring
            regfree(&regex);
            compile_regex(&regex, "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?", REG_ICASE);
            if(regexec(&regex, string, 1, &rmatch, 0) == REG_NOMATCH)
                LERROR(ExitRegexError, 0, "a regex that should have matched didn't match. Escaping ...");
            workstring = malloc((strlen(string) + 1 - (rmatch.rm_eo - rmatch.rm_so)) * sizeof(char));
            if(workstring == NULL)
                LERROR(ExitUnknownError, 0, "memory allocation error");
            for(p = string, q = &string[rmatch.rm_so], i=0;
                    p < q; ++p)
                workstring[i++] = *p;
            for(p = &string[rmatch.rm_eo]; *p; ++p)
                workstring[i++] = *p;
            workstring[i] = '\0';
        } else {
            LERROR(EXIT_FAILURE,0,"filename already contains a CRC hexstring. Specify the -s flag to allow stripping the old hexstring.");
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
    p = calloc(strlen(r) + strlen(string) + 2, sizeof(char));
    strcat(p, r);
    strcat(p, "/");
    strcat(p, string);
    q = calloc(strlen(r) + strlen(newstring) + 2, sizeof(char));
    strcat(q, r);
    strcat(q, "/");
    strcat(q, newstring);
    if(rename((const char*) p, (const char*) q) != 0)
        LERROR(EXIT_FAILURE, errno, "failed call to rename()");
    free(p);
    free(q);
    free(workstring);
    free(newstring);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    int opt;
    int cmd = CmdCheck;
    int cmd_tag_flags = 0;

    while((opt = getopt(argc, argv, "+tchs")) != -1) {
        switch(opt) {
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
                usage();
                return EXIT_SUCCESS;
            default:
                LERROR(ExitArgumentError, 0, "unknown option: %c", opt);
        }
    }
    if(optind >= argc)
        LERROR(ExitArgumentError, 0, "too few arguments. Use the -h flag to obtain usage information.");
    switch(cmd) {
        case CmdCheck:
            return command_check(argv[argc-1]);
        case CmdTag:
            return command_tag(argv[argc-1], cmd_tag_flags);
    }
    return EXIT_SUCCESS; // not reached
}
