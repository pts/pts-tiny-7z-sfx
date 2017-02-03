#!/bin/sh --
#
# Compiles and links against an embedded libc.
# by pts@fazekas.hu at Wed Oct 22 00:09:13 CEST 2014
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.com/2013/12/how-to-make-smaller-c-and-c-binaries.html
#

CFLAGS='-DUSE_MINIINC1 -DUSE_MINIALLOC -ansi -pedantic -nostdinc -m32 -s -Os -U_FORTIFY_SOURCE -fno-stack-protector -fno-ident -fomit-frame-pointer -mpreferred-stack-boundary=2 -falign-functions=1 -falign-jumps=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-unroll-loops -fmerge-all-constants -fno-math-errno -W -Wall -Wextra -Wsystem-headers -Werror=implicit -Werror=implicit-int -Werror=implicit-function-declaration --sysroot minidiet -isystem minidiet -static-libgcc -DUSE_MINIINC2'
# Not needed: ld -z norelro --build-id=none
LDFLAGS1='-nostdlib -m elf_i386 -static -s'
LDFLAGS2='-T minidiet/minidiet.scr'

set -ex

test -f minidiet/miniinc1.h
test -f minidiet/minidiet.scr

gcc $CFLAGS -c all.c minidiet/minidiet.c
ld -o tiny7zx $LDFLAGS1 all.o minidiet.o $LDFLAGS2
cp -a tiny7zx tiny7zx.unc
./upx.pts -q -q --ultra-brute tiny7zx
ls -ld tiny7zx tiny7zx.unc

: c-minidiet.sh OK.
