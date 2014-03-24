.PHONY: clean install
prefix ?= /usr/local
version_ ?= 0.3.99
prefix ?= $(HOME)/bin
gccflags = -Wall -Os -march=native -DVERSION=\"$(version)\"
cflags = $(gccflags) $(shell pkg-config --cflags libcdb kyotocabinet)
ldflags	= -lz $(shell pkg-config --libs libcdb kyotocabinet)
havegit=$(shell test -d .git &>/dev/null; echo $$?)
ifeq ($(havegit), 0)
version = $(version_)-$(shell git rev-parse --short HEAD)
else
version = $(version_)
endif

SOURCES = $(filter-out src/crctk.c,$(wildcard src/*.c))
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

crctk: $(OBJECTS) src/crctk.c
	gcc $(cflags) -o $@ $^ $(ldflags)

src/%.o: src/%.c
	gcc -c -o $@ $<

librealpathtest: test/realpathtest.c $(OBJECTS)
	gcc $(cflags) -Isrc -o$@ $^ $(ldflags)

newcrclibtest: test/newcrclibtest.c $(OBJECTS)
	gcc $(cflags) -Isrc -o$@ $^ $(ldflags)

clean:
	rm -f $(OBJECTS) crctk librealpathtest newcrclibtest
	rm -rf lib/cdb-*

cdb:
	wget -qOlib/cdb-0.75.tar.gz http://cr.yp.to/cdb/cdb-0.75.tar.gz
	cd lib ; tar -xf cdb-0.75.tar.gz
	cd lib/cdb-0.75 ; patch < ../fix-errno.patch
	make -C lib/cdb-0.75
	cp lib/cdb-0.75/libcdb.a lib/

install: crctk
	@echo Ensuring $(prefix)/bin exists
	@install -m 755 -d $(prefix)/bin
	@echo Installing the binary to $(prefix)/bin
	@install -m 755 crctk $(prefix)/bin
