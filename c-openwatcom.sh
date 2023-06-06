#!/bin/sh --
#
# Compiles with OpenWatcom C compiler, links against OpenWatcom libc
# by pts@fazekas.hu at Tue May 30 19:12:19 CEST 2023
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.com/2013/12/how-to-make-smaller-c-and-c-binaries.html
#

set -ex

owcc -blinux -I"$WATCOM/lh" -DUSE_LZMA2 -DUSE_CHMODW -DUSE_STAT64 -s -O2 -march=i386 -std=c99 -W -Wall -Wextra -Werror -o tiny7zx.ow all.c && strip tiny7zx.ow
ls -ld tiny7zx.ow

: c-openwatcom.sh OK.
