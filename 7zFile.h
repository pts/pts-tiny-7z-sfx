/* 7zFile.h -- File IO
2009-11-24 : Igor Pavlov : Public domain */

#ifndef __7Z_FILE_H
#define __7Z_FILE_H

#include "7zSys.h"

#include "Types.h"

EXTERN_C_BEGIN

/* ---------- File ---------- */

typedef struct
{
  #ifdef USE_WINDOWS_FILE
  HANDLE handle;
  #else
  FILE *file;
  #endif
} CSzFile;

#if !defined(UNDER_CE) || !defined(USE_WINDOWS_FILE)
STATIC WRes InFile_Open(CSzFile *p, const char *name);
STATIC WRes OutFile_Open(CSzFile *p, const char *name);
#endif
#ifdef USE_WINDOWS_FILE
STATIC WRes InFile_OpenW(CSzFile *p, const WCHAR *name);
STATIC WRes OutFile_OpenW(CSzFile *p, const WCHAR *name);
#endif
STATIC WRes File_Close(CSzFile *p);

/* reads max(*size, remain file's size) bytes */
STATIC WRes File_Read(CSzFile *p, void *data, size_t *size);

/* writes *size bytes */
STATIC WRes File_Write(CSzFile *p, const void *data, size_t *size);

STATIC WRes File_Seek(CSzFile *p, Int64 *pos, ESzSeek origin);


/* ---------- FileInStream ---------- */

typedef struct
{
  ISeqInStream s;
  CSzFile file;
} CFileSeqInStream;

typedef struct
{
  ISeekInStream s;
  CSzFile file;
} CFileInStream;

STATIC void FileInStream_CreateVTable(CFileInStream *p);


typedef struct
{
  ISeqOutStream s;
  CSzFile file;
} CFileOutStream;

EXTERN_C_END

#endif
