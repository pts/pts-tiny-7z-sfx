# by pts@fazekas.hu at Tue Feb  7 15:44:55 CET 2017

CC = gcc
CFLAGS = -s -O2 -D_FILE_OFFSET_BITS=64 -DUSE_LZMA2 -DUSE_CHMODW \
    -W -Wall -Wextra -Werror=implicit -Werror=implicit-function-declaration \
    -Werror=implicit-int -Werror=pointer-sign -Werror=pointer-arith
CDEFS =  # Example: make CDEFS=-UUSE_LZMA2
SOURCES = 7zAlloc.c 7zCrc.c 7zDec.c 7zIn.c 7zMain.c 7zStream.c Bcj2.c \
    Bra.c Bra86.c Lzma2Dec.c LzmaDec.c

.PHONY: all clean

all: tiny7zx.dynamic

clean:
	rm -f core *.o tiny7zx.dynamic

tiny7zx.dynamic: $(SOURCES)
	$(CC) $(CFLAGS) -o tiny7zx.dynamic $(CDEFS) $(SOURCES)
	ls -ld tiny7zx.dynamic
