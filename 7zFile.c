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
