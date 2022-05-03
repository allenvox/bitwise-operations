CFLAGS = gcc -Wall -I src -I varint -MMD
DIRGUARD = @mkdir -p $(@D)

all: bin/coder
-include obj/*.d

varint: bin/varint

uncompress: bin/uncompress

bin/coder: obj/main.o obj/coder.o obj/command.o -lm
	$(DIRGUARD)
	$(CFLAGS) -o $@ $^
	
obj/main.o: src/main.c
	$(DIRGUARD)
	$(CFLAGS) -c -o $@ $<

obj/coder.o: src/coder.c
	$(CFLAGS) -c -o $@ $<

obj/command.o: src/command.c
	$(CFLAGS) -c -o $@ $<

obj/varint.o: varint/varint.c
	$(DIRGUARD)
	$(CFLAGS) -c -o $@ $<

clean:
	rm -rf bin/ obj/ result.txt *.bin

bin/uncompress: varint/uncompress.c
	$(DIRGUARD)
	gcc -Wall -o $@ $^
	echo > bin/uncompressed.dat
	bin/uncompress > bin/uncompressed.dat
	rm bin/uncompress

bin/varint: obj/varint.o
	make uncompress
	$(CFLAGS) -o $@ $^