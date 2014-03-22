version_	?= 0.3.4
prefix 		?= $(HOME)/bin
gccflags	 = -Wall -Os -march=native -DVERSION=\"$(version)\"
cflags 		 = $(gccflags) $(shell pkg-config --cflags libcdb)
ldflags		 = -lz $(shell pkg-config --libs libcdb)
.PHONY: 		 clean push pull install uninstall doc test

havegit=$(shell test -d .git &>/dev/null; echo $$?)
ifeq ($(havegit), 0)
	version = $(version_)-$(shell git rev-parse --short HEAD)
else
	version = $(version_)
endif

all: crctk doc

crctk: src/crctk.c realpath.o src/crctk.h
	gcc $(cflags) -o $@ $^ $(ldflags)

realpath.o: src/realpath.c
	gcc -c $(cflags) $< $(ldflags)

color.o: src/color.c
	gcc -c $(cflags) $< $(ldflags)

test: librealpathtest
	./librealpathtest

librealpathtest: src/realpathtest.c realpath.o 
	gcc $(cflags) -o $@ $^

doc:
	+make -C doc

README.md: doc/README.head crctk 
	cat $< > $@
	./crctk -h >> $@
	echo '```' >> $@

packages/crctk-$(version_).tar.xz: crctk
	-mkdir packages
	git archive master | xz > packages/crctk-$(version_).tar.xz

clean:
	rm -f crctk README.md *.o librealpathtest

push:
	git push github master --tags

pull:
	git pull github master --tags

install: crctk
	install -m 700 -d $(prefix)
	install -m 700 -t $(prefix) crctk

uninstall:
	-rm $(prefix)/crctk
