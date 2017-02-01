/* 7zStream.c -- 7z Stream functions
2010-03-11 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "Types.h"

STATIC SRes LookInStream_SeekTo(CLookToRead *stream, UInt64 offset)
{
  Int64 t = offset;
#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEK LookInStream_SeekTo pos=%lld, origin=0\n", offset);
#endif
  return LookToRead_Seek(stream, &t);
}

STATIC SRes LookInStream_Read(CLookToRead *p, void *buf, size_t size)
{
  while (size != 0) {
    size_t processed = size;
    size_t rem = p->size - p->pos;
    if (rem == 0) {
      RINOK(FileInStream_Read(p->realStream, buf, &processed));
    } else {
      if (rem > processed) rem = processed;
      memcpy(buf, p->buf + p->pos, rem);
      p->pos += rem;
      processed = rem;
    }    
    if (processed == 0) return SZ_ERROR_INPUT_EOF;
    buf = (void *)((Byte *)buf + processed);
    size -= processed;
  }
  return SZ_OK;
}

STATIC SRes LookToRead_Look_Exact(CLookToRead *p, const void **buf, size_t *size)
{
  SRes res = SZ_OK;
  size_t size2 = p->size - p->pos;
  if (*size == 0) return size2;
  if (*size > LookToRead_BUF_SIZE) *size = LookToRead_BUF_SIZE;

  /* Checking size2 <= p->pos is needed to avoid overlap in memcpy. */
  /* TODO(pts): Do we need memmove instead? */
  if (size2 < *size && size2 <= p->pos) {
    memcpy(p->buf, p->buf + p->pos, size2);
    p->pos = 0;
    /* True but we can do it later: p->size = size2; */
    *size -= size2;
    res = FileInStream_Read(p->realStream, p->buf + size2, size);
    size2 = p->size = *size += size2;
  }
  if (size2 < *size) *size = size2;
  *buf = p->buf + p->pos;
  return res;
}

STATIC SRes LookToRead_Skip(CLookToRead *p, size_t offset)
{
  p->pos += offset;
  return SZ_OK;
}

STATIC SRes LookToRead_Seek(CLookToRead *p, Int64 *pos)
{
  p->pos = p->size = 0;
#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEK LookToRead_Seek pos=%lld, origin=0\n", *pos);
#endif
  return FileInStream_Seek(p->realStream, pos);
}

STATIC void LookToRead_Init(CLookToRead *p)
{
  p->pos = p->size = 0;
}
