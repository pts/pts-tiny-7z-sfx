/* 7zBuf.c -- Byte Buffer
2008-03-28
Igor Pavlov
Public domain */

#include "7zAlloc.h"
#include "7zBuf.h"

STATIC void Buf_Init(CBuf *p)
{
  p->data = 0;
  p->size = 0;
}

STATIC int Buf_Create(CBuf *p, size_t size)
{
  p->size = 0;
  if (size == 0)
  {
    p->data = 0;
    return 1;
  }
  p->data = (Byte *)SzAlloc(size);
  if (p->data != 0)
  {
    p->size = size;
    return 1;
  }
  return 0;
}

STATIC void Buf_Free(CBuf *p)
{
  SzFree(p->data);
  p->data = 0;
  p->size = 0;
}
