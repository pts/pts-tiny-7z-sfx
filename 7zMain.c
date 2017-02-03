/* 7zMain.c - Test application for 7z Decoder2010-10-28 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7z.h"
#include "7zAlloc.h"
#include "7zCrc.h"
#include "7zVersion.h"

static int Buf_EnsureSize(CBuf *dest, size_t size)
{
  if (dest->size >= size)
    return 1;
  Buf_Free(dest);
  return Buf_Create(dest, size);
}

#ifndef _WIN32

static Byte kUtf8Limits[5] = { 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static Bool Utf16_To_Utf8(Byte *dest, size_t *destLen, const UInt16 *src, size_t srcLen)
{
  size_t destPos = 0, srcPos = 0;
  for (;;)
  {
    unsigned numAdds;
    UInt32 value;
    if (srcPos == srcLen)
    {
      *destLen = destPos;
      return True;
    }
    value = src[srcPos++];
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
      if (value >= 0xDC00 || srcPos == srcLen)
        break;
      c2 = src[srcPos++];
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

static SRes Utf16_To_Utf8Buf(CBuf *dest, const UInt16 *src, size_t srcLen)
{
  size_t destLen = 0;
  Bool res;
  Utf16_To_Utf8(NULL, &destLen, src, srcLen);
  destLen += 1;
  if (!Buf_EnsureSize(dest, destLen))
    return SZ_ERROR_MEM;
  res = Utf16_To_Utf8(dest->data, &destLen, src, srcLen);
  dest->data[destLen] = 0;
  return res ? SZ_OK : SZ_ERROR_FAIL;
}
#endif

static SRes Utf16_To_Char(CBuf *buf, const UInt16 *s)
{
  int len = 0;
  for (len = 0; s[len] != 0; len++);
  return Utf16_To_Utf8Buf(buf, s, len);
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

static void PrintMyCreateDirError(int res) {
  if (res == 1) {
    PrintError("can not create output dir");
  } else if (res == 2) {
    PrintError("can not chmod output dir");
  }
}

static WRes MyCreateDir(const UInt16 *name, unsigned *umaskv, Bool attribDefined, UInt32 attrib)
{
  unsigned mode;
  CBuf buf;
  Bool res;
  Buf_Init(&buf);
  RINOK(Utf16_To_Char(&buf, name));

  #ifdef _WIN32
  res = _mkdir((const char *)buf.data) == 0;
  #else
  mode = GetUnixMode(umaskv, attribDefined ? attrib :
      FILE_ATTRIBUTE_UNIX_EXTENSION | 0755 << 16);
  res = mkdir((const char *)buf.data, mode) == 0;
  if (!res && errno == EEXIST) res = 1;  /* Already exists. */
  if (res && attribDefined) {
    /* !! TODO(pts): chmod directory after its contents */
    if (0 != chmod((const char *)buf.data, GetUnixMode(umaskv, attrib))) {
      Buf_Free(&buf);
      return 2;
    }
  }
  #endif
  Buf_Free(&buf);
  return res ? 0 : 1;
}

#ifdef __linux
#define MyUtimes utimes
#else
/* Fallback which can't do subsecond precision. */
static int MyUtimes(const char *filename, const struct timeval tv[2]) {
  struct utimbuf times;
  times.actime = tv[0].tv_sec;
  times.modtime = tv[1].tv_sec;
  return utime(filename, &times);
}
#endif

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

static WRes SetMTime(const UInt16 *name, const CNtfsFileTime *mtime) {
  UInt32 usec;
  UInt64 sec;
  int got;
  struct timeval tv[2];
  CBuf buf;
  GetTimeSecAndUsec(mtime, &sec, &usec);
  sec -= (369 * 365 + 89) * (UInt64)(24 * 60 * 60);
  Buf_Init(&buf);
  RINOK(Utf16_To_Char(&buf, name));
  gettimeofday(&tv[0], NULL);
  tv[1].tv_sec = sec;
  tv[1].tv_usec = usec;
  got = MyUtimes((const char *)buf.data, tv);
  Buf_Free(&buf);
  return got != 0;
}

static SRes OutFile_OpenUtf16(int *p, const UInt16 *name, Bool doYes) {
  CBuf buf;
  WRes res;
  mode_t mode = O_WRONLY | O_CREAT | O_TRUNC;
  Buf_Init(&buf);
  RINOK(Utf16_To_Char(&buf, name));
  if (!doYes) mode |= O_EXCL;
  *p = open((const char *)buf.data, mode, 0644);
  res = *p >= 0 ? SZ_OK : errno == EEXIST ? SZ_ERROR_WRITE : SZ_ERROR_FAIL;
  Buf_Free(&buf);
  return res;
}

