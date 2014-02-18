SHELL=/bin/bash
.PHONY: all tag_randombin check_tag_randombin check_verify_randombin clean

all: check_tag_randombin check_verify_randombin check_verify_db_randombin

random.bin.crc: random.bin
	@echo "*** $@ ***"
	../crctk -c random.bin > random.bin.crc

tag_randombin: random.bin.crc
	@echo "*** $@ ***"
	../crctk -t random.bin

check_tag_randombin: tag_randombin
	@echo "*** $@ ***"
	[[ "$$(grep --color=never -o '[0-9a-fA-F]\{8\}' random.bin.crc)" = "$$(ls random*.bin|grep --color=never -o '[0-9a-fA-F]\{8\}')" ]]

check_verify_randombin: tag_randombin
	@echo "*** $@ ***"
	../crctk -v random*.bin

check_verify_db_randombin: random.bin
	../crctk -C crc.cdb random*.bin
	../crctk -V crc.cdb
	../crctk -V crc.cdb random*.bin

random.bin: clean
	@echo "*** $@ ***"
	dd if=/dev/urandom bs=512 count=2048 of=./random.bin

notsorandom.bin: check_verify_db_randombin
	@echo "*** $@ ***"
	dd if=/dev/urandom bs=512 count=2048 of=$@
	../crctk -aC crc.cdb $@
	../crctk -V crc.cdb
	../crctk -V crc.cdb $@

clean:
	@echo "*** $@ ***"
	-rm random*
	-rm notsorandom*
	-rm *.cdb
