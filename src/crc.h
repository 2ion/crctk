#ifndef CRC_H
#define CRC_H

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>
#include "util.h"

long get_file_size(const char*);
uint32_t compute_crc32(const char*);

#endif /* CRC_H */