static SRes PrintString(const UInt16 *s)
{
  CBuf buf;
  SRes res;
  Buf_Init(&buf);
  res = Utf16_To_Char(&buf, s);
  if (res == SZ_OK)
    WriteMessage((const char *)buf.data);
  Buf_Free(&buf);
  return res;
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
  UInt16 *temp = NULL;
  size_t tempSize = 0;
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
  if ((lookStream.fd = open(archive, O_RDONLY)) < 0) {
    PrintError("can not open input file");
    return 1;
  }

  LOOKTOREAD_INIT(&lookStream);

  /*CrcGenerateTable();*/

  SzArEx_Init(&db);
  res = SzArEx_Open(&db, &lookStream);
  if (res == SZ_OK)
  {
    if (res == SZ_OK)
    {
      UInt32 i;

      /*
      if you need cache, use these 3 variables.
      if you use external function, you can make these variable as static.
      */
      UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
      Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
      size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */

      for (i = 0; i < db.db.NumFiles; i++)
      {
        size_t offset = 0;
        size_t outSizeProcessed = 0;
        const CSzFileItem *f = db.db.Files + i;
        size_t len;
        len = SzArEx_GetFileNameUtf16(&db, i, NULL);

        if (len > tempSize)
        {
          SzFree(temp);
          tempSize = len;
          temp = (UInt16 *)SzAlloc(tempSize * sizeof(temp[0]));
          if (temp == 0)
          {
            res = SZ_ERROR_MEM;
            break;
          }
        }

        SzArEx_GetFileNameUtf16(&db, i, temp);
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
          res = PrintString(temp);
          if (res != SZ_OK)
            break;
          if (f->IsDir)
            WriteMessage("/");
          WriteMessage("\n");
          continue;
        }
        WriteMessage(testCommand ?
            "Testing    ":
            "Extracting ");
        res = PrintString(temp);
        if (res != SZ_OK)
          break;
        if (f->IsDir)
          WriteMessage("/");
        else
        {
          res = SzArEx_Extract(&db, &lookStream, i,
              &blockIndex, &outBuffer, &outBufferSize,
              &offset, &outSizeProcessed);
          if (res != SZ_OK)
            break;
        }
        if (!testCommand)
        {
          int outFile = 0;  /* Initialize to 0 to pacify gcc-4.8. */
          size_t processedSize;
          size_t j;
          UInt16 *name = (UInt16 *)temp;
          const UInt16 *destPath = (const UInt16 *)name;
          for (j = 0; name[j] != 0; j++)
            if (name[j] == '/')
            {
              WRes dres;
              name[j] = 0;
              dres = MyCreateDir(name, &umaskv, 0, 0);
              name[j] = CHAR_PATH_SEPARATOR;
              if (dres) break;
            }

          if (f->IsDir)
          {
            /* 7-Zip stores the directory after its contents, so it's safe to
             * make the directory read-only now.
             */
            WRes dres = MyCreateDir(destPath, &umaskv, f->AttribDefined, f->Attrib);
            if (dres) {
              PrintMyCreateDirError(dres);
              break;
            }
            goto next_file;
          }
          else if (f->AttribDefined &&
                   (f->Attrib & FILE_ATTRIBUTE_UNIX_EXTENSION) &&
                   S_ISLNK(f->Attrib >> 16)) {
            char *target;
            CBuf buf;
            WRes sres;
            Buf_Init(&buf);
            if ((sres = Utf16_To_Char(&buf, name))) {
              PrintError("symlink malloc");
              res = sres;
              break;
            }
            target = (char*)SzAlloc(outSizeProcessed + 1);
            memcpy(target, outBuffer + offset, outSizeProcessed);
            target[outSizeProcessed] = '\0';
            if (0 != symlink(target, (const char *)buf.data)) {
              if (errno == EEXIST) {
                if (!doYes) {
                  res = SZ_ERROR_WRITE;
                  goto overw;
                }
                unlink((const char *)buf.data);
                if (0 == symlink(target, (const char *)buf.data)) goto reok;
              }
              PrintError("can not create symlink");
              res = SZ_ERROR_FAIL;
              SzFree(target);
              Buf_Free(&buf);
              break;
            }
           reok:
            SzFree(target);
            Buf_Free(&buf);
            goto next_file;
          }
          else if ((res = OutFile_OpenUtf16(&outFile, destPath, doYes)))
          {
           overw:
            PrintError(res == SZ_ERROR_WRITE ?
                       "already exists, specify -y to overwrite" :
                       "can not open output file");
            break;
          }
          if (f->AttribDefined) {
            if (0 != fchmod(outFile, GetUnixMode(&umaskv, f->Attrib))) {
              close(outFile);
              PrintError("can not chmod output file");
              res = SZ_ERROR_WRITE;
              break;
            }
          }
          processedSize = outSizeProcessed;
          if ((size_t)write(outFile, outBuffer + offset, processedSize) != processedSize) {
            close(outFile);
            PrintError("can not write output file");
            res = SZ_ERROR_WRITE;
            break;
          }
          close(outFile);
          if (f->MTimeDefined) {
            if (SetMTime(destPath, &f->MTime)) {
              res = SZ_ERROR_FAIL;
              PrintError("can not set mtime");
              /* Don't break, it's not a big problem. */
            }
          }
        }
       next_file:
        WriteMessage("\n");
      }
      SzFree(outBuffer);
    }
  }
  SzArEx_Free(&db);
  SzFree(temp);

  close(lookStream.fd);
  if (res == SZ_OK)
  {
    WriteMessage("\nEverything is Ok\n");
    return 0;
  }
  if (res == SZ_ERROR_UNSUPPORTED)
    PrintError("decoder doesn't support this archive");
  else if (res == SZ_ERROR_MEM)
    PrintError("can not allocate memory");
  else if (res == SZ_ERROR_CRC)
    PrintError("CRC error");
  else if (res == SZ_ERROR_NO_ARCHIVE)
    PrintError("input file is not a .7z archive");
  else {
    char s[12];
    UIntToStr(s, res, 0);
    WriteMessage("\nERROR #");
    WriteMessage(s);
    WriteMessage("\n");
  }
  return 1;
}
