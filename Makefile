.PHONY: clean
version_	?= 0.3.4
prefix ?= $(HOME)/bin
gccflags	= -Wall -Os -march=native -DVERSION=\"$(version)\"
cflags = $(gccflags) $(shell pkg-config --cflags libcdb)
ldflags	= -lz $(shell pkg-config --libs libcdb)
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
