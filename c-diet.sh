#! /bin/sh --
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.hu/2013/12/how-to-make-smaller-c-and-c-binaries.html

set -ex
# The warning ``main: warning: the use of LEGACY `utimes' is discouraged,
# use `utime' '' is harmless.
diet -Os gcc -DSTATIC=static -m32 -s -Os -W -Wall -o tiny7zx \
    -fno-stack-protector -fomit-frame-pointer \
    -Wl,--build-id=none -Wl,--hash-style=gnu -Wl,-z,norelro \
    -mpreferred-stack-boundary=2 -fno-unroll-loops -fmerge-all-constants \
    -falign-functions=1 -falign-jumps=1 -falign-loops=1 \
    -fno-unwind-tables -fno-asynchronous-unwind-tables \
    -ffunction-sections -fdata-sections -Wl,--gc-sections \
    all.c
#    7zAlloc.c 7zBuf.c 7zBuf2.c 7zCrc.c 7zCrcOpt.c 7zDec.c 7zFile.c 7zIn.c \
#    7zMain.c 7zStream.c Bcj2.c Bra.c Bra86.c CpuArch.c Lzma2Dec.c LzmaDec.c \
#    Ppmd7.c Ppmd7Dec.c
# This stripping saves about 136 bytes.
strip -s --strip-unneeded --remove-section=.note.gnu.gold-version \
    --remove-section=.comment --remove-section=.note \
    --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag \
    --remove-section=.eh_frame --remove-section=.eh_frame_ptr \
    --remove-section=.jcr --remove-section=.got.plt \
    tiny7zx
./sstrip tiny7zx
upx.pts --ultra-brute tiny7zx

: c-xstatic.sh OK.
