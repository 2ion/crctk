#ifndef COMMAND_CALC_BATCH_H
#define COMMAND_CALC_BATCH_H

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "crctk.h"
#include "util.h"
#include "database.h"

int command_calc_batch(int, char**, int, int);

#endif
