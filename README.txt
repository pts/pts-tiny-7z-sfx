README for pts-mini-7z-sfx
^^^^^^^^^^^^^^^^^^^^^^^^^^
pts-mini-7z-sfx is a mini 7-Zip extractor and self-extractor (sfx) written
in ANSI C. It's Unix-only.

Features:

* Small (the Linux statically linked binary is less than 40 kB).
* Can be used to create a SFX (self-extract) binary by prepending to a .7z
  archive. (Same as the `7z -sfx' flag.)
* It supports file and directory attributes (i.e. it calls chmod(2)).
* It sets the mtime (i.e. it calls utimes(2)).

Limitations:

* It supports only: LZMA, LZMA2, BCJ, BCJ2, COPY.
* It keeps an uncompressed version of each file to RAM.
* It decompresses solid 7z blocks (it can be whole 7z archive) to RAM.
  So user that calls SFX installer must have free RAM of size of largest
  solid 7z block (size of 7z archive at simplest case).
* It overwrites files without asking.
* It always extracts to the current directory.
* It does not support (and may misbehave for) encryption in archives.

See http://sourceforge.net/p/sevenzip/discussion/45797/thread/233f5efd
about 7zS2con.sfx, a similar software for Win32.

Forked from 7z922.tar.bz2 from
http://sourceforge.net/projects/sevenzip/files/7-Zip/9.22/7z922.tar.bz2/download

TODOs:

* TODO(pts): Add symlink support.
* TODO(pts): Make it smaller with fastcall.

__EOF__
