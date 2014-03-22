#ifndef DATABASE_H
#define DATABASE_H

#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"

struct DBItem {
  char *kbuf;
  int kbuflen;
  unsigned long crc;
  struct DBItem *next;
};

struct DBBackend {
  int(*open)(const char*);
  int(*close)(int);
  int(*write)(const struct DBItem*);
  int(*read)(const struct DBItem*);
};

struct DB {
  char *file;
  int fd;
  int readOnly;
  struct DBBackend *backend;
};

int DB_open(const char *file, struct DB *db, struct DBBackend *be);
int DB_write(struct DB *db, const struct DBItem *dbi);
int DB_read(struct DB *db, struct DBItem *dbi);
int DB_close(struct DB *db);

#endif /* DATABASE_H */
