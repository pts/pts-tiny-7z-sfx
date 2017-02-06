/* 7zMain.c - Test application for 7z Decoder2010-10-28 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7z.h"
#include "7zAlloc.h"
#include "7zCrc.h"
#include "7zVersion.h"
#include "CpuArch.h"

static Byte kUtf8Limits[5] = { 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

/* TODO(pts): Check for overflow in destPos? */
static Bool Utf16Le_To_Utf8(Byte *dest, size_t *destLen, const Byte *srcUtf16Le, size_t srcUtf16LeLen)
{
  size_t destPos = 0;
  const Byte *srcUtf16LeEnd = srcUtf16Le + srcUtf16LeLen * 2;
  for (;;)
  {
    unsigned numAdds;
    UInt32 value;
    if (srcUtf16Le == srcUtf16LeEnd)
    {
      *destLen = destPos;
      return True;
    }
    value = GetUi16(srcUtf16Le);
    srcUtf16Le += 2;
    if (value < 0x80)
    {
      if (dest)
        dest[destPos] = (char)value;
      destPos++;
      continue;
    }
    if (value >= 0xD800 && value < 0xE000)
    {
      UInt32 c2;
      if (value >= 0xDC00 || srcUtf16Le == srcUtf16LeEnd)
        break;
      c2 = GetUi16(srcUtf16Le);
      srcUtf16Le += 2;
      if (c2 < 0xDC00 || c2 >= 0xE000)
        break;
      value = (((value - 0xD800) << 10) | (c2 - 0xDC00)) + 0x10000;
    }
    for (numAdds = 1; numAdds < 5; numAdds++)
      if (value < (((UInt32)1) << (numAdds * 5 + 6)))
        break;
    if (dest)
      dest[destPos] = (char)(kUtf8Limits[numAdds - 1] + (value >> (6 * numAdds)));
    destPos++;
    do
    {
      numAdds--;
      if (dest)
        dest[destPos] = (char)(0x80 + ((value >> (6 * numAdds)) & 0x3F));
      destPos++;
    }
    while (numAdds != 0);
  }
  *destLen = destPos;
  return False;
}

static unsigned GetUnixMode(unsigned *umaskv, UInt32 attrib) {
  unsigned mode;
  if (*umaskv + 1U == 0U) {
    unsigned default_umask = 022;
    *umaskv = umask(default_umask);
    if (*umaskv != default_umask) umask(*umaskv);
  }
  if (attrib & FILE_ATTRIBUTE_UNIX_EXTENSION) {
    mode = attrib >> 16;
  } else {
    mode = (attrib & FILE_ATTRIBUTE_READONLY ? 0444 : 0666) |
        (attrib & FILE_ATTRIBUTE_DIRECTORY ? 0111 : 0);
  }
  return mode & ~*umaskv & 07777;
}

static char stdout_buf[4096];
static unsigned stdout_bufc = 0;

/* Writes NUL-terminated string sz to stdout, does line buffering. */
static void WriteMessage(const char *sz) {
  char had_newline = False, c;
  while ((c = *sz++) != '\0') {
    if (stdout_bufc == sizeof stdout_buf) {
      (void)!write(1, stdout_buf, stdout_bufc);
      stdout_bufc = 0;
    }
    if (c == '\n') had_newline = True;
    stdout_buf[stdout_bufc++] = c;
  }
  if (had_newline) {
    (void)!write(1, stdout_buf, stdout_bufc);
    stdout_bufc = 0;
  }
}

STATIC void PrintError(char *sz)
{
  WriteMessage("\nERROR: ");
  WriteMessage(sz);
  WriteMessage("\n");
}

