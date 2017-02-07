README for pts-tiny-7z-sfx
^^^^^^^^^^^^^^^^^^^^^^^^^^
pts-tiny-7z-sfx is a tiny 7-Zip (.7z archive) extractor and self-extractor
(SFX) written in standard C. It's Unix-only, actively tested on Linux.

Features:

* Small (the Linux statically linked binary is less than 40 kB).
* Can be used stand-alone to extract .7z archives.
* Can be used to create an SFX (self-extracting) executable by prepending it to
  a .7z archive. (Same as the `7z -sfx' flag.)
* It supports file and directory attributes (i.e. it calls chmod(2)).
* It sets the mtime (i.e. it calls utimes(2)) of extracted files.
* It can extract symlinks.
* It has a command-line syntax compatible with the regular console SFX binaries.
* It's implemented in standard C (and C++).
* Its command-line is compatible with `7z', `rar', `unrar' and `unzip'.
* It refuses to modify files with unsafe names (e.g. ../../../etc/passwd).

Limitations:

* It supports only these compressors: LZMA, LZMA2, BCJ, BCJ2, COPY.
* Memory usage can be high, especially for solid archives. See below.
* It always extracts to the current directory.
* It does not support (and may misbehave for) encryption in archives.
* Works on Unix only (tested on Linux). Doesn't work on Windows.
* Doesn't restore Unix file owners (UID, GID).
* Doesn't restore Unix file extended attributes.
* Doesn't restore Unix character devices, block devices, sockets or pipes.

Quick try on Linux i386 and amd64:

  wget -O hello.7z https://github.com/pts/pts-tiny-7z-sfx/releases/download/v9.22+pts5/hello.7z
  wget -O tiny7zx  https://github.com/pts/pts-tiny-7z-sfx/releases/download/v9.22+pts5/tiny7zx
  chmod +x tiny7zx
  ./tiny7zx t hello.7z
  cat tiny7zx hello.7z >hello.sfx.7z  # Create self-extracting archive.
  chmod +x hello.sfx.7z
  ./hello.sfx.7z  # The archive self-extracts on Linux i386 and amd64.
  hello/linkhi.sh  # Runs shell script pointed to by symlink.
  ./hello.sfx.7z  # Won't overwrite files by default.
  ./hello.sfx.7z -y  # Overwrites files.

For best results, please use the latest release version number (instead of
v9.22+pts5) in the links above.

To create a .7z archive compatible with pts-tiny-7z-sfx:

* Run: 7z a -t7z -mx=7 -ms=50m -ms=on foo.7z foo...
* Use `7z -ms=...' to limit the extraction memory usage, see also below.

To create a self-extracting archive (SFX):

* Run: 7z a -sfx.../tiny7zx ...
* Replace `.../' with the path to the tiny7zx executable created in the
  `Installation' section.
* If you already have a .7z archive, just concatenate:

    cat .../tiny7zx myarchive.7z >myarchive.sfx.7z
    chmod +x myarchive.sfx.7z
    ./myarchive.sfx.7zx --help
    ./myarchive.sfx.7zx  # Extracts its contents.

Installation
~~~~~~~~~~~~
If your target system is Linux i386 or amd64 (x86_64), you can download a
binary release from https://github.com/pts/pts-tiny-7z-sfx/releases ,
otherwise you need to compile from source.

To compile from source, run `make' and copy the resulting executable
`tiny7zx.dynamic' as `tiny7zx' to your $PATH.

Alternatively, you can run one of the ./c-*.sh scripts to compile from
source. To create the tiny Linux i386 binary, run `./c-minidiet.sh'.

Compilation options to add/remove features:

* -UUSE_CHMODW: Remove support for chmod() to make files
  and directories writable upon a ``Permission denied''.
  Saves about 160 bytes of tiny7zx (c-minidiet.sh).

* -UUSE_LZMA2: Remove LZMA2 decompression support.
  Saves about 784 bytes of tiny7zx (c-minidiet.sh).

Apply the compilation options like this:

* ./c-minidiet.sh -UUSE_LZMA2 -UUSE_CHMODW
* make clean; make CDEFS="-UUSE_LZMA2 -UUSE_CHMODW"

pts-tiny-7z-sfx compiles cleanly with any of:

* gcc
* gcc -ansi
* gcc -ansi -pedantic
* gcc -std=c89
* gcc -std=c90
* gcc -std=c99
* gcc -std=c11
* g++
* g++ -ansi
* g++ -ansi -pedantic
* g++ -std=c++98
* g++ -std=c++0x
* g++ -std=c++11

Supported systems:

* Linux i386 without libc (recommended): compile with ./c-minidiet.sh
  Recommeded compiler is gcc-4.8 or later.
* Linux with glibc: compile with ./c-dynamic.sh
* Linux with dietlibc: compile with ./c-diet.sh
* Linux i386 with xstatic uClibc: compile with ./c-xstatic.sh
* other Unix: ./c-dynamic.sh probably works, maybe needs minor porting
* Windows: not supported.

Memory usage
~~~~~~~~~~~~
Info about memory usage during extraction:

* It keeps an uncompressed version of each file in memory before writing the
  file.
* It decompresses each solid block (it can be the whole .7z archive) to memory.
* You can limit the memory usage of decompression by specifying `7z -m...'
  flags when creating the .7z archive.
* The dictionary size (`7z -md=...') doesn't matter for memory usage.
* Only the solid block size (`7z -ms=...') matters. The default can be
  very high (up to 4 GB), so always specify something small (e.g.
  `7z -ms=50000000b') or turn off solid blocks (`7z -ms=off').
* The memory usage of c-minidiet.c will be (most of the time):

  total_memory_usage_for_tiny7zx_decompression <=
      static_memory_size +
      archive_header_size +
      listing_structures_size +
      max([solid_block_size] + uncompressed_file_sizes).
  static_memory_size == 100 000 bytes.
  archive_header_size == file_count * 32 bytes + sum(filename_sizes).
  filename_sizes counts each character as 2 bytes (because of UTF-16 encoding).
  listing_structures_size == file_count * 56 bytes.
  solid_block_size == value of `7z -ms=...', or 0 if `7z -ms=off'.
      Be careful, the default can be as large as 4 GB.
  uncompressed_file_sizes: List of uncompressed file sizes in the archive.
  file_count and file_size include both files and directories (folders).

License
~~~~~~~
pts-tiny-7z-sfx is released under the GNU GPL v2.

Related software
~~~~~~~~~~~~~~~~
See http://sourceforge.net/p/sevenzip/discussion/45797/thread/233f5efd
about 7zS2con.sfx, a similar software for Win32.

Forked from 7z922.tar.bz2 from
http://sourceforge.net/projects/sevenzip/files/7-Zip/9.22/7z922.tar.bz2/download

__END__
