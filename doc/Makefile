TEMPLATEDIR=/usr/share/pandoc/data/templates/default
TEMPLATE_MAN=$(TEMPLATEDIR).man
TEMPLATE_HTML=$(TEMPLATEDIR).html
.PHONY: all

all: crctk.3 crctk.html

crctk.3: crctk.mkd
	pandoc -t man --template $(TEMPLATE_MAN) -o $@ $<

crctk.html: crctk.mkd
	pandoc -t html --template $(TEMPLATE_HTML) -o $@ $<
