#include "database.h"

extern const char *dbiofile;

int db_open(const char *file, struct DB *db, struct DBBackend *be) {
  assert(file != NULL);
  assert(db != NULL);
  assert(be != NULL);
  // init
  db->file = NULL;
  db->fd = -1;
  db->backend = be;

  if((db->fd = be->open(file, db->open_flags)) == -1) {
    LERROR(0, errno, "%s: opening the database failed", be->name);
    return -1;
  }
  db->file = strdup(file);
  assert(db->file != NULL);
  return 0;
}

int db_write(struct DB *db, const struct DBItem *dbi) {
  assert(db != NULL);
  assert(dbi != NULL);
  DB_FD_CHECK(db);
  DB_WRITABLE_CHECK(db);
  return db->backend->write(db, dbi);
}

int db_read(struct DB *db, struct DBItem *dbi) {
  assert(db != NULL);
  assert(dbi != NULL);
  DB_FD_CHECK(db);
  DB_READABLE_CHECK(db);
  return db->backend->read(db, dbi);
}

int db_close(struct DB *db) {
  return 0;
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
