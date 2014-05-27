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

#include "database.h"

int DB_merge(const char *path, const char **pathlist, int do_truncate) {
  assert(path != NULL);
  assert(pathlist != NULL);
  KCDB *db = kcdbnew();
  char *kcdbiofile = DB_getkcdbiofile(path);
  KCDB **dbsrc = NULL;
  KCDB **dbsrcfinal = NULL;
  char **dbiofiles = NULL;
  int j = -1;
  int i = 0;
  int k = 0;
  int h = 0;
  int ret = 0;
  int kcwriteflags = (do_truncate == 1) ?
    (KCOWRITER | KCOCREATE | KCOTRUNCATE) :
    (KCOWRITER | KCOCREATE );

  /* open target database */
  if(!kcdbopen(db, kcdbiofile, kcwriteflags)) {
    LERROR(0, 0, "Could not open target database: %s: %s",
        path, kcecodename(kcdbecode(db)));
    kcdbdel(db);
    free(kcdbiofile);
    return -1;
  }

  /* open source databases */
  while(pathlist[++j]);
  dbiofiles = malloc(sizeof(char*)*j);
  if(dbiofiles == NULL)
    LERROR(EXIT_FAILURE, errno, "");
  dbsrc = malloc(sizeof(KCDB*)*j);
  if(dbsrc == NULL)
    LERROR(EXIT_FAILURE, errno, "");
  for(k = 0, i = 0; i < j; ++i) {
    dbiofiles[i] = DB_getkcdbiofile(pathlist[i]);
    dbsrc[i] = kcdbnew();
    if(!kcdbopen(dbsrc[i], dbiofiles[i], KCOREADER)) {
      printf("[%s]: skipping: is not a database (%s)\n",
          pathlist[i], kcecodename(kcdbecode(dbsrc[i])));
      kcdbdel(dbsrc[i]);
      dbsrc[i] = NULL;
      free(dbiofiles[i]);
      dbiofiles[i] = NULL;
    } else {
      printf("[%s] provides %d entries\n", pathlist[i], kcdbcount(dbsrc[i]));
      ++k;
    }
  }

  /* merge */
  dbsrcfinal = malloc(sizeof(KCDB*)*k);
  if(dbsrcfinal == NULL)
    LERROR(EXIT_FAILURE, errno, "");

  for(h = 0, i = 0; i < j && h < k; ++i)
    if(dbsrc[i]) {
      dbsrcfinal[h++] = dbsrc[i];
    }


  if(!kcdbmerge(db, dbsrcfinal, k, KCMSET)) {
    fprintf(stderr, "Merging failed\n");
    ret = -1;
  }

  /* cleanup */
  for(h = 0; h < k; ++h) {
    kcdbclose(dbsrcfinal[h]);
    kcdbdel(dbsrcfinal[h]);
  }
  for(i = 0; i < j; ++i) {
    if(dbiofiles[i] != NULL)
      free(dbiofiles[i]);
  }
  free(dbsrc);
  free(dbsrcfinal);
  free(dbiofiles);
  kcdbclose(db);
  kcdbdel(db);
  free(kcdbiofile);

  return ret;
}

int DB_find_open(const char *path, struct DBFinder *dbf) {
  assert(path != NULL);
  assert(dbf != NULL);
  char *kc_dbiofile = DB_getkcdbiofile(path);
  dbf->db = kcdbnew();

  if(!kcdbopen(dbf->db, kc_dbiofile, KCOWRITER)) {
    LERROR(0,0, "kcdbopen() error: %s", kcecodename(kcdbecode(dbf->db)));
    return -1;
  }
  dbf->cur = kcdbcursor(dbf->db);
  return 0;
}

int DB_find_close(struct DBFinder *dbf) {
  assert(dbf != NULL);
  kccurdel(dbf->cur);
  kcdbclose(dbf->db);
  kcdbdel(dbf->db);
  return 0;
}

int DB_find_get(struct DBFinder *dbf, const char *key, uint32_t *crcbuf) {
  assert(dbf != NULL);
  assert(key != NULL);
  assert(crcbuf != NULL);
  char *v;
  size_t vs;
  kccurjump(dbf->cur);
  if(!kccurjumpkey(dbf->cur, key, sizeof(char)*(strlen(key)+1))) {
    *crcbuf = 0;
    return -1;
  } 
  if((v = kccurgetvalue(dbf->cur, &vs, 0)) == NULL) {
    *crcbuf = 0;
    return -1;
  }
  if(vs != sizeof(uint32_t)) {
    LERROR(0, 0, "The requested key (%s) holds a value of the wrong size",
        key);
    *crcbuf = 0;
    return -1;
  }
  memcpy(crcbuf, v, sizeof(uint32_t));
  kcfree(v);
  return 0;
}

