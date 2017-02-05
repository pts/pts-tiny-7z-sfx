#! /bin/sh --
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.hu/2013/12/how-to-make-smaller-c-and-c-binaries.html

set -ex
# The warning ``main: warning: the use of LEGACY `utimes' is discouraged,
# use `utime' '' is harmless.
xstatic gcc -D_FILE_OFFSET_BITS=64 -DSTATIC=static "$@" -m32 -s -Os -W -Wall -Wextra -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int -Werror=pointer-sign -Werror=pointer-arith \
    -o tiny7zx.xstatic.unc \
    -fno-stack-protector -fomit-frame-pointer \
    -ffunction-sections -fdata-sections -Wl,--gc-sections \
    all.c
#    7zAlloc.c 7zBuf.c 7zBuf2.c 7zCrc.c 7zDec.c 7zIn.c \
#    7zMain.c 7zStream.c Bcj2.c Bra.c Bra86.c Lzma2Dec.c LzmaDec.c \
#    Ppmd7.c Ppmd7Dec.c
# This stripping saves about 136 bytes.
strip -s --remove-section=.note.gnu.gold-version \
    --remove-section=.comment --remove-section=.note \
    --remove-section=.note.gnu.build-id \
    tiny7zx.xstatic.unc
cp -a tiny7zx.xstatic.unc tiny7zx.xstatic
upx.pts --ultra-brute tiny7zx.xstatic

: c-xstatic.sh OK.
