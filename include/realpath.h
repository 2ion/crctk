#ifndef REALPATH_H
#define REALPATH_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern int my_splitpath(const char *path,
    char **dir, size_t *dirlen,
    char **base, size_t *baselen);
extern int my_realpath(const char *path);

#endif
