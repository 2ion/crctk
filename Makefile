version ?= 0.1-$(shell git rev-parse --short HEAD)

crctk: crctk.c
	gcc -Wall -Os -DVERSION=\"$(version)\" -o $@ $< -lz
