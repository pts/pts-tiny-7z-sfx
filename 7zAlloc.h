/* 7zAlloc.h -- Allocation functions
2010-10-29 : Igor Pavlov : Public domain */

#ifndef __7Z_ALLOC_H
#define __7Z_ALLOC_H

#include "Types.h"

#include <stdlib.h>

STATIC void *SzAlloc(void *p, size_t size);
STATIC void SzFree(void *p, void *address);

STATIC void *SzAllocTemp(void *p, size_t size);
STATIC void SzFreeTemp(void *p, void *address);

#endif
