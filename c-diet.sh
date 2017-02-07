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
# This stripping saves about 136 bytes.
strip -s --strip-unneeded --remove-section=.note.gnu.gold-version \
    --remove-section=.comment --remove-section=.note \
    --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag \
    --remove-section=.eh_frame --remove-section=.eh_frame_ptr \
    --remove-section=.jcr --remove-section=.got.plt \
    tiny7zx.diet.unc
./sstrip tiny7zx.diet.unc
cp -a tiny7zx.diet.unc tiny7zx.diet
upx.pts --ultra-brute tiny7zx.diet

: c-diet.sh OK.
