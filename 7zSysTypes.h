#ifndef __7Z_SYS_TYPES_H
#define __7Z_SYS_TYPES_H

#ifdef USE_MINIINC1

#include <miniinc1.h>
typedef int64_t Int64;
typedef uint64_t UInt64;
#define UINT64_CONST(n) n ## ULL

#else

#include <stddef.h>  /* size_t */

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _SZ_NO_INT_64

/* define _SZ_NO_INT_64, if your compiler doesn't support 64-bit integers.
   NOTES: Some code will work incorrectly in that case! */

typedef long Int64;
typedef unsigned long UInt64;

#else

#include <stdint.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#define UINT64_CONST(n) n
#else
typedef int64_t Int64;
typedef uint64_t UInt64;
#define UINT64_CONST(n) n ## ULL
#endif

#endif

#endif  /* USE_MINIINC1 */

#endif
