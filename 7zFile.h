/* 7zFile.h -- File IO
2009-11-24 : Igor Pavlov : Public domain */

#ifndef __7Z_FILE_H
#define __7Z_FILE_H

#include "7zSys.h"

#include "Types.h"

EXTERN_C_BEGIN

/* ---------- File ---------- */

STATIC WRes InFile_Open(CSzFile *p, const char *name);
STATIC WRes OutFile_Open(CSzFile *p, const char *name);
STATIC WRes File_Close(CSzFile *p);

/* writes *size bytes */
STATIC WRes File_Write(CSzFile *p, const void *data, size_t *size);

EXTERN_C_END

#endif
