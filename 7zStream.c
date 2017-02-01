/* 7zStream.c -- 7z Stream functions
2010-03-11 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "Types.h"

STATIC SRes LookInStream_SeekTo(CLookToRead *p, UInt64 offset)
{
#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEK LookInStream_SeekTo pos=%lld, origin=0, from=%ld\n", offset, (long)lseek(p->fd, 0, SEEK_CUR));
#endif
  /* TODO(pts): Use 64-bit offset. */
  const UInt64 offset0 = offset;
  offset = lseek(p->fd, (off_t)offset0, SEEK_SET);
  p->pos = p->size = 0;
  return offset == offset0 ? SZ_OK : SZ_ERROR_READ;
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
    if (originalSize != 0) {
#ifdef _SZ_READ_DEBUG
      fprintf(stderr, "READ size=%ld\n", (long)originalSize);
#endif
      *size = read(p->fd, p->buf + size2, originalSize);  /* -1 on error, need 0 */
      if (*size != originalSize) { *size = 0; res = SZ_ERROR_READ; }
    }
    size2 = p->size = *size += size2;
  }
  if (size2 < *size) *size = size2;
  *buf = p->buf + p->pos;
  return res;
}

STATIC SRes LookToRead_ReadAll(CLookToRead *p, void *buf, size_t *size) {
  Byte *lbuf;
  SRes res = SZ_OK;
  size_t got;
  while (*size > 0) {
    got = *size;
    res = LookToRead_Look_Exact(p, (const void**)&lbuf, &got);
    if (res != SZ_OK) break;
    if (got == 0) { res = SZ_ERROR_INPUT_EOF; break; }
    LOOKTOREAD_SKIP(p, got);
    memcpy(buf, lbuf, got);
    *size -= got;
    buf = (Byte*)buf + got;
  }
  return res;
}
