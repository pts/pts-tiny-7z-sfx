/* Types.h -- Basic types
2010-10-09 : Igor Pavlov : Public domain */

#ifndef __7Z_TYPES_H
#define __7Z_TYPES_H

#include "7zSysTypes.h"

#ifndef EXTERN_C_BEGIN
#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif
#endif

#ifndef STATIC
#define STATIC
#endif

EXTERN_C_BEGIN

#define SZ_OK 0

#define SZ_ERROR_DATA 1
#define SZ_ERROR_MEM 2
#define SZ_ERROR_CRC 3
#define SZ_ERROR_UNSUPPORTED 4
#define SZ_ERROR_PARAM 5
#define SZ_ERROR_INPUT_EOF 6
#define SZ_ERROR_OUTPUT_EOF 7
#define SZ_ERROR_READ 8
#define SZ_ERROR_WRITE 9
#define SZ_ERROR_PROGRESS 10
#define SZ_ERROR_FAIL 11
#define SZ_ERROR_THREAD 12

#define SZ_ERROR_ARCHIVE 16
#define SZ_ERROR_NO_ARCHIVE 17

typedef int SRes;

#ifdef _WIN32
typedef DWORD WRes;
#else
typedef int WRes;
#endif

#ifndef RINOK
#define RINOK(x) { int __result__ = (x); if (__result__ != 0) return __result__; }
#endif

typedef unsigned char Byte;
typedef short Int16;
typedef unsigned short UInt16;

#ifdef _LZMA_UINT32_IS_ULONG
typedef long Int32;
typedef unsigned long UInt32;
#else
typedef int Int32;
typedef unsigned int UInt32;
#endif

#ifdef _LZMA_NO_SYSTEM_SIZE_T
typedef UInt32 size_t;
typedef Int32 ssize_t;
#endif

typedef int Bool;
#define True 1
#define False 0


#ifdef _WIN32
#define MY_STD_CALL __stdcall
#else
#define MY_STD_CALL
#endif

#ifdef _MSC_VER

#if _MSC_VER >= 1300
#define MY_NO_INLINE __declspec(noinline)
#else
#define MY_NO_INLINE
#endif

#define MY_CDECL __cdecl
#define MY_FAST_CALL __fastcall

#else

#define MY_CDECL
#define MY_FAST_CALL

#endif


/* The following interfaces use first parameter as pointer to structure */

typedef struct
{
  Byte (*Read)(void *p); /* reads one byte, returns 0 in case of EOF or error */
} IByteIn;

typedef struct
{
  void (*Write)(void *p, Byte b);
} IByteOut;

typedef struct
{
  SRes (*Read)(void *p, void *buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
       (output(*size) < input(*size)) is allowed */
} ISeqInStream;

typedef struct
{
  size_t (*Write)(void *p, const void *buf, size_t size);
    /* Returns: result - the number of actually written bytes.
       (result < size) means error */
} ISeqOutStream;

struct CFileInStream;

#define LookToRead_BUF_SIZE (1 << 14)

typedef struct
{
  int fd;
  size_t pos;
  size_t size;
  Byte buf[LookToRead_BUF_SIZE];
} CLookToRead;

STATIC SRes LookInStream_SeekTo(CLookToRead *p, UInt64 offset);

/* STATIC void LookToRead_Init(CLookToRead *p) */
#define LOOKTOREAD_INIT(p) do { (p)->pos = (p)->size = 0; } while (0)
/* 1. If less than *size bytes are already in the input buffer, then fills the
 *    rest of the input buffer from disk.
 * 2. Sets *size to the number of bytes now in the input buffer. Can be more or
 *    less or equal to the original *size. Detect EOF by calling
 *    LOOKTOREAD_SKIP(*size), calling LookToRead_Look again, and then checking
 *    *size == 0.
 */
STATIC SRes LookToRead_Look(CLookToRead *p, const void **buf, size_t *size);
STATIC SRes LookToRead_ReadAll(CLookToRead *p, void *buf, size_t *size);
/* offset must be <= output(*size) of Look */
/* STATIC SRes LookToRead_Skip(CLookToRead *p, size_t offset) */
#define LOOKTOREAD_SKIP(p, offset) do { (p)->pos += (offset); } while (0)

typedef struct
{
  SRes (*Progress)(void *p, UInt64 inSize, UInt64 outSize);
    /* Returns: result. (result != SZ_OK) means break.
       Value (UInt64)(Int64)-1 for size means unknown value. */
} ICompressProgress;

#ifdef _WIN32

#define CHAR_PATH_SEPARATOR '\\'
#define WCHAR_PATH_SEPARATOR L'\\'
#define STRING_PATH_SEPARATOR "\\"
#define WSTRING_PATH_SEPARATOR L"\\"

#else

#define CHAR_PATH_SEPARATOR '/'
#define WCHAR_PATH_SEPARATOR L'/'
#define STRING_PATH_SEPARATOR "/"
#define WSTRING_PATH_SEPARATOR L"/"

#endif

EXTERN_C_END

#endif
