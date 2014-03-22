#include "database.h"

int db_open(const char *file, struct DB *db, struct DBBackend *be) {
  assert(file != NULL);
  assert(db != NULL);
  assert(be != NULL);
  // init
  db->file = NULL;
  db->readOnly = O_RDONLY;
  db->fd = -1;
  db->backend = be;

  return 0;
}

int db_write(struct DB *db, const struct DBItem *dbi) {
  return 0;
}

int db_read(struct DB *db, struct DBItem *dbi) {
  return 0;
}

int db_close(struct DB *db) {
  return 0;
}


