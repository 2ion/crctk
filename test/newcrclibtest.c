#include "crc.h"
#include "crc32.h"
#include <stdio.h>
#include <inttypes.h>

int main(int argc, char **argv) {
  uint32_t crcNEW = 0;
  unsigned long crcOLD = 0L;
  crcNEW = compute_crc32(argv[argc-1]);
  crcOLD = computeCRC32(argv[argc-1]);
  printf("crcNEW = %08X\ncrcOLD = %08lX\n",
      crcNEW, crcOLD);
  printf("sizeof(uint32_t)=%" PRIuPTR, sizeof(uint32_t));
  puts("");
  printf("sizeof(unsigned long)=%" PRIuPTR, sizeof(unsigned long));
  puts("");
  return 0;
}