static SRes MyCreateDir(const char *filename, unsigned *umaskv, Bool attribDefined, UInt32 attrib) {
  const unsigned mode = GetUnixMode(umaskv, attribDefined ? attrib :
      FILE_ATTRIBUTE_UNIX_EXTENSION | 0755 << 16);
  if (mkdir(filename, mode) != 0) {
    if (errno != EEXIST) return SZ_ERROR_WRITE_MKDIR;
  }
  if (attribDefined) {
    if (0 != chmod(filename, GetUnixMode(umaskv, attrib))) return SZ_ERROR_WRITE_MKDIR_CHMOD;
  }
  return SZ_OK;
}

/* Returns *a % b, and sets *a = *a_old / b; */
static UInt32 UInt64DivAndGetMod(UInt64 *a, UInt32 b) {
#ifdef __i386__  /* u64 / u32 division with little i386 machine code. */
  /* http://stackoverflow.com/a/41982320/97248 */
  UInt32 upper = ((UInt32*)a)[1], r;
  ((UInt32*)a)[1] = 0;
  if (upper >= b) {
    ((UInt32*)a)[1] = upper / b;
    upper %= b;
  }
  __asm__("divl %2" : "=a" (((UInt32*)a)[0]), "=d" (r) :
      "rm" (b), "0" (((UInt32*)a)[0]), "1" (upper));
  return r;
#else
  const UInt64 q = *a / b;  /* Calls __udivdi3. */
  const UInt32 r = *a - b * q;  /* `r = *a % b' would use __umoddi3. */
  *a = q;
  return r;
#endif
}

static void GetTimeSecAndUsec(
    const CNtfsFileTime *mtime, UInt64 *sec_out, UInt32 *usec_out) {
  /* mtime is 10 * number of microseconds since 1601 (+ 89 days). */
  *sec_out = (UInt64)(UInt32)mtime->High << 32 | (UInt32)mtime->Low;
  *usec_out = UInt64DivAndGetMod(sec_out, 10000000) / 10;
}

/* tv[0] is assumed to be prefilled with the desired st_atime */
static WRes SetMTime(const char *filename,
                     const CNtfsFileTime *mtime,
                     struct timeval tv[2]) {
  UInt64 sec;
  if (mtime) {
    if (sizeof(tv[1].tv_usec) == 4) {  /* i386 Linux */
      GetTimeSecAndUsec(mtime, &sec, (UInt32*)&tv[1].tv_usec);
    } else {
      UInt32 usec;
      GetTimeSecAndUsec(mtime, &sec, &usec);
      tv[1].tv_usec = usec;
    }
    sec -= (369 * 365 + 89) * (UInt64)(24 * 60 * 60);
    /* If (signed) tv_sec would underflow or overflow */
    if (sizeof(tv[1].tv_sec) == 4 && (UInt32)(sec >> 32) + 1U > 1U) return -1;
    tv[1].tv_sec = sec;
  } else {
    tv[1] = tv[0];
  }
  return utimes(filename, tv) != 0;
}

#ifdef USE_CHMODW
static Bool MakeFileWritable(const char *filename) {
  struct stat st;
#ifdef _SZ_CHMODW_DEBUG
  fprintf(stderr, "CHMODW MFW %s\n", filename);
#endif
  return 0 == lstat(filename, &st) && S_ISREG(st.st_mode) &&
      (st.st_mode & 0200) == 0 &&
      chmod(filename, (st.st_mode & 07777) | 0200) != 0 &&
      errno == EACCES;  /* Permission denied. */
}

static void MakeDirWritable(const char *filename) {
  struct stat st;
#ifdef _SZ_CHMODW_DEBUG
  fprintf(stderr, "CHMODW MDW %s\n", filename);
#endif
  if (0 == lstat(filename, &st) && S_ISDIR(st.st_mode) &&
      (st.st_mode & 0200) == 0) {
    chmod(filename, (st.st_mode & 07777) | 0200);
  }
}

static void MakeDirsWritable(char *filename) {
  char *p = filename;
  /* MakeDirWritable("."); */ /* Don't do this, for security reasons. */
  for (;;) {
    while (*p && *p != '/') ++p;
    if (*p == '\0') break;
    *p = '\0';
    MakeDirWritable(filename);
    *p++ = '/';
  }
}
#endif

