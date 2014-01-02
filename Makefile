version ?= 0.1-$(shell git rev-parse --short HEAD)
.PHONY: all

all: crctk README

crctk: crctk.c
	gcc -Wall -O3 -DVERSION=\"$(version)\" -o $@ $< -lz

README: crctk
	./crctk -h > README

clean:
	-rm crctk README
