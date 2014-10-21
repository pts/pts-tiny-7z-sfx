#ifndef __7Z_SYS_H
#define __7Z_SYS_H

#ifdef _WIN32
#define USE_WINDOWS_FILE
#endif

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

#ifndef USE_WINDOWS_FILE
/* for mkdir */
#ifdef _WIN32
#include <direct.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <utime.h>
#include <unistd.h>  /* symlink() */
#include <sys/time.h>  /* futimes() for uClibc */
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_WINDOWS_FILE
#include <windows.h>
#else
#include <stdio.h>
#endif

#ifndef UNDER_CE
#include <errno.h>
#endif


#endif
