/* 7zAlloc.c -- Allocation functions
2010-10-29 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7zAlloc.h"

/* Use _SZ_ALLOC_DEBUG to debug alloc/free operations. */

#ifdef USE_MINIALLOC

#ifdef USE_MINIINC1
static char *heap_pos;
struct AssertSsizeTSizeAtMost8 { int _ : sizeof(ssize_t) <= 8; };
#else
/* 2.1 GB doesn't work on some Ubuntu 14.04 amd64 systems, Linux 3.13.
 * It's unreliable, sometimes 2.0 GB doesn't work either.
 */
#define HEAP_SIZE 220000000  /* 220 MB */
/* Almost 2 GB of virtual memory, should be enough. Please note that physical
 * memory is not allocated until used.
 *
 * There is a linked list: heap_pos - ((ssize_t*)heap_pos)[-1] points to the
 * previous value of heap_pos (i.e. the return value of the most recent
 * SzAlloc). But if ((ssize_t*)heap_pos)[-1] is negative, then it's a
 * free block, and heap_pos - ~((ssize_t*)heap_pos)[-1] goes back 1 step.
 */
static char heap[HEAP_SIZE] __attribute__((aligned(8)));
static char *heap_pos = heap + 8;
#endif

#ifdef _SZ_ALLOC_ASSERT
#define ALLOC_ASSERT(cond, msg) do { \
  if (!(cond)) { \
    (void)!write(2, "SZ_ALLOC_ASSERT FAILURE, EXPECTED: ", 35); \
    (void)!write(2, (msg), sizeof(msg) - 1); \
    (void)!write(2, "\n", 1); \
    *(char*)0 = 0;  /* Cause Segmentation fault. */ \
  } \
} while (0);
#else
#define ALLOC_ASSERT(cond, msg) do {} while (0)
#endif

STATIC void *SzAlloc(size_t size) {
#ifdef USE_MINIINC1
  static char *base, *end;
  ssize_t new_heap_size;
  size_t delta;
#endif
  size_t size_padded;
  char *result;
  if ((ssize_t)size <= 0) return 0;  /* Disallow 0, check for overflow. */
  size_padded = size + sizeof(ssize_t);
  size_padded += -size_padded & 7;  /* Align to 8-byte boundary. */
#ifdef USE_MINIINC1
  delta = 0;
  if (!base || size_padded > (size_t)(end - heap_pos)) {
    if (!base) {
      if (!(base = heap_pos = (char*)sys_brk(NULL))) return NULL;  /* Error getting the initial data segment size for the very first time. */
      new_heap_size = 64 << 10;  /* 64 KiB. */
      delta = 8;
      size_padded += delta;
      goto grow_heap;  /* TODO(pts): Reset base to NULL if we overflow below. */
    }
    while (size_padded > (size_t)(end - heap_pos)) {  /* Double the heap size until there is `size' bytes heap_pos. */
      new_heap_size = (end - base) << 1;  /* !! TODO(pts): Don't allocate more than 1 MiB if not needed. */
     grow_heap:
      if ((ssize_t)new_heap_size <= 0 || (size_t)base + new_heap_size < (size_t)base) return NULL;  /* Heap would be too large. */
      if ((char*)sys_brk(base + new_heap_size) != base + new_heap_size) return NULL;  /* Out of memory. */
      end = base + new_heap_size;
    }
  }
  heap_pos += size_padded;
  size_padded -= delta;
  result = heap_pos - size_padded;
  if (delta != 0) {
    ((ssize_t*)result)[-1] = -1;  /* Mark first block as used. */
  } else {
    ALLOC_ASSERT(((ssize_t*)result)[-1] >= 0, "heap_pos is free in SzAlloc.");
    /* GCC complies this negation to a `not' instruction on i386 and amd64. */
    ((ssize_t*)result)[-1] ^= (ssize_t)-1;  /* Mark block as used. */
  }
#else
  /* Report out-of-memory error. */
  if (size_padded > (size_t)(heap + sizeof(heap) - heap_pos)) return 0;
  ALLOC_ASSERT(((ssize_t*)heap_pos)[-1] >= 0, "heap_pos is free in SzAlloc.");
  /* GCC complies this negation to a `not' instruction on i386 and amd64. */
  ((ssize_t*)heap_pos)[-1] ^= (ssize_t)-1;  /* Mark block as used. */
  result = heap_pos;
  heap_pos += size_padded;
#endif
   /* Mark next block (heap_pos) as free, set backward linked list pointer. */
  ((ssize_t*)heap_pos)[-1] = size_padded;
#ifdef _SZ_ALLOC_DEBUG
#ifdef USE_MINIINC1
  (void)!write(2, "@", 1);
#else
  fprintf(stderr, "DYNAMIC ALLOC %lld = %p AT %lld\n", (long long)size, result, (long long)(heap_pos - heap));
#endif
#endif
  return result;
}

STATIC void SzFree(void *address) {
#ifdef _SZ_ALLOC_DEBUG
#ifdef USE_MINIINC1
  (void)!write(2, ".", 1);
#else
  fprintf(stderr, "DYNAMIC FREE %p\n", address);
#endif
#endif
  /* Only reclaim memory from backwards in heap_pos.
   * Doing anything smarter would make the malloc implementation longer.
   * Please note that memory is not handed back to the operating system.
   * This is already useful, it matches how how 7z extractor uses memory.
   */
  if (address) {
    ALLOC_ASSERT(((ssize_t*)address)[-1] < 0, "address is used in SzFree.");
    ALLOC_ASSERT(((ssize_t*)heap_pos)[-1] >= 0, "heap_pos is free in SzFree.");
    ((ssize_t*)address)[-1] ^= (ssize_t)-1;  /* Mark block as free. */
    while (((ssize_t*)(address = heap_pos - ((ssize_t*)heap_pos)[-1]))[-1] >= 0 &&
           address != heap_pos  /* Beginning of heap, equalivalent to checking heap_pos != heap + 8. */) {
#ifdef _SZ_ALLOC_DEBUG
      (void)!write(2, "$\n", 2);
#endif
      heap_pos = address;
    }
#ifdef _SZ_ALLOC_DEBUG
    if (heap_pos == heap + 8) {
      (void)!write(2, "DYNAMIC HEAP IS EMPTY\n", 22);
    }
#ifndef USE_MINIINC1
    fprintf(stderr, "DYNAMIC REMAINING %lld %p\n", (long long)(heap_pos - heap), heap_pos);
#endif
#endif
  }
}

#else  /* Not USE_MINIALLOC */

STATIC void *SzAlloc(size_t size) {
  if (size == 0)
    return 0;
#ifdef _SZ_ALLOC_DEBUG
  void *r = malloc(size);
  fprintf(stderr, "DYNAMIC ALLOC %lld = %p\n", (long long)size, r);
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
