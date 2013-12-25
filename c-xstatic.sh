#! /bin/sh --

set -ex
# The warning ``main: warning: the use of LEGACY `utimes' is discouraged,
# use `utime' '' is harmless.
xstatic gcc -m32 -s -Os -W -Wall -o tiny7zx \
    -fno-stack-protector -fomit-frame-pointer \
    -ffunction-sections -fdata-sections -Wl,--gc-sections \
    7zAlloc.c 7zBuf.c 7zBuf2.c 7zCrc.c 7zCrcOpt.c 7zDec.c 7zFile.c 7zIn.c \
    7zMain.c 7zStream.c Bcj2.c Bra.c Bra86.c CpuArch.c Lzma2Dec.c LzmaDec.c \
    Ppmd7.c Ppmd7Dec.c
upx.pts --ultra-brute tiny7zx

: c-xstatic.sh OK.