static SRes OutFile_Open(int *p, char *filename, Bool doYes) {
  mode_t mode = O_WRONLY | O_CREAT | O_TRUNC;
#ifdef USE_CHMODW
  Bool had_again = False;
#endif
  if (!doYes) mode |= O_EXCL;
#ifdef USE_CHMODW
 again:
#endif
  *p = open(filename, mode, 0644);
  if (*p >= 0) return SZ_OK;
  if (errno == EEXIST) return SZ_ERROR_OVERWRITE;
#ifdef USE_CHMODW
  if (errno == EACCES && !had_again) {  /* Permission denied. */
    if (MakeFileWritable(filename)) {  /* Permission denied. */
      MakeDirsWritable(filename);
      MakeFileWritable(filename);
    }
    had_again = True;
    goto again;
  }
#endif
  return SZ_ERROR_WRITE_OPEN;
}

static void UInt64ToStr(UInt64 value, char *s, int numDigits, char pad)
{
  char temp[32];
  int pos = 0;
  do
    temp[pos++] = (char)('0' + UInt64DivAndGetMod(&value, 10));
  while (value != 0);
  for (numDigits -= pos; numDigits > 0; numDigits--)
    *s++ = pad;  /* ' ' or '0'; */
  do
    *s++ = temp[--pos];
  while (pos);
  *s = '\0';
}

static char *UIntToStr(char *s, unsigned value, int numDigits)
{
  char temp[16];
  int pos = 0;
  do
    temp[pos++] = (char)('0' + (value % 10));
  while (value /= 10);
  for (numDigits -= pos; numDigits > 0; numDigits--)
    *s++ = '0';
  do
    *s++ = temp[--pos];
  while (pos);
  *s = '\0';
  return s;
}

#define PERIOD_4 (4 * 365 + 1)
#define PERIOD_100 (PERIOD_4 * 25 - 1)
#define PERIOD_400 (PERIOD_100 * 4 + 1)

static void ConvertFileTimeToString(const CNtfsFileTime *ft, char *s)
{
  UInt32 usec;
  UInt64 sec;
  unsigned year, mon, day, hour, min, ssec;
  Byte ms[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  unsigned t;
  UInt32 v;
  GetTimeSecAndUsec(ft, &sec, &usec);
  ssec = UInt64DivAndGetMod(&sec, 60);
  min = UInt64DivAndGetMod(&sec, 60);
  hour = UInt64DivAndGetMod(&sec, 24);
  v = (UInt32)sec;  /* Days. */

  year = (unsigned)(1601 + v / PERIOD_400 * 400);
  v %= PERIOD_400;

  t = v / PERIOD_100; if (t ==  4) t =  3; year += t * 100; v -= t * PERIOD_100;
  t = v / PERIOD_4;   if (t == 25) t = 24; year += t * 4;   v -= t * PERIOD_4;
  t = v / 365;        if (t ==  4) t =  3; year += t;       v -= t * 365;

  if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
    ms[1] = 29;
  for (mon = 1; mon <= 12; mon++)
  {
    unsigned s = ms[mon - 1];
    if (v < s)
      break;
    v -= s;
  }
  day = (unsigned)v + 1;
  s = UIntToStr(s, year, 4); *s++ = '-';
  s = UIntToStr(s, mon, 2);  *s++ = '-';
  s = UIntToStr(s, day, 2);  *s++ = ' ';
  s = UIntToStr(s, hour, 2); *s++ = ':';
  s = UIntToStr(s, min, 2);  *s++ = ':';
  s = UIntToStr(s, ssec, 2);
  *s = '\0';
}

#ifdef MY_CPU_LE_UNALIGN
/* We do the comparison in a quirky way so we won't depend on strcmp. */
#define IS_HELP(p) ((*(const UInt16*)(p) == ('-' | '-' << 8)) && \
    *(const UInt32*)((const UInt16*)(p) + 1) == ('h' | 'e' << 8 | 'l' << 16 | 'p' << 24) && \
    ((const Byte*)(p))[6] == 0)
