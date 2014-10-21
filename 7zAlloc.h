/* 7zAlloc.h -- Allocation functions
2010-10-29 : Igor Pavlov : Public domain */

#ifndef __7Z_ALLOC_H
#define __7Z_ALLOC_H

#include "Types.h"

#include <stdlib.h>

STATIC void *SzAlloc(size_t size);
STATIC void SzFree(void *address);

#endif
