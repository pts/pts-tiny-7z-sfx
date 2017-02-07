#! /bin/sh --

set -ex
${CC:-gcc} -s -O2 -D_FILE_OFFSET_BITS=64 -DUSE_LZMA2 -DUSE_CHMODW \
    -W -Wall -Wextra -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int -Werror=pointer-sign -Werror=pointer-arith \
    -o tiny7zx.dynamic "$@" \
    7zAlloc.c 7zCrc.c 7zDec.c 7zIn.c 7zMain.c 7zStream.c Bcj2.c Bra.c Bra86.c Lzma2Dec.c LzmaDec.c
ls -ld tiny7zx.dynamic

: c-dynamic.sh OK.
