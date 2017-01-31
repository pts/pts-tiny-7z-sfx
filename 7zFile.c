/* 7zFile.c -- File IO
2009-11-24 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7zFile.h"

static WRes File_Open(CSzFile *p, const char *name, int writeMode)
{
  p->file = fopen(name, writeMode ? "wb+" : "rb");
  return (p->file != 0) ? 0 : errno;
}

STATIC WRes InFile_Open(CSzFile *p, const char *name) { return File_Open(p, name, 0); }
STATIC WRes OutFile_Open(CSzFile *p, const char *name) { return File_Open(p, name, 1); }

STATIC WRes File_Close(CSzFile *p)
{
  if (p->file != NULL)
  {
    int res = fclose(p->file);
    if (res != 0)
      return res;
    p->file = NULL;
  }
  return 0;
}

STATIC WRes File_Write(CSzFile *p, const void *data, size_t *size)
{
  size_t originalSize = *size;
  if (originalSize == 0)
    return 0;

  *size = fwrite(data, 1, originalSize, p->file);
  if (*size == originalSize)
    return 0;
  return ferror(p->file);
}

STATIC WRes File_Seek(CSzFile *p, Int64 *pos, ESzSeek origin)
{
  int moveMethod;
  int res;
  switch (origin)
  {
    case SZ_SEEK_SET: moveMethod = SEEK_SET; break;
    case SZ_SEEK_END: moveMethod = SEEK_END; break;
    default: return 1;
  }
  res = fseek(p->file, (long)*pos, moveMethod);
  *pos = ftell(p->file);
  return res;
}

/* ---------- FileSeqInStream ---------- */

/* ---------- FileInStream ---------- */

static SRes FileInStream_Read(void *pp, void *data, size_t *size)
{
  CFileInStream *p = (CFileInStream *)pp;
  size_t originalSize = *size;
  if (originalSize == 0)
    return SZ_OK;
  *size = fread(data, 1, originalSize, p->file.file);
  if (*size == originalSize)
    return SZ_OK;
  return SZ_ERROR_READ;
}

static SRes FileInStream_Seek(void *pp, Int64 *pos, ESzSeek origin)
{
  CFileInStream *p = (CFileInStream *)pp;
#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEK FileInStream_Seek pos=%lld, origin=%d\n", *pos, origin);
#endif
  return File_Seek(&p->file, pos, origin);
}

STATIC void FileInStream_CreateVTable(CFileInStream *p)
{
  p->s.Read = FileInStream_Read;
  p->s.Seek = FileInStream_Seek;
}


/* ---------- FileOutStream ---------- */
