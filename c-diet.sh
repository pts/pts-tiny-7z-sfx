#! /bin/sh --
#
# Compiles with dietlibc.
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.hu/2013/12/how-to-make-smaller-c-and-c-binaries.html
#

set -ex
# The warning ``main: warning: the use of LEGACY `utimes' is discouraged,
# use `utime' '' is harmless.
diet -Os gcc -m32 -s -Os -DSTATIC=static -DUSE_LZMA2 -DUSE_CHMODW \
    -W -Wall -fno-stack-protector -fomit-frame-pointer \
    -Wl,--build-id=none -Wl,--hash-style=gnu -Wl,-z,norelro \
    -mpreferred-stack-boundary=2 -fno-unroll-loops -fmerge-all-constants \
    -falign-functions=1 -falign-jumps=1 -falign-loops=1 \
    -fno-unwind-tables -fno-asynchronous-unwind-tables \
    -ffunction-sections -fdata-sections -Wl,--gc-sections \
    -o tiny7zx.diet.unc "$@" \
    all.c
# This doesn't make a difference for --elftiny below.
./upxbc --elfstrip tiny7zx.diet.unc
./upxbc --upx=./upx.pts --elftiny -f -o tiny7zx.diet tiny7zx.diet.unc

: c-diet.sh OK.