int DB_find_remove(struct DBFinder *dbf, const char *key) {
  assert(dbf != NULL);
  assert(key != NULL);
  kccurjump(dbf->cur);

  if(!kccurjumpkey(dbf->cur, key, sizeof(char)*(strlen(key)+1)))
    return -1;
  if(!kccurremove(dbf->cur))
    return -2;
  return 0;
}

int DB_write(const char *path, const struct DBItem *dbi, int do_truncate) {
  assert(path != NULL);
  assert(dbi != NULL);
  assert(dbi->kbuf != NULL);
  KCDB *db = kcdbnew();
  char *kc_dbiofile = DB_getkcdbiofile(path);
  const struct DBItem *e = dbi;
  int kcdbopen_flags = do_truncate == 1 ?
    (KCOWRITER | KCOCREATE | KCOTRUNCATE) :
    (KCOWRITER | KCOCREATE);
  
  if(!kcdbopen(db, kc_dbiofile, kcdbopen_flags)) {
    LERROR(0, 0, "kcdbopen() error: %s", kcecodename(kcdbecode(db)));
    return -1;
  }
  do {
    if(!kcdbset(db,
          (const char*)e->kbuf,
          e->kbuflen,
          (const char*)&e->crc,
          sizeof(uint32_t)))
      LERROR(0, 0, "kcdbset: could not add record (%s, %d): %s",
          e->kbuf, e->crc, kcecodename(kcdbecode(db)));
  } while((e = (const struct DBItem*)e->next) != NULL);
  if(!kcdbclose(db)) {
    free(kc_dbiofile);
    LERROR(0, 0, "kcdbclose() error: %s", kcecodename(kcdbecode(db)));
    return -1;
  }
  kcdbdel(db);
  free(kc_dbiofile);
  return 0;
}

int DB_read(const char *path, struct DBItem *dbi) {
  assert(path != NULL);
  assert(dbi != NULL);
  char *kc_dbiofile = DB_getkcdbiofile(path);
  KCDB *db = kcdbnew();
  KCCUR *cur;
  size_t ksize, vsize;
  char *kbuf;
  const char *vbuf;
  struct DBItem *curi = dbi;
  int at_first = 1;

  if(!kcdbopen(db, kc_dbiofile, KCOREADER))
    return -1;
  cur = kcdbcursor(db);
  kccurjump(cur);
  while((kbuf = kccurget(cur, &ksize, &vbuf, &vsize, 1)) != NULL) {
    if(vsize > sizeof(uint32_t)) {
      LERROR(0,0, "Skipping record with wrong value size");
      kcfree(kbuf);
      continue;
    }
    if(at_first == 0) {
      curi->next = DB_item_alloc();
      curi->next->prev = curi;
      curi = curi->next;
      curi->next = NULL;
    }
    curi->kbuf = malloc(ksize);
    if(curi->kbuf==NULL) LERROR(EXIT_FAILURE, errno, "malloc() failed");
    curi->kbuflen = ksize;
    memcpy(curi->kbuf, kbuf, ksize);
    memcpy(&curi->crc, vbuf, vsize);
    kcfree(kbuf);
    if(at_first == 1) at_first = 0;
    curi->next = NULL;
  }
  kccurdel(cur);
  if(!kcdbclose(db)) {
    LERROR(0, 0, "kcdbclose() error: %s", kcecodename(kcdbecode(db)));
    return -1;
  }
  kcdbdel(db);
  free(kc_dbiofile);
  return 0;
}

struct DBItem* DB_item_alloc(void) {
  struct DBItem *e = calloc(1, sizeof(struct DBItem));
  if(e == NULL)
    LERROR(EXIT_FAILURE, errno, "memory allocation error");
  return e;
}

struct DBItem* DB_item_new(const char *kbuf, size_t kbuflen, uint32_t crc) {
  assert(kbuf != NULL);
  struct DBItem *e = DB_item_alloc();
  e->kbuf = malloc(kbuflen);
  if(e->kbuf == NULL) LERROR(EXIT_FAILURE, errno, "memory allocation error");
  memcpy(e->kbuf, kbuf, kbuflen);
  e->crc = crc;
  return e;
}

