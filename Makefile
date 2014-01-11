version_ ?= 0.2
version = $(version_)-$(shell git rev-parse --short HEAD)
prefix ?= $(HOME)/bin
.PHONY: clean push pull install uninstall

crctk: crctk.c
	gcc -Wall -O3 -DVERSION=\"$(version)\" -o $@ $< -lz 
	strip $@

README: crctk
	./crctk -h > README

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
