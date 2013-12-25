#! /bin/sh --

gcc -s -O2 -W -Wall -o tiny7zx.dynamic \
    7zAlloc.c 7zBuf.c 7zBuf2.c 7zCrc.c 7zCrcOpt.c 7zDec.c 7zFile.c 7zIn.c \
    7zMain.c 7zStream.c Bcj2.c Bra.c Bra86.c CpuArch.c Lzma2Dec.c LzmaDec.c \
    Ppmd7.c Ppmd7Dec.c

: c-dynamic.sh OK.
