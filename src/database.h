/*
 * crctk - CRC32 Hexstring Toolkit
 * Copyright (C) 2014 Jens Oliver John <dev@2ion.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * */

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
#define DBITEM_NULL { NULL, 0, 0, NULL, NULL }
#define DBFINDER_NULL { NULL, NULL }
#define DBITEM_SET_NULL(e) (e).kbuf=NULL;(e).kbuflen=0;(e).crc=0;(e).next=0;
#define DBFINDER_SET_NULL(e) (e).db=NULL;(e).cur=NULL;

struct DBItem {
  char *kbuf;
  size_t kbuflen;
  uint32_t crc;
  struct DBItem *next;
  struct DBItem *prev;
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
int DB_find_remove(struct DBFinder *dbf, const char *key);
int DB_find_close(struct DBFinder *dbf);
int DB_make_paths_absolute(const char *path);
void DB_item_free(struct DBItem*);
struct DBItem* DB_item_alloc(void);
struct DBItem* DB_item_new(const char *kbuf, size_t kbuflen, uint32_t crc);
struct DBItem* DB_item_append(struct DBItem* parent, const char *kbuf,
    size_t kbuflen, uint32_t crc);
struct DBItem* DB_item_append_terminator(struct DBItem*);
char* DB_getkcdbiofile(const char *path);

#endif /* DATABASE_H */
