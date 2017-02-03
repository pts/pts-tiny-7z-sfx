/* 7zAlloc.c -- Allocation functions
2010-10-29 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7zAlloc.h"

/* #define _SZ_ALLOC_DEBUG */
/* use _SZ_ALLOC_DEBUG to debug alloc/free operations */

#ifdef USE_MINIALLOC

/* Almost 2 GB of virtual memory, should be enough. Please note that physical
 * memory is not allocated until used.
 *
 * TODO(pts): Return an explicit out-of-memory error rather than a segfault.
 *            But how do we do that, even if we use mmap?
 */
static char heap[2100000000], *heap_end = heap, *prev_alloc = 0;

STATIC void *SzAlloc(size_t size) {
  char *result;
  if (size == 0)
    return 0;
  /* It wouldn't fit to the rest of heap. */
  if (size > (size_t)(heap + sizeof(heap) - heap_end)) return 0;
  heap_end += -(size_t)heap_end & 7;  /* Align to 8-byte boundary. */
  prev_alloc = result = heap_end;
  heap_end += size;
#ifndef USE_MINIINC1
#ifdef _SZ_ALLOC_DEBUG
  fprintf(stderr, "DYNAMIC ALLOC %d = %p\n", size, result);
#endif
#endif
  return result;
}

STATIC void SzFree(void *address) {
#ifndef USE_MINIINC1
#ifdef _SZ_ALLOC_DEBUG
  fprintf(stderr, "DYNAMIC FREE %p\n", address);
#endif
#endif
  /* Only undo the previous allocation, have memory leaks for everything else.
   * Doing anything smarter would make the malloc implementation longer.
   * This is already useful, it matches how how 7z extractor uses memory.
   */
  if (!address) {
  } else if (address == prev_alloc) {
#ifdef _SZ_ALLOC_DEBUG
    (void)!write(2, "#", 1);
#endif
    heap_end = prev_alloc;
    prev_alloc = 0;
  } else {
#ifdef _SZ_ALLOC_DEBUG
    (void)!write(2, ".", 1);
#endif
  }
}

#else  /* Not USE_MINIALLOC */

STATIC void *SzAlloc(size_t size) {
  if (size == 0)
    return 0;
#ifdef _SZ_ALLOC_DEBUG
  void *r = malloc(size);
  fprintf(stderr, "DYNAMIC ALLOC %d = %p\n", size, r);
  return r;
#else
  return malloc(size);
#endif
}

STATIC void SzFree(void *address) {
  #ifdef _SZ_ALLOC_DEBUG
  fprintf(stderr, "DYNAMIC FREE %p\n", address);
  #endif
  free(address);
}

#endif
