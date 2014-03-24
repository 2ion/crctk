#ifndef COMMAND_CHECK_BATCH_H
#define COMMAND_CHECK_BATCH_H

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "crctk.h"
#include "util.h"
#include "database.h"

int command_check_batch(int, char**, int, int);

int command_check_batch_from_db(int argc, char **argv,
    int optind, int cmdflags);
int command_check_batch_from_argv(int argc, char **argv,
    int optind, int cmdflags);

#endif /* COMMAND_CHECK_BATCH_H */
