#! /bin/sh --

set -ex
gcc -s -O2 -D_FILE_OFFSET_BITS=64 "$@" -W -Wall -Wextra -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int -Werror=pointer-sign -Werror=pointer-arith -o tiny7zx.dynamic \
    7zAlloc.c 7zBuf.c 7zCrc.c 7zDec.c 7zIn.c \
    7zMain.c 7zStream.c Bcj2.c Bra.c Bra86.c Lzma2Dec.c LzmaDec.c

: c-dynamic.sh OK.
