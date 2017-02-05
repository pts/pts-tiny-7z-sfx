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
* Memory usage is high, especially for solid archives. See below.
* It always extracts to the current directory.
* It does not support (and may misbehave for) encryption in archives.

Memory usage:

* It keeps an uncompressed version of each file in memory.
* It decompresses solid blocks (it can be whole .7z archive) to memory.
* You can limit the memory usage of decompression by specifying `7z -m...'
  flags when creating the .7z archive.
* The dictionary size (`7z -md=...') doesn't matter for memory usage.
* Only the solid block size (`7z -ms=...') matters. The default can be
  very high (up to 4 GB), so always specify something small (e.g.
  `7z -ms=50000000b') or turn off solid blocks (`7z -ms=off').
* The memory usage will be:

  total_memory_usage_for_tiny7zip_decompression <=
      static_memory_size +
      archive_header_size +
      listing_structures_size +
      max([solid_block_size] + uncompressed_file_sizes).
  static_memory_size == 100 000 bytes.
  archive_header_size == file_count * 32 bytes + sum(filename_sizes).
  filename_sizes counts each character as 2 bytes (because of UTF-16 encoding).
  listing_structures_size == file_count * 58 bytes + sum(filename_sizes).
  solid_block_size == value of `7z -ms=...', or 0 if `7z -ms=off'.
      Be careful, the default can be as large as 4 GB.
  uncompressed_file_sizes: List of uncompressed file sizes in the archive.
  file_count and file_size include both files and directories (folders).

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
