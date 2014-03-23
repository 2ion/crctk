#ifndef DATABASE_H
#define DATABASE_H

#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"

#define DB_FD_CHECK(dbptr) if((dbptr)->fd == -1) {\
  LERROR(0,0, "%s: database %s is not open", (dbptr)->backend->name, \
          (dbptr)->file); \
      return -1; \
} 
#define DB_WRITABLE_CHECK(db) if((db)->open_flags != O_WRONLY || (db)->open_flags != O_RDWR) { \
  LERROR(0,0, "%s: database %s is not open for writing", (db)->backend->name, \
      (db)->file); \
  return -1; \
}
#define DB_READABLE_CHECK(db) if((db)->open_flags != O_RDONLY || (db)->open_flags != O_RDWR) {\
  LERROR(0,0, "%s: database %s is not open for reading", (db)->backend->name, \
      (db)->file); \
  return -1; \
}

struct DBItem {
  char *kbuf;
  int kbuflen;
  uint32_t crc;
  struct DBItem *next;
};

struct DB {
  char *file;
  int fd;
  int open_flags;
  struct DBBackend *backend;
};

struct DBBackend {
  char *name;
  int(*open)(const char*,int);
  int(*close)(int);
  int(*write)(struct DB*, const struct DBItem*);
  int(*read)(struct DB*, const struct DBItem*);
};

int DB_open(const char *file, struct DB *db, struct DBBackend *be);
int DB_write(struct DB *db, const struct DBItem *dbi);
int DB_read(struct DB *db, struct DBItem *dbi);
int DB_close(struct DB *db);

int db2array(const char*, struct DBItem*);

#endif /* DATABASE_H */
