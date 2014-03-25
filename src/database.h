#ifndef DATABASE_H
#define DATABASE_H

#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"

#include <kclangc.h>

#define CRCTK_DB_TUNINGSUFFIX "#type=kct#zcomp=gz#opts=cs#apow=0#bnum=30"
#define DBITEM_NULL { NULL, 0, 0, NULL }

struct DBItem {
  char *kbuf;
  size_t kbuflen;
  uint32_t crc;
  struct DBItem *next;
};

struct DBFinder {
  KCDB *db;
  KCCUR *cur;
};

int DB_write(const char *path, const struct DBItem *dbi, int do_truncate);
int DB_read(const char *path, struct DBItem *dbi);
int DB_merge(const char *path, const char **pathlist, int do_truncate);
int DB_find_open(const char *path, struct DBFinder *dbf);
int DB_find_get(struct DBFinder *dbf, const char *key, uint32_t *crcbuf);
int DB_find_close(struct DBFinder *dbf);

struct DBItem* DB_item_alloc(void);
struct DBItem* DB_item_new(const char *kbuf, size_t kbuflen, uint32_t crc);
struct DBItem* DB_item_append(struct DBItem* parent, const char *kbuf,
    size_t kbuflen, uint32_t crc);
struct DBItem* DB_item_append_terminator(struct DBItem*);

char* DB_getkcdbiofile(const char *path);


#endif /* DATABASE_H */
