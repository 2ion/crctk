crctk: crctk.c
	gcc -Wall -Os -DVERSION=\\"$(version)\\" -o $@ $< -lz
