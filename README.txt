README for pts-mini-7z-sfx
^^^^^^^^^^^^^^^^^^^^^^^^^^
pts-mini-7z-sfx is a mini 7-Zip extractor and self-extractor (SFX) written
in ANSI C. It's Unix-only, actively tested on Linux.

Features:

* Small (the Linux statically linked binary is less than 40 kB).
* Can be used to create a SFX (self-extract) binary by prepending to a 7z
  archive. (Same as the `7z -sfx' flag.)
* It supports file and directory attributes (i.e. it calls chmod(2)).
* It sets the mtime (i.e. it calls utimes(2)).
* It can extract symlinks.
* Has a command-line syntax compatible with the regular console SFX binaries.

Limitations:

* It supports only: LZMA, LZMA2, BCJ, BCJ2, COPY.
* It keeps an uncompressed version of each file in RAM.
* It decompresses solid 7z blocks (it can be whole 7z archive) to RAM.
  So user that calls SFX installer must have free RAM of size of largest
  solid 7z block (size of 7z archive at simplest case).
* It always extracts to the current directory.
* It does not support (and may misbehave for) encryption in archives.

Supported systems:

* Linux i386 without libc (recommended): compile with ./c-minidiet.sh
* Linux with glibc: compile with ./c-dynamic.sh
* Linux with dietlibc: compile with ./c-diet.sh
* Linux i386 with xstatic uClibc: compile with ./c-xstatic.sh
* other Unix: ./c-dynamic.sh probably works, maybe needs minor porting
* Windows: not supported.

See http://sourceforge.net/p/sevenzip/discussion/45797/thread/233f5efd
about 7zS2con.sfx, a similar software for Win32.

Forked from 7z922.tar.bz2 from
http://sourceforge.net/projects/sevenzip/files/7-Zip/9.22/7z922.tar.bz2/download

__EOF__
