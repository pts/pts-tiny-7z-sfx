#ifndef __7Z_SYS_H
#define __7Z_SYS_H

#if defined(USE_MINIINC1) && !defined(USE_MINIALLOC_SYS_BRK)
#  define USE_MINIALLOC_SYS_BRK 1
#endif

#if defined(USE_MINIALLOC_SYS_BRK) && !defined(USE_MINIALLOC)
#  define USE_MINIALLOC 1
#endif

#ifdef USE_MINIINC1
#include <miniinc1.h>
#else

#ifndef _WIN32
#ifdef __linux
#ifndef _GNU_SOURCE
#define _GNU_SOURCE  /* For futimesat() */
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE  /* For utimes() in diet libc. */
#endif
#include <sys/time.h>
#endif
#endif

/* for mkdir */
#ifdef _WIN32
#include <direct.h>
#else
#include <fcntl.h>  /* open() */
#include <sys/stat.h>
#include <errno.h>
#include <utime.h>
#include <unistd.h>  /* symlink() */
#include <sys/time.h>  /* futimes() for uClibc */
#endif

#include <stdlib.h>
#include <string.h>

#if defined(_SZ_ALLOC_DEBUG) || defined(_SZ_SEEK_DEBUG) || defined(_SZ_HEADER_DEBUG) || defined(_SZ_READ_DEBUG) || defined(_SZ_CODER_DEBUG) || defined(_SZ_CHMODW_DEBUG)
#include <stdio.h>
#endif

#ifndef UNDER_CE
#include <errno.h>
#endif

#ifdef USE_STAT64
#define stat stat64
#define lstat lstat64
#endif

#endif  /* USE_MINIINC1 */

#endif
