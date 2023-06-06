/* 7zStream.c -- 7z Stream functions
2010-03-11 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "Types.h"

STATIC SRes LookInStream_SeekTo(CLookToRead *p, UInt64 offset)
{
#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEK LookInStream_SeekTo pos=%lld, origin=0, from=%lld\n", (long long)offset, (long long)lseek64(p->fd, 0, SEEK_CUR));
#endif
#ifdef USE_MINIINC1
  const int result = lseek64set(p->fd, offset);
  p->pos = p->size = 0;
  return result == 0 ? SZ_OK : SZ_ERROR_READ;
#else
#ifdef __MINILIBC686__
  const int result = lseek64_set(p->fd, offset);
  p->pos = p->size = 0;
  return result == 0 ? SZ_OK : SZ_ERROR_READ;
#else
#ifdef __WATCOMC__  /* OpenWatcom doesn't have the lseek64(...) function, and its _lseeki64(...) function is buggy. */
  /*const UInt64 offset1 = _lseeki64(p->fd, offset, SEEK_SET);*/  /* Buggy. */
  Int64 offset1;
  if (_llseek(p->fd, offset >> 32, offset, &offset1, SEEK_SET)) return SZ_ERROR_READ;
#else
  const UInt64 offset1 = lseek64(p->fd, offset, SEEK_SET);
#endif
  p->pos = p->size = 0;
  return offset == (UInt64)offset1 ? SZ_OK : SZ_ERROR_READ;
#endif
#endif
}

STATIC SRes LookToRead_Look(CLookToRead *p, const void **buf, size_t *size) {
  SRes res = SZ_OK;
  size_t size_in_buf = p->size - p->pos;
  ssize_t rsize;

#ifdef _SZ_READ_DEBUG
  fprintf(stderr, "READ size_in_buf=%lld size=%lld pos=%lld\n", (long long)size_in_buf, (long long)*size, (long long)p->pos);
#endif
  /* Checking size_in_buf <= p->pos is needed to avoid overlap in memcpy. */
  if (*size > size_in_buf && size_in_buf <= p->pos) {  /* TODO(pts): Do we need memmove instead? */
    memcpy(p->buf, p->buf + p->pos, size_in_buf);
    p->pos = 0;
    /* True but we can do it later: p->size = size_in_buf; */
    /* : res = FileInStream_Read(p->realStream, p->buf + size_in_buf, size); */
    /* We fill the buffer, we read more than: *size - size_in_buf; */
    rsize = LookToRead_BUF_SIZE - size_in_buf;
#ifdef _SZ_READ_DEBUG
    fprintf(stderr, "READ rsize=%ld\n", (long)rsize);
#endif
    rsize = read(p->fd, p->buf + size_in_buf, rsize);
    if (rsize < 0) { rsize = 0; res = SZ_ERROR_READ; }
    p->size = *size = size_in_buf += rsize;
  } else {
    *size = size_in_buf;
  }
  *buf = p->buf + p->pos;
  return res;
}

STATIC SRes LookToRead_ReadAll(CLookToRead *p, void *buf, size_t size) {
  Byte *lbuf;
  SRes res = SZ_OK;
  size_t got;
  while (size > 0) {
    got = size;
    res = LookToRead_Look(p, (const void**)&lbuf, &got);
    if (res != SZ_OK) break;
    if (got == 0) { res = SZ_ERROR_INPUT_EOF; break; }
    LOOKTOREAD_SKIP(p, got);
    memcpy(buf, lbuf, got);
    size -= got;
    buf = (Byte*)buf + got;
  }
  return res;
}
