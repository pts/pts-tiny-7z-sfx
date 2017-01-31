/* 7zAlloc.c -- Allocation functions
2010-10-29 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7zAlloc.h"

/* #define _SZ_ALLOC_DEBUG */
/* use _SZ_ALLOC_DEBUG to debug alloc/free operations */


STATIC void *SzAlloc(size_t size)
{
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

STATIC void SzFree(void *address)
{
  #ifdef _SZ_ALLOC_DEBUG
  fprintf(stderr, "DYNAMIC FREE %p\n", address);
  #endif
  free(address);
}
