#include "database.h"

extern const char *dbiofile;

int DB_find_open(const char *path, struct DBFinder *dbf) {
  assert(path != NULL);
  assert(dbf != NULL);
  char *kc_dbiofile = DB_getkcdbiofile(path);
  dbf->db = kcdbnew();

  if(!kcdbopen(dbf->db, kc_dbiofile, KCOREADER)) {
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
          (size_t)e->kbuflen,
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

char* DB_getkcdbiofile(const char *path) {
  char *s = malloc(sizeof(char)*(strlen(path) + 2 + strlen(CRCTK_DB_TUNINGSUFFIX)));
  if(s==NULL) LERROR(EXIT_FAILURE, errno, "malloc() failed");
  memcpy(s, dbiofile, sizeof(char)*(strlen(path)+1));
  return strcat(s, CRCTK_DB_TUNINGSUFFIX);
}

