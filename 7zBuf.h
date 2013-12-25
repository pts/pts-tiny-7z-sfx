/* 7zBuf.h -- Byte Buffer
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __7Z_BUF_H
#define __7Z_BUF_H

#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  Byte *data;
  size_t size;
} CBuf;

STATIC void Buf_Init(CBuf *p);
STATIC int Buf_Create(CBuf *p, size_t size, ISzAlloc *alloc);
STATIC void Buf_Free(CBuf *p, ISzAlloc *alloc);

typedef struct
{
  Byte *data;
  size_t size;
  size_t pos;
} CDynBuf;

STATIC void DynBuf_Construct(CDynBuf *p);
STATIC void DynBuf_SeekToBeg(CDynBuf *p);
STATIC int DynBuf_Write(CDynBuf *p, const Byte *buf, size_t size, ISzAlloc *alloc);
STATIC void DynBuf_Free(CDynBuf *p, ISzAlloc *alloc);

#ifdef __cplusplus
}
#endif

#endif
