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

STATIC SRes LookToRead_Look_Exact(CLookToRead *p, const void **buf, size_t *size)
{
  SRes res = SZ_OK;
  size_t size2 = p->size - p->pos;
  size_t originalSize;

  if (*size == 0) return size2;
  if (*size > LookToRead_BUF_SIZE) *size = LookToRead_BUF_SIZE;

  /* Checking size2 <= p->pos is needed to avoid overlap in memcpy. */
  /* TODO(pts): Do we need memmove instead? */
  if (size2 < *size && size2 <= p->pos) {
    memcpy(p->buf, p->buf + p->pos, size2);
    p->pos = 0;
    /* True but we can do it later: p->size = size2; */
    /* : res = FileInStream_Read(p->realStream, p->buf + size2, size); */
    originalSize = *size -= size2;
    if (originalSize == 0) {
      res = SZ_OK;
    } else {
      *size = fread(p->buf + size2, 1, originalSize, p->realStream->file.file);  /* 0 on error */
      res = (*size == originalSize) ? SZ_OK : SZ_ERROR_READ;
    }
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
#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEK FileInStream_Seek pos=%lld, origin=0, from=%ld\n", *pos, ftell(p->realStream->file.file));
#endif
  /* TODO(pts): Use fseeko for 64-bit offset. */
  Int64 pos0 = *pos;
  int res = fseek(p->realStream->file.file, (long)pos0, SEEK_SET);
  p->pos = p->size = 0;
  *pos = ftell(p->realStream->file.file);
  return res == 0 && *pos == pos0 ? SZ_OK : SZ_ERROR_READ;
}

STATIC void LookToRead_Init(CLookToRead *p)
{
  p->pos = p->size = 0;
}
