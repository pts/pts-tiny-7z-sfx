#!/bin/sh --
#
# Compiles and links against an embedded libc.
# by pts@fazekas.hu at Wed Oct 22 00:09:13 CEST 2014
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.com/2013/12/how-to-make-smaller-c-and-c-binaries.html
#
# TODO(pts): Why does it generate larger output than without the linker
# script (-Wl,-T,minidiet/minidiet.scr)? It's not only the 32 bytes of
# PT_GNU_STACK.
#
# GCC 7.5.0 generates shorter uncompressed output, but GCC 4.8.4 generates
# shorter compressed output.
#

set -ex

test -f minidiet/miniinc1.h
test -f minidiet/minidiet.scr

nasm -O999 -f elf -o minidiet/minidiet32.o minidiet/minidiet32.nasm

# This will add the following unwanted flags to ld: --build-id -z relro -L/usr/lib/...
# The linker script (-T) makes `--build-id' and `-z relro' irrelevant.
# The lack of -l... flags makes `-L...' irrelevant.
# Not needed (because we use a linker script): -Wl,-z,norelro,--build-id=none
gcc -m32 -s -Os -static -march=i386 -fno-pic \
    -DUSE_MINIINC1 -DUSE_MINIALLOC_SYS_BRK -DUSE_LZMA2 -DUSE_CHMODW \
    -ansi -pedantic -W -Wall -Wextra -Wsystem-headers -Werror=implicit -Werror=implicit-int -Werror=implicit-function-declaration \
    -fno-builtin-memcpy -fno-builtin-malloc -fno-stack-protector -fno-ident -fomit-frame-pointer -mpreferred-stack-boundary=2 -falign-functions=1 -falign-jumps=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-unroll-loops -fmerge-all-constants -fno-math-errno \
    --sysroot minidiet -isystem minidiet -nostdlib -nostartfiles -nodefaultlibs -nostdinc -Wl,-T,minidiet/minidiet.scr \
    -o tiny7zx.unc "$@" \
    all.c minidiet/minidiet32.o
rm -f minidiet/minidiet32.o
./upxbc --upx=./upx.pts --elftiny -f -o tiny7zx tiny7zx.unc
ls -ld tiny7zx tiny7zx.unc

: c-minidiet.sh OK.