struct DBItem* DB_item_append(struct DBItem *parent, const char *kbuf,
    size_t kbuflen, uint32_t crc) {
  assert(kbuf != NULL);
  if(parent == NULL) return DB_item_new(kbuf, kbuflen, crc);
  struct DBItem *e = DB_item_new(kbuf, kbuflen, crc);
  parent->next = e;
  return e;
}

void DB_item_free(struct DBItem *dbi) {
  assert(dbi != NULL);
  if(dbi == NULL)
    LERROR(EXIT_FAILURE, 0, "invalid pointer passed (NULL)");
  struct DBItem *e = dbi;
  struct DBItem *f = NULL;
  do {
    if(e->kbuf != NULL) free(e->kbuf);
    f = e;
  } while((e = e->next) != NULL);
  e = f;
  do {
    if(e->next != NULL) free(e->next);
  } while((e = e->prev) != NULL);
  free(e);
}

char* DB_getkcdbiofile(const char *path) {
  char *s = malloc(sizeof(char)*(strlen(path) + 2 + strlen(CRCTK_DB_TUNINGSUFFIX)));
  if(s==NULL) LERROR(EXIT_FAILURE, errno, "malloc() failed");
  memcpy(s, path, sizeof(char)*(strlen(path)+1));
  return strcat(s, CRCTK_DB_TUNINGSUFFIX);
}

/* We do the following: read the existing database into an in-memory
 * stash db while changing relative into absolute paths on-the-fly.
 * Afterwards, we merge the in-memory database back into the on-disk
 * one with merge mode set to KCMSET. This is because we need to
 * read the contents using a cursor anyway and because this is already
 * implemented in DB_read(). This function can thus be rewritten for
 * further optimization at a later point.
 */
int DB_make_paths_absolute(const char *path) {
  assert(path != NULL);
  int ret = 0;
  struct DBItem dbi = DBITEM_NULL;
  struct DBItem *e = NULL;
  char rpath[PATH_MAX]; /* FIXME: rewrite to escape from PATH_MAX */
  size_t pathlen = 0;
  char *kcdbiofile = DB_getkcdbiofile(path);
  KCDB *tdb = kcdbnew();
  KCDB *mdb[1];
  mdb[0] = kcdbnew();

  if(DB_read(path, &dbi) != 0)
    return -1;
  if(dbi.kbuf == NULL)
    return 0;

  if(!kcdbopen(mdb[0], ":", KCOWRITER))
    LERROR(EXIT_FAILURE, 0, "in-memory database error: %s",
        kcecodename(kcdbecode(mdb[0])));
  if(!kcdbopen(tdb, kcdbiofile, KCOWRITER | KCOTRUNCATE)) {
    LERROR(0, 0, "Could not open the database: %s: %s",
        path, kcecodename(kcdbecode(tdb)));
    free(kcdbiofile);
    return -1;
  }

  e = &dbi;
  do {
    if(e->kbuf[0] != '/') {
      if(realpath(e->kbuf, rpath) == NULL)
        LERROR(EXIT_FAILURE, errno, "realpath() -- memory allocation error");
      pathlen = strlen(rpath) + 1;
      LERROR(0,0, "%s -> %s\n", e->kbuf, rpath);
      if(pathlen > e->kbuflen)
        if((e->kbuf = realloc(e->kbuf, pathlen)) == NULL)
          LERROR(EXIT_FAILURE, errno, "realloc() -- memory allocation error");
      memcpy(e->kbuf, rpath, pathlen);
      e->kbuflen = pathlen;
      /* zero for next iteration */
      pathlen = 0;
    }
    kcdbset(mdb[0],
        (const char*)e->kbuf,
        e->kbuflen,
        (const char*)&e->crc,
        sizeof(uint32_t));
  } while((e = e->next) != NULL);

  if(!kcdbmerge(tdb, mdb, 1, KCMSET)) {
    fprintf(stderr, "merging failed.");
    ret = -1;
  }

  if(dbi.next != NULL)
    DB_item_free(dbi.next);

  kcdbclose(tdb);
  kcdbclose(mdb[0]);
  free(kcdbiofile);

  return ret;
}
