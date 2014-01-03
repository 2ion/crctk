version ?= 0.1-$(shell git rev-parse --short HEAD)
.PHONY: all

crctk: crctk.c
	gcc -Wall -O3 -DVERSION=\"$(version)\" -o $@ $< -lz
	strip $@

README: crctk
	./crctk -h > README

clean:
	-rm crctk README

push:
	git push github master --tags

pull:
	git pull github master --tags