#define STRCMP1(p, c) (*(const UInt16*)(p) == (UInt16)(Byte)(c))
#else
#define IS_HELP(p) (0 == strcmp((p), "--help"))
#define STRCMP1(p, c) (((const Byte*)(p))[0] == (c) && ((const Byte*)(p))[1] == 0)
#endif

int MY_CDECL main(int numargs, char *args[])
{
  CLookToRead lookStream;
  CSzArEx db;
  SRes res;
  Byte *filename_utf8 = NULL;
#ifndef USE_MINIALLOC
  size_t filename_utf8_capacity = 0;
#endif
  unsigned umaskv = -1;
  const char *archive = args[0];
  Bool listCommand = 0, testCommand = 0, doYes = 0;
  int argi = 2;
  const char *args1 = numargs >= 2 ? args[1] : "";

  WriteMessage("Tiny 7z extractor " MY_VERSION "\n\n");
  if ((args1[0] == '-' && args1[1] == 'h' && args1[2] == '\0') ||
      IS_HELP(args1)) {
    WriteMessage("Usage: ");
    WriteMessage(args[0]);
    WriteMessage(" <command> [<switches>...]\n\n"
      "<Commands>\n"
      "  l: List contents of archive\n"
      "  t: Test integrity of archive\n"
      "  x: eXtract files with full pathname (default)\n"
      "<Switches>\n"
      "  -e{Archive}: archive to Extract (default is self, argv[0])\n"
      "  -y: assume Yes on all queries\n");
     return 0;
  }
  if (numargs >= 2) {
    if (args1[0] == '-') {
      argi = 1;  /* Interpret args1 as a switch. */
    } else if (STRCMP1(args1, 'l')) {
      listCommand = 1;
    } else if (STRCMP1(args1, 't')) {
      testCommand = 1;
    } else if (STRCMP1(args1, 'x')) {
      /* extractCommand = 1; */
    } else {
      PrintError("unknown command");
      return 1;
    }
  }
  for (; argi < numargs; ++argi) {
    const char *arg = args[argi];
    if (arg[0] != '-') { incorrect_switch:
      PrintError("incorrect switch");
      return 1;
    }
   same_arg:
    if (arg[1] == 'e') {
      archive = arg + 2;
    } else if (arg[1] == 'y') {
      doYes = 1;
      if (arg[2] != '\0') {
        ++arg;
        goto same_arg;
      }
    } else {
      goto incorrect_switch;
    }
  }

  WriteMessage("Processing archive: ");
  WriteMessage(archive);
  WriteMessage("\n");
  WriteMessage("\n");
  if ((lookStream.fd = open(archive, O_RDONLY, 0)) < 0) {
    PrintError("can not open input archive");
    return 1;
  }

  LOOKTOREAD_INIT(&lookStream);

  /*CrcGenerateTable();*/

  SzArEx_Init(&db);
  res = SzArEx_Open(&db, &lookStream);
  if (res == SZ_OK) {
    UInt32 fileIndex;
    struct timeval tv[2];

    /*
    if you need cache, use these 3 variables.
    if you use external function, you can make these variable as static.
    */
    UInt32 blockIndex = (UInt32)-1; /* it can have any value before first call (if outBuffer = 0) */
    Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
    size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */

    /* Get desired st_time for extracted files. */
    gettimeofday(&tv[0], NULL);

    for (fileIndex = 0; fileIndex < db.db.NumFiles; fileIndex++) {
      size_t offset = 0;
      size_t outSizeProcessed = 0;
      const CSzFileItem *f = db.db.Files + fileIndex;
      const size_t filename_offset = db.FileNameOffsets[fileIndex];
      const size_t filename_utf16le_len =  db.FileNameOffsets[fileIndex + 1] - filename_offset;
      const Byte *filename_utf16le = db.FileNamesInHeaderBufPtr + filename_offset * 2;
      /* 2 for UTF-18 + 3 for UTF-8. 1 UTF-16 entry point can create at most 3 UTF-8 bytes (averaging for surrogates). */
      size_t filename_utf8_len = filename_utf16le_len * 3;
      SRes extract_res = SZ_OK;

      if (!listCommand && !f->IsDir) {
        if (blockIndex != db.FileIndexToFolderIndexMap[fileIndex]) {
          /* Memory usage optimization for USE_MINIALLOC.
           *
           * Without this, all solid blocks would be kept in memory,
           * potentially using gigabytes.
           */
          SzFree(filename_utf8);
          filename_utf8 = NULL;
#ifndef USE_MINIALLOC
          filename_utf8_capacity = 0;
#endif
        }
        /* It's important to do this first, before we allocate memory for
         * filename_utf8 for filename processing. Otherwise, with USE_MINIALLOC,
         * solid blocks would accumulate in memory.
         */
        extract_res = SzArEx_Extract(&db, &lookStream, fileIndex,
            &blockIndex, &outBuffer, &outBufferSize,
            &offset, &outSizeProcessed);
      }
#ifdef USE_MINIALLOC  /* Do an SzAlloc for each filename. It's cheap with USE_MINIALLOC. */
      SzFree(filename_utf8);
      if ((filename_utf8 = (Byte*)SzAlloc(filename_utf8_len)) == 0) {
        res = SZ_ERROR_MEM;
        break;
      }
#else
      if (filename_utf8_len > filename_utf8_capacity) {
        SzFree(filename_utf8);
        if (filename_utf8_capacity == 0) filename_utf8_capacity = 128;
        while (filename_utf8_capacity < filename_utf8_len) {
          filename_utf8_capacity <<= 1;
        }
        if ((filename_utf8 = (Byte*)SzAlloc(filename_utf8_capacity)) == 0) {
          res = SZ_ERROR_MEM;
          break;
        }
      }
#endif
      if (!Utf16Le_To_Utf8(filename_utf8, &filename_utf8_len, filename_utf16le, filename_utf16le_len)) {
        res = SZ_ERROR_BAD_FILENAME;
        break;
      }

      if (listCommand)
      {
        char s[32], t[32];

        UInt64ToStr(f->Size, s, 10, ' ');
        if (f->MTimeDefined)
          ConvertFileTimeToString(&f->MTime, t);
        else
        {
          size_t j;
          for (j = 0; j < 19; j++)
            t[j] = ' ';
          t[j] = '\0';
        }

        WriteMessage(t);
        WriteMessage(" . ");  /* attrib */
        WriteMessage(s);
        WriteMessage(" ");
        WriteMessage(" ");
        WriteMessage((const char*)filename_utf8);
        if (f->IsDir)
          WriteMessage("/");
        WriteMessage("\n");
        continue;
      }
      WriteMessage(testCommand ?
          "Testing    ":
          "Extracting ");
      WriteMessage((const char*)filename_utf8);
      if (f->IsDir) {
        WriteMessage("/");
      } else {
        if (extract_res != SZ_OK) { res = extract_res; break; }
      }
      if (!testCommand) {
        size_t processedSize;
        size_t j;
        for (j = 0; j < filename_utf8_len; j++) {
          if (filename_utf8[j] == '/') {
            filename_utf8[j] = '\0';
            res = MyCreateDir((const char*)filename_utf8, &umaskv, 0, 0);
            filename_utf8[j] = '/';
            if (res != SZ_OK) break;
          }
        }
        if (f->IsDir) {
          /* 7-Zip stores the directory after its contents, so it's safe to
           * make the directory read-only now.
           */
          if ((res = MyCreateDir((const char*)filename_utf8, &umaskv, f->AttribDefined, f->Attrib)) != SZ_OK) break;
        } else if (f->AttribDefined &&
                 (f->Attrib & FILE_ATTRIBUTE_UNIX_EXTENSION) &&
                 S_ISLNK(f->Attrib >> 16)) {
          Byte * const target = outBuffer + offset;
          Byte * const target_end = target + outSizeProcessed;
          const Byte target_end_byte = *target_end;
          Bool had_again = False;
         again:
          *target_end = '\0';  /* symlink() needs the NUL-terminator */
          /* SZ_OK == 0 is OK, other error codes are incorrect. */
          res = symlink((const char*)target, (const char*)filename_utf8);
          *target_end = target_end_byte;
          if (res == SZ_OK) {
          } else if (had_again || errno != EEXIST) { err:
            res = SZ_ERROR_WRITE_SYMLINK;
            break;
          } else if (!doYes) {
            res = SZ_ERROR_OVERWRITE;
            break;
          } else if (unlink((const char*)filename_utf8) == 0) {
            had_again = True;
            goto again;
#ifdef USE_CHMODW
          } else if (errno == EACCES) {  /* Permission denied. */
            MakeDirsWritable((char*)filename_utf8);
            if (unlink((const char*)filename_utf8) != 0) goto err;
            had_again = True;
            goto again;
#endif
          } else {
            goto err;
          }
        } else {
          int outFile;
          if ((res = OutFile_Open(&outFile, (char*)filename_utf8, doYes)) != SZ_OK) break;
          if (f->AttribDefined) {
            if (0 != fchmod(outFile, GetUnixMode(&umaskv, f->Attrib))) {
              res = SZ_ERROR_WRITE_CHMOD;
              close(outFile);
              break;
            }
          }
          processedSize = outSizeProcessed;
          if ((size_t)write(outFile, outBuffer + offset, processedSize) != processedSize) {
            close(outFile);
            res = SZ_ERROR_WRITE;
            break;
          }
          close(outFile);
          if (SetMTime((const char*)filename_utf8, f->MTimeDefined ? &f->MTime : NULL, tv)) {
            PrintError("can not set mtime");
            /* Don't break, it's not a big problem. */
          }
        }
      }
      WriteMessage("\n");
    }
    SzFree(outBuffer);
  }
  SzArEx_Free(&db);
  SzFree(filename_utf8);

  close(lookStream.fd);
  if (res == SZ_OK) {
    WriteMessage("\nEverything is Ok\n");
    return 0;
  } else if (res == SZ_ERROR_UNSUPPORTED) {
    PrintError("decoder doesn't support this archive");
  } else if (res == SZ_ERROR_MEM) {
    PrintError("can not allocate memory");
  } else if (res == SZ_ERROR_CRC) {
    PrintError("CRC error");
  } else if (res == SZ_ERROR_NO_ARCHIVE) {
    PrintError("input file is not a .7z archive");
  } else if (res == SZ_ERROR_OVERWRITE) {
    PrintError("already exists, specify -y to overwrite");
  } else if (res == SZ_ERROR_WRITE_OPEN) {
    PrintError("can not open output file");
  } else if (res == SZ_ERROR_WRITE_CHMOD) {
    PrintError("can not chmod output file");
  } else if (res == SZ_ERROR_WRITE) {
    PrintError("can not write output file");
  } else if (res == SZ_ERROR_BAD_FILENAME) {
    PrintError("bad filename (UTF-16 encoding)");
  } else if (res == SZ_ERROR_WRITE_MKDIR) {
    PrintError("can not create output dir");
  } else if (res == SZ_ERROR_WRITE_MKDIR_CHMOD) {
    PrintError("can not chmod output dir");
  } else if (res == SZ_ERROR_WRITE_SYMLINK) {
    PrintError("can not create symlink");
  } else {
    char s[12];
    UIntToStr(s, res, 0);
    WriteMessage("\nERROR #");
    WriteMessage(s);
    WriteMessage("\n");
  }
  return 1;
}
