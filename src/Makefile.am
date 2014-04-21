bin_PROGRAMS = crctk ncrctk

ncrctk_SOURCES = ncrctk.cc
#ncrctk_CFLAGS = $(ZLIB_CFLAGS) $(KYOTOCABINET_CFLAGS)  $(CWDIGET_CFLAGS)
ncrctk_CXXFLAGS = $(ZLIB_CXXFLAGS) $(KYOTOCABINET_CXXFLAGS) $(CWIDGET_CFLAGS) $(SIGCPP_CFLAGS)
ncrctk_LDFLAGS = $(ZLIB_LIBS) $(KYOTOCABINET_LIBS) $(CWIDGET_LIBS) $(SIGCPP_LIBS)


crctk_SOURCES = command_calc_batch.c command_calc.c command_check_batch.c command_check.c command_check_hexstring.c command_delete.c command_help.c command_idle.c command_list.c command_merge.c command_remove_tag.c command_tag.c crc.c crctk.c database.c realpath.c util.c
crctk_CFLAGS = $(ZLIB_CFLAGS) $(KYOTOCABINET_CFLAGS)
crctk_LDFLAGS = $(ZLIB_LIBS) $(KYOTOCABINET_LIBS)
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
