#include "database.h"

extern const char *dbiofile;

int DB_write(const char *path, const struct DBItem *dbi, int do_truncate) {
  assert(path != NULL);
  assert(dbi != NULL);
  assert(dbi->kbuf != NULL);
  KCDB *db;
  char *kc_dbiofile = DB_getkcdbiofile(path);
  puts(kc_dbiofile);
  const struct DBItem *e = dbi;
  int kcdbopen_flags = do_truncate == 1 ?
    (KCOWRITER | KCOCREATE | KCOTRUNCATE) :
    (KCOWRITER | KCOCREATE);

  db = kcdbnew();
  
  if(!kcdbopen(db, kc_dbiofile, kcdbopen_flags)) {
    LERROR(0, 0, "kcdbopen() error: %s", kcecodename(kcdbecode(db)));
    return -1;
  }
  do {
    if(!kcdbset(db,
          (const char*)e->kbuf,
          (size_t)e->kbuflen,
          (const char*)&e->crc,
          sizeof(uint32_t))) {
      LERROR(0, 0, "kcdbset: could not add record (%s, %d): %s",
          e->kbuf, e->crc, kcecodename(kcdbecode(db)));
    } else
      LERROR(0,0, "Wrote: (%s) -> (%08X)[length:%d]", e->kbuf, e->crc,(int)sizeof(e->crc));
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

  if(!kcdbopen(db, kc_dbiofile, KCOREADER)) {
    LERROR(0, 0, "kcdbopen: could not open the db: %s",
        kcecodename(kcdbecode(db)));
    return -1;
  }
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

char* DB_getkcdbiofile(const char *path) {
  char *s = malloc(sizeof(char)*(strlen(path) + 2 + strlen(CRCTK_DB_TUNINGSUFFIX)));
  if(s==NULL) LERROR(EXIT_FAILURE, errno, "malloc() failed");
  memcpy(s, dbiofile, sizeof(char)*(strlen(path)+1));
  return strcat(s, CRCTK_DB_TUNINGSUFFIX);
}

int db2array(const char *dbfile, struct DBItem *first) {
  assert(dbfile != NULL);
  assert(first != NULL);
  struct cdb db;
  unsigned up, kpos, klen, vpos, vlen;
  struct DBItem *cur = first;
  int atfirst = 1;
  int fd;
  cur->next = first;

  if(check_access_flags_v(dbfile, F_OK | R_OK, 1) != 0) {
    LERROR(0,0, "file not accessible: %s", dbfile);
    return EXIT_FAILURE;
  }
  if((fd = open(dbfile, O_RDONLY)) == -1) {
    LERROR(0,errno, "%s", dbiofile);
    return EXIT_FAILURE;
  }
  if(cdb_init(&db, fd) != 0) {
    LERROR(0,0, "cdb_init() failed");
    goto cleanup_error;
  }
  cdb_seqinit(&up, &db);
  while(cdb_seqnext(&up, &db) > 0) {
    if(atfirst == 0) {
      cur->next = malloc(sizeof(struct DBItem));
      if(cur->next == NULL)
        LERROR(EXIT_FAILURE, errno, "malloc() failed");
      cur = cur->next;
      cur->next = NULL;
    }
    kpos = cdb_keypos(&db);
    klen = cdb_keylen(&db);
    vpos = cdb_datapos(&db);
    vlen = cdb_datalen(&db);
    if(vlen > sizeof(uint32_t)) {
      LERROR(0,0, "Skipping entry with a data size "
          "> sizeof(uint32_t)");
      continue;
    }
    cur->kbuflen = klen;
    if((cur->kbuf = malloc(klen)) == NULL)
      LERROR(EXIT_FAILURE, 0, "malloc() failed");
    if(cdb_read(&db, cur->kbuf, klen, kpos) != 0) {
      LERROR(0,0, "cdb_read() failed. Skipping key at pos=%u", kpos);
      continue;
    }
    if(cdb_read(&db, &cur->crc, vlen, vpos) != 0) {
      LERROR(0,0, "cdb_read() failed. Skipping data at pos=%u", vpos);
      continue;
    }
    if(atfirst == 1) atfirst = 0;
    cur->next = NULL;
  } // while
  cdb_free(&db);
  close(fd);
  return EXIT_SUCCESS;
cleanup_error:
  close(fd);
  return EXIT_FAILURE;
}
