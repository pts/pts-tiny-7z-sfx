#!/bin/sh --
#
# Compiles and links against an embedded dietlibc.
# by pts@fazekas.hu at Wed Oct 22 00:09:13 CEST 2014
#
# See the explanation why these flags are useful for small output here:
# http://ptspts.blogspot.hu/2013/12/how-to-make-smaller-c-and-c-binaries.html
#
# TODO(pts): Disable WANT_STACKGAP for dietlibc.
# TODO(pts): -D_FILE_OFFSET_BITS=64, fopen64 etc.
# TODO(pts): Use fread_unlocked, *_unlocked etc.
#

CFLAGS='-DUSE_MINIINC1 -ansi -pedantic -nostdinc -m32 -s -U_FORTIFY_SOURCE -fno-stack-protector -fno-ident -fomit-frame-pointer -mpreferred-stack-boundary=2 -falign-functions=1 -falign-jumps=1 -falign-loops=1 -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-unroll-loops -fmerge-all-constants -Os -W -Wall -Wextra -Wsystem-headers -Werror=implicit -Werror=implicit-int -Werror=implicit-function-declaration --sysroot minidiet -isystem minidiet -static-libgcc -DUSE_MINIINC2'
# Not needed: ld -z norelro --build-id=none
LDFLAGS1='-nostdlib -m elf_i386 -static -s minidiet/start.o'
LDFLAGS2='minidiet/lib1.a minidiet/libgcc1.a -T minidiet/xtiny.scr'

# strip -g --remove-section=.eh_frame --remove-section=.comment *.o
# perl -ne 'print"$1\n" if m@^(?:free|__errno_location|mremap|__unified_syscall|errno|mmap|munmap|exit|stackgap|__libc_initary|strstr|getenv|__guard|memcmp|environ|close|read|open|close|set_thread_area|fputs|stdout|utimes|gettimeofday|fchmod|fileno|fflush|fwrite_unlocked|lstat|symlink|unlink|strcmp|ferror|ftell|fseek|fwrite|fputc_unlocked|isatty|__libc_write|lseek|__stdin_is_tty|__stack_chk_fail|ioctl|fread|fclose|fopen|chmod|mkdir|umask|__write2|fgetc_unlocked|__stdio_init_file|__stdio_parse_mode|__stdio_init_file_nothreads|fstat|feof_unlocked) (\S+)$@ ' smap | sort | uniq | tee olist && ar cr ../minidiet/lib1.a `cat olist`

set -ex

test -f minidiet/lib1.a
test -f minidiet/libgcc1.a
test -f minidiet/start.o
test -f minidiet/miniinc1.h
test -f minidiet/xtiny.scr

gcc $CFLAGS -c all.c
ld -o tiny7zx $LDFLAGS1 all.o $LDFLAGS2
cp -a tiny7zx tiny7zx.unc
./upx.pts -q -q --ultra-brute tiny7zx
ls -ld tiny7zx tiny7zx.unc

: c-minidiet.sh OK.
