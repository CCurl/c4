ARCH ?= 64
CFLAGS = -O3 -m$(ARCH)
srcfiles := $(shell find . -name "*.c")
incfiles := $(shell find . -name "*.h")

c4: $(srcfiles) $(incfiles)
	$(CC) $(CFLAGS) $(srcfiles) -o $@

run: c4
	./c4

clean:
	rm -f c4

test: c4
	./c4 block-200.fth

bin: c4
	cp -u -p c4 ~/bin/