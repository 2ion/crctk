#ifndef DATABASE_H
#define DATABASE_H

#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"

#include <kclangc.h>

#define CRCTK_DB_TUNINGSUFFIX "#type=kch"

struct DBItem {
  char *kbuf;
  size_t kbuflen;
  uint32_t crc;
  struct DBItem *next;
};

int DB_write(const char *path, const struct DBItem *dbi, int do_truncate);
int DB_read(const char *path, struct DBItem *dbi);
struct DBItem* DB_item_alloc(void);
struct DBItem* DB_item_new(const char *kbuf, size_t kbuflen, uint32_t crc);
struct DBItem* DB_item_append(struct DBItem* parent, const char *kbuf,
    size_t kbuflen, uint32_t crc);
struct DBItem* DB_item_append_terminator(struct DBItem*);

int db2array(const char*, struct DBItem*);
char* DB_getkcdbiofile(const char *path);


#endif /* DATABASE_H */
