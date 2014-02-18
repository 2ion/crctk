version_	?= 0.3.3
prefix 		?= $(HOME)/bin
gccflags	 = -Wall -Os -march=native -DVERSION=\"$(version)\"
cflags 		 = $(gccflags) $(shell pkg-config --cflags libcdb)
ldflags		 = -lz $(shell pkg-config --libs libcdb)
.PHONY: 		 clean push pull install uninstall doc

havegit=$(shell test -d .git &>/dev/null; echo $$?)
ifeq ($(havegit), 0)
	version = $(version_)-$(shell git rev-parse --short HEAD)
else
	version = $(version_)
endif

all: crctk doc

crctk: crctk.c librealpath
	gcc $(cflags) -o $@ realpath.o $< $(ldflags)

librealpath: realpath.c
	gcc $(cflags) -c $< $(ldflags)

librealpathtest: librealpath realpathtest.c
	gcc $(cflags) -o $@ realpathtest.c realpath.o $(ldflags)

doc:
	make -C doc

README.md: crctk README.head
	cat README.head > $@
	./crctk -h >> $@
	echo '```' >> $@

packages/crctk-$(version_).tar.xz: crctk
	git archive master | xz > packages/crctk-$(version_).tar.xz

clean:
	-rm crctk README

push:
	git push github master --tags

pull:
	git pull github master --tags

install: crctk
	install -m 700 -d $(prefix)
	install -m 700 -t $(prefix) crctk

uninstall:
	-rm $(prefix)/crctk
