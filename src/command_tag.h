#ifndef COMMAND_TAG_H
#define COMMAND_TAG_H

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "crctk.h"
#include "util.h"
#include "database.h"

int command_tag(int, char**, int, int);

#endif /*COMMAND_TAG_H*/
