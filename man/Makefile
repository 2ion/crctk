TEMPLATE=/usr/share/pandoc/data/templates/default.man
crctk.3: crctk.mkd
	pandoc -t man --template $(TEMPLATE)  -o $@ $<
