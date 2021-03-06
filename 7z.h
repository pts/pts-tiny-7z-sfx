/* 7z.h -- 7z interface
2010-03-11 : Igor Pavlov : Public domain */

#ifndef __7Z_H
#define __7Z_H

#include "Types.h"

EXTERN_C_BEGIN

#define k7zStartHeaderSize 0x20
#define k7zSignatureSize 6
/* The first byte is deliberately wrong, it should be '7' */
/* extern const Byte k7zSignature[k7zSignatureSize]; */
#define k7zMajorVersion 0

enum EIdEnum
{
  k7zIdEnd,
  k7zIdHeader,
  k7zIdArchiveProperties,
  k7zIdAdditionalStreamsInfo,
  k7zIdMainStreamsInfo,
  k7zIdFilesInfo,
  k7zIdPackInfo,
  k7zIdUnpackInfo,
  k7zIdSubStreamsInfo,
  k7zIdSize,
  k7zIdCRC,
  k7zIdFolder,
  k7zIdCodersUnpackSize,
  k7zIdNumUnpackStream,
  k7zIdEmptyStream,
  k7zIdEmptyFile,
  k7zIdAnti,
  k7zIdName,
  k7zIdCTime,
  k7zIdATime,
  k7zIdMTime,
  k7zIdWinAttributes,
  k7zIdComment,
  k7zIdEncodedHeader,
  k7zIdStartPos,
  k7zIdDummy
};

typedef struct
{
  UInt32 NumInStreams;
  UInt32 NumOutStreams;
  UInt64 MethodID;
  Byte *Props;
  size_t PropsSize;
} CSzCoderInfo;

typedef struct
{
  UInt32 InIndex;
  UInt32 OutIndex;
} CSzBindPair;

typedef struct
{
  CSzCoderInfo *Coders;
  CSzBindPair *BindPairs;
  UInt32 *PackStreams;
  UInt64 *UnpackSizes;
  UInt32 NumCoders;
  UInt32 NumBindPairs;
  UInt32 NumPackStreams;
  int UnpackCRCDefined;
  UInt32 UnpackCRC;

  UInt32 NumUnpackStreams;
} CSzFolder;

STATIC void SzFolder_Init(CSzFolder *p);
STATIC UInt64 SzFolder_GetUnpackSize(CSzFolder *p);
STATIC int SzFolder_FindBindPairForInStream(CSzFolder *p, UInt32 inStreamIndex);
STATIC UInt32 SzFolder_GetNumOutStreams(CSzFolder *p);
STATIC UInt64 SzFolder_GetUnpackSize(CSzFolder *p);

STATIC SRes SzFolder_Decode(const CSzFolder *folder, const UInt64 *packSizes,
    CLookToRead *stream, UInt64 startPos,
    Byte *outBuffer, size_t outSize);

typedef struct
{
  UInt32 Low;
  UInt32 High;
} CNtfsFileTime;

typedef struct
{
  CNtfsFileTime MTime;  /* Initialized only if MTimeDefined. */
  UInt64 Size;
  UInt32 Crc;  /* Initialized only if CrcDefined. */
  UInt32 Attrib;  /* Undefined of (UInt32)-1. */
  Byte HasStream;
  Byte IsDir;
  Byte CrcDefined;
  Byte MTimeDefined;
} CSzFileItem;
#define FILE_ATTRIBUTE_READONLY             1
/*#define FILE_ATTRIBUTE_DIRECTORY           16*/
#define FILE_ATTRIBUTE_UNIX_EXTENSION   0x8000

typedef struct
{
  UInt64 *PackSizes;
  Byte *PackCRCsDefined;
  UInt32 *PackCRCs;
  CSzFolder *Folders;
  CSzFileItem *Files;
  UInt32 NumPackStreams;
  UInt32 NumFolders;
  UInt32 NumFiles;
} CSzAr;

STATIC void SzAr_Init(CSzAr *p);
STATIC void SzAr_Free(CSzAr *p);


/*
  SzExtract extracts file from archive

  *outBuffer must be 0 before first call for each new archive.

  Extracting cache:
    If you need to decompress more than one file, you can send
    these values from previous call:
      *blockIndex,
      *outBuffer,
      *outBufferSize
    You can consider "*outBuffer" as cache of solid block. If your archive is solid,
    it will increase decompression speed.

    If you use external function, you can declare these 3 cache variables
    (blockIndex, outBuffer, outBufferSize) as static in that external function.

    Free *outBuffer and set *outBuffer to 0, if you want to flush cache.
*/

typedef struct
{
  CSzAr db;

  UInt64 startPosAfterHeader;
  UInt64 dataPos;

  UInt32 *FolderStartPackStreamIndex;
  UInt64 *PackStreamStartPositions;
  UInt32 *FolderStartFileIndex;
  UInt32 *FileIndexToFolderIndexMap;

  size_t *FileNameOffsets; /* in 2-byte steps */
  Byte *FileNamesInHeaderBufPtr;  /* UTF-16-LE */
  Byte *HeaderBufStart;  /* Buffer containing FileNamesInHeaderBufPtr. */
} CSzArEx;

/*static void SzArEx_Init(CSzArEx *p);*/
STATIC void SzArEx_Free(CSzArEx *p);
STATIC UInt64 SzArEx_GetFolderStreamPos(const CSzArEx *p, UInt32 folderIndex, UInt32 indexInFolder);

STATIC SRes SzArEx_Extract(
    const CSzArEx *db,
    CLookToRead *inStream,
    UInt32 fileIndex,         /* index of file */
    UInt32 *blockIndex,       /* index of solid block */
    Byte **outBuffer,         /* pointer to pointer to output buffer (allocated with allocMain) */
    size_t *outBufferSize,    /* buffer size for output buffer */
    size_t *offset,           /* offset of stream for required file in *outBuffer */
    size_t *outSizeProcessed); /* size of file in *outBuffer */


/*
SzArEx_Open Errors:
SZ_ERROR_NO_ARCHIVE
SZ_ERROR_ARCHIVE
SZ_ERROR_UNSUPPORTED
SZ_ERROR_MEM
SZ_ERROR_CRC
SZ_ERROR_INPUT_EOF
SZ_ERROR_FAIL
*/

STATIC SRes SzArEx_Open(CSzArEx *p, CLookToRead *inStream);

EXTERN_C_END

#endif
