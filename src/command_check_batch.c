#include "command_check_batch.h"

extern const char *dbiofile;

int command_check_batch(int argc, char **argv, int optind,
    int cmdflags) {
  struct cdb db;
  int fd;
  char statickbuf[255];
  char *kbuf = statickbuf;
  size_t kbuflen = 255;
  int kbuf_isstatic = 1;
  uint32_t vbuf, crc;
  unsigned up, vpos, vlen, klen, kpos;
  int i, err;
  char *x;

  check_access_flags(dbiofile, F_OK | R_OK, 1);
  if((fd = open(dbiofile, O_RDONLY)) == -1)
    LERROR(EXIT_FAILURE, errno, "could not open cdb file: %s",
        dbiofile);
  if(cdb_init(&db, fd) != 0)
    LERROR(EXIT_FAILURE, 0, "cdb_init() failed");
  
  if(optind < argc) { for(i=optind; i < argc; ++i) {
    klen = sizeof(char) * (strlen(argv[i]) + 1);
    if((err = cdb_find(&db, argv[i], klen)) > 0) {
      printf("%s: <%s> ... ", dbiofile, argv[i]);
      vpos = cdb_datapos(&db);
      if((vlen = cdb_datalen(&db)) != sizeof(uint32_t)) {
        LERROR(0,0, "%s: invalid data value /%s", dbiofile,argv[i]);
        continue;
      }
      if(cdb_read(&db, &vbuf, vlen, vpos) != 0) {
        LERROR(0,0, "%s: cdb_read() failed on key <%s>", dbiofile,
            argv[i]);
        continue;
      }
      //
      if(cmdflags & CHECK_BATCH_PREFER_HEXSTRING) {
        x = get_tag(argv[i]);
        if(x == NULL)
          LERROR(0,0, "ERROR: option -x: argument does not contain an "
              "hexstring: %s", argv[i]);
        else {
          if((crc = strtol((const char*)x, NULL, 16)) == vbuf)
            printf("OK (%08X)[x]\n", crc);
          else
            printf("ERROR (is: %08X, db: %08X)[x]\n", crc, vbuf);
          free(x);
        }
      } else {
        //
        if(check_access_flags_v(argv[i], F_OK | R_OK, 1) != 0)
          LERROR(0,0, "ERROR: file is not accessible");
        if((crc = compute_crc32(argv[i])) != 0) {
          if(crc == vbuf)
            printf("OK (%08X)\n", crc);
          else
            printf("ERROR (is: %08X, db: %08X)\n", crc, vbuf);
        } else
          printf("ERROR (CRC32 is zero)\n");
        //
      }
    } else if(err == 0) {
      printf("%s: Not entry found for path: %s\n", dbiofile, argv[i]);
    } else {
      LERROR(0,0, "cdb_find() failed: unknown error");
    } 
  }} else {
    // check all the paths from the database
    cdb_seqinit(&up, &db);
    while(cdb_seqnext(&up, &db) > 0) {
      klen = cdb_keylen(&db);
      kpos = cdb_keypos(&db);
      vlen = cdb_datalen(&db);
      vpos = cdb_datapos(&db);
      if((vlen = cdb_datalen(&db)) != sizeof(uint32_t)) {
        //FIXME: output key name
        LERROR(0,0, "%s: skipping entry: wrong data size (keypos=%u, "
            "vlen=%u)", dbiofile, kpos, vlen);
        continue;
      }
      helper_manage_stackheapbuf(kbuf, &kbuflen, &kbuf_isstatic, klen);
      if(cdb_read(&db, kbuf, klen, kpos) != 0) {
        LERROR(0,0, "cdb_read() failed for kbuf");
        continue;
      }
      if(cdb_read(&db, &vbuf, vlen, vpos) != 0) {
        LERROR(0,0, "cdb_read() failed for &vbuf");
        continue;
      }
      printf("%s: <%s> ... ", dbiofile, kbuf);
      if(check_access_flags_v(kbuf, F_OK | R_OK, 1) != 0) {
        LERROR(0,0, "ERROR: file is not accessible");
        continue;
      }
      if((crc = compute_crc32(kbuf)) != 0) {
        if(crc == vbuf)
          printf("OK (%08X)\n", crc);
        else
          printf("ERROR (is: %08X, db: %08X)\n", crc, vbuf);
      } else
        printf("ERROR (CRC32 is zero)\n");
    }
  }
  if(kbuf_isstatic == 0)
    free(kbuf);
  cdb_free(&db);
  close(fd);
  return EXIT_SUCCESS;
}
