#!/bin/sh --
#
# Compiles with OpenWatcom C compiler and links against minilibc686
# by pts@fazekas.hu at Tue May 30 19:12:19 CEST 2023
#
# See: https://github.com/pts/minilibc686
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.com/2013/12/how-to-make-smaller-c-and-c-binaries.html
#

set -ex

minicc --watcom -DUSE_MINIALLOC_SYS_BRK -DUSE_LZMA2 -DUSE_CHMODW -DUSE_STAT64 -Wl,-N -march=i386 -fno-unroll-loops -fmerge-all-constants -fno-math-errno -o tiny7zx.mlw all.c
ls -ld tiny7zx.mlw

: c-minilibc686-openwatcom.sh OK.
