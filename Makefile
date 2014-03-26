.PHONY: clean install 
prefix ?= /usr/local
version_ ?= 0.3.100
prefix ?= $(HOME)/bin
gccflags = -g -Wall -Os -march=native -DVERSION=\"$(version)\"
cflags = $(gccflags) $(shell pkg-config --cflags kyotocabinet)
ldflags	= -lz $(shell pkg-config --libs kyotocabinet)
havegit=$(shell test -d .git &>/dev/null; echo $$?)
ifeq ($(havegit), 0)
version = $(version_)-$(shell git rev-parse --short HEAD)
else
version = $(version_)
endif
bindir = binaries-$(shell uname -m)

SOURCES = $(filter-out src/crctk.c,$(wildcard src/*.c))
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

crctk: $(OBJECTS) src/crctk.c
	gcc $(cflags) -o $@ $^ $(ldflags)

$(bindir)/crctk-$(version).bin: crctk
	mkdir -p $(bindir)
	cp crctk $(bindir)/crctk-$(version).bin
	strip $(bindir)/crctk-$(version).bin

src/%.o: src/%.c
	gcc -c -o $@ $<

librealpathtest: test/realpathtest.c $(OBJECTS)
	gcc $(cflags) -Isrc -o$@ $^ $(ldflags)

newcrclibtest: test/newcrclibtest.c $(OBJECTS)
	gcc $(cflags) -Isrc -o$@ $^ $(ldflags)

clean:
	rm -f $(OBJECTS) crctk librealpathtest

install: crctk
	@echo Ensuring $(prefix)/bin exists
	@install -m 755 -d $(prefix)/bin
	@echo Installing the binary to $(prefix)/bin
	@install -m 755 crctk $(prefix)/bin
