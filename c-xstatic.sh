#! /bin/sh --
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.hu/2013/12/how-to-make-smaller-c-and-c-binaries.html

set -ex
# The warning ``main: warning: the use of LEGACY `utimes' is discouraged,
# use `utime' '' is harmless.
xstatic gcc -D_FILE_OFFSET_BITS=64 -DSTATIC=static -DUSE_LZMA2 -DUSE_CHMODW \
    -m32 -s -Os \
    -W -Wall -Wextra -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int -Werror=pointer-sign -Werror=pointer-arith \
    -fno-stack-protector -fomit-frame-pointer -ffunction-sections -fdata-sections -Wl,--gc-sections \
    -o tiny7zx.xstatic.unc "$@" \
    all.c
# This stripping saves about 136 bytes.
strip -s --remove-section=.note.gnu.gold-version \
    --remove-section=.comment --remove-section=.note \
    --remove-section=.note.gnu.build-id \
    tiny7zx.xstatic.unc
./upxbc --upx=./upx.pts --elftiny -f -o tiny7zx.xstatic tiny7zx.xstatic.unc

: c-xstatic.sh OK.
