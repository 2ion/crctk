#ifndef CRC32_H
#define CRC32_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>
#include "util.h"

unsigned long getFileSize(const char*);
unsigned long computeCRC32(const char*);

#endif /* CRC32_H */
