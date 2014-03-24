#ifndef COMMAND_CHECK_BATCH_H
#define COMMAND_CHECK_BATCH_H

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cdb.h>

#include "crctk.h"
#include "util.h"
#include "database.h"

int command_check_batch(int, char**, int, int);

#endif /* COMMAND_CHECK_BATCH_H */
