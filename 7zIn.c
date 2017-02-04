/* 7zIn.c -- 7z Input functions
2010-10-29 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7z.h"
#include "7zAlloc.h"
#include "7zCrc.h"
#include "CpuArch.h"

#if 0
/* Using 'S' instead of '7' in the beginning so that the 7-Zip signature
 * won't be detected with the SFX binary. Will replace it with '7' later.
 */
STATIC const Byte k7zSignature[k7zSignatureSize] = {'7', 'z', 0xBC, 0xAF, 0x27, 0x1C};
#endif

#if 0
/* This is a nice trick, but unfortunately it's not a const expression.
 * http://stackoverflow.com/questions/4286671/endianness-conversion-in-arm
 */
STATIC const char isBigEndian = (*(UInt16*)"\0\xff" < 0x100);
#endif

/* gcc -m64 -dM -E - </dev/null defines: i386 __i386 __i386__ __x86_64
 * __x86_64__ __amd64 __amd64__
 */
#ifdef MY_CPU_LE_UNALIGN
#define IS_7Z_SIGNATURE(p) (*(const UInt16*)(p) == ('z' << 8 | '7') && *(const UInt32*)((const UInt16*)(p) + 1) == 0x1C27AFBC)
#else
STATIC const Byte k57zSignature[k7zSignatureSize - 1] = {'z', 0xBC, 0xAF, 0x27, 0x1C};
/* We check the 1st byte separately to avoid having k7ZSignature in the
 * executable .rodata, thus detecting our signature instea of the .7z file in an sfx.
 */
#define IS_7Z_SIGNATURE(p) (((const Byte*)(p))[0] == '7' && 0 == memcmp((const Byte*)(p) + 1, k57zSignature, 5))
#endif

#define RINOM(x) { if ((x) == 0) return SZ_ERROR_MEM; }

#define NUM_FOLDER_CODERS_MAX 32
#define NUM_CODER_STREAMS_MAX 32

STATIC void SzCoderInfo_Init(CSzCoderInfo *p)
{
  Buf_Init(&p->Props);
}

STATIC void SzCoderInfo_Free(CSzCoderInfo *p)
{
  Buf_Free(&p->Props);
  SzCoderInfo_Init(p);
}

STATIC void SzFolder_Init(CSzFolder *p)
{
  p->Coders = 0;
  p->BindPairs = 0;
  p->PackStreams = 0;
  p->UnpackSizes = 0;
  p->NumCoders = 0;
  p->NumBindPairs = 0;
  p->NumPackStreams = 0;
  p->UnpackCRCDefined = 0;
  p->UnpackCRC = 0;
  p->NumUnpackStreams = 0;
}

STATIC void SzFolder_Free(CSzFolder *p)
{
  UInt32 i;
  if (p->Coders)
    for (i = 0; i < p->NumCoders; i++)
      SzCoderInfo_Free(&p->Coders[i]);
  SzFree(p->Coders);
  SzFree(p->BindPairs);
  SzFree(p->PackStreams);
  SzFree(p->UnpackSizes);
  SzFolder_Init(p);
}

STATIC UInt32 SzFolder_GetNumOutStreams(CSzFolder *p)
{
  UInt32 result = 0;
  UInt32 i;
  for (i = 0; i < p->NumCoders; i++)
    result += p->Coders[i].NumOutStreams;
  return result;
}

STATIC int SzFolder_FindBindPairForInStream(CSzFolder *p, UInt32 inStreamIndex)
{
  UInt32 i;
  for (i = 0; i < p->NumBindPairs; i++)
    if (p->BindPairs[i].InIndex == inStreamIndex)
      return i;
  return -1;
}


STATIC int SzFolder_FindBindPairForOutStream(CSzFolder *p, UInt32 outStreamIndex)
{
  UInt32 i;
  for (i = 0; i < p->NumBindPairs; i++)
    if (p->BindPairs[i].OutIndex == outStreamIndex)
      return i;
  return -1;
}

STATIC UInt64 SzFolder_GetUnpackSize(CSzFolder *p)
{
  int i = (int)SzFolder_GetNumOutStreams(p);
  if (i == 0)
    return 0;
  for (i--; i >= 0; i--)
    if (SzFolder_FindBindPairForOutStream(p, i) < 0)
      return p->UnpackSizes[i];
  /* throw 1; */
  return 0;
}

STATIC void SzFile_Init(CSzFileItem *p)
{
  p->HasStream = 1;
  p->IsDir = 0;
  p->IsAnti = 0;
  p->CrcDefined = 0;
  p->MTimeDefined = 0;
}

STATIC void SzAr_Init(CSzAr *p)
{
  p->PackSizes = 0;
  p->PackCRCsDefined = 0;
  p->PackCRCs = 0;
  p->Folders = 0;
  p->Files = 0;
  p->NumPackStreams = 0;
  p->NumFolders = 0;
  p->NumFiles = 0;
}

STATIC void SzAr_Free(CSzAr *p)
{
  UInt32 i;
  if (p->Folders)
    for (i = 0; i < p->NumFolders; i++)
      SzFolder_Free(&p->Folders[i]);

  SzFree(p->PackSizes);
  SzFree(p->PackCRCsDefined);
  SzFree(p->PackCRCs);
  SzFree(p->Folders);
  SzFree(p->Files);
  SzAr_Init(p);
}


STATIC void SzArEx_Init(CSzArEx *p)
{
  SzAr_Init(&p->db);
  p->FolderStartPackStreamIndex = 0;
  p->PackStreamStartPositions = 0;
  p->FolderStartFileIndex = 0;
  p->FileIndexToFolderIndexMap = 0;
  p->FileNameOffsets = 0;
  Buf_Init(&p->FileNames);
}

STATIC void SzArEx_Free(CSzArEx *p)
{
  SzFree(p->FolderStartPackStreamIndex);
  SzFree(p->PackStreamStartPositions);
  SzFree(p->FolderStartFileIndex);
  SzFree(p->FileIndexToFolderIndexMap);

  SzFree(p->FileNameOffsets);
  Buf_Free(&p->FileNames);

  SzAr_Free(&p->db);
  SzArEx_Init(p);
}

/*
UInt64 GetFolderPackStreamSize(int folderIndex, int streamIndex) const
{
  return PackSizes[FolderStartPackStreamIndex[folderIndex] + streamIndex];
}

UInt64 GetFilePackSize(int fileIndex) const
{
  int folderIndex = FileIndexToFolderIndexMap[fileIndex];
  if (folderIndex >= 0)
  {
    const CSzFolder &folderInfo = Folders[folderIndex];
    if (FolderStartFileIndex[folderIndex] == fileIndex)
    return GetFolderFullPackSize(folderIndex);
  }
  return 0;
}
*/

#define MY_ALLOC(T, p, size) { if ((size) == 0) p = 0; else \
  if ((p = (T *)SzAlloc((size) * sizeof(T))) == 0) return SZ_ERROR_MEM; }

static SRes SzArEx_Fill(CSzArEx *p)
{
  UInt32 startPos = 0;
  UInt64 startPosSize = 0;
  UInt32 i;
  UInt32 folderIndex = 0;
  UInt32 indexInFolder = 0;
  MY_ALLOC(UInt32, p->FolderStartPackStreamIndex, p->db.NumFolders);
  for (i = 0; i < p->db.NumFolders; i++)
  {
    p->FolderStartPackStreamIndex[i] = startPos;
    startPos += p->db.Folders[i].NumPackStreams;
  }

  MY_ALLOC(UInt64, p->PackStreamStartPositions, p->db.NumPackStreams);

  for (i = 0; i < p->db.NumPackStreams; i++)
  {
    p->PackStreamStartPositions[i] = startPosSize;
    startPosSize += p->db.PackSizes[i];
  }

  MY_ALLOC(UInt32, p->FolderStartFileIndex, p->db.NumFolders);
  MY_ALLOC(UInt32, p->FileIndexToFolderIndexMap, p->db.NumFiles);

  for (i = 0; i < p->db.NumFiles; i++)
  {
    CSzFileItem *file = p->db.Files + i;
    int emptyStream = !file->HasStream;
    if (emptyStream && indexInFolder == 0)
    {
      p->FileIndexToFolderIndexMap[i] = (UInt32)-1;
      continue;
    }
    if (indexInFolder == 0)
    {
      /*
      v3.13 incorrectly worked with empty folders
      v4.07: Loop for skipping empty folders
      */
      for (;;)
      {
        if (folderIndex >= p->db.NumFolders)
          return SZ_ERROR_ARCHIVE;
        p->FolderStartFileIndex[folderIndex] = i;
        if (p->db.Folders[folderIndex].NumUnpackStreams != 0)
          break;
        folderIndex++;
      }
    }
    p->FileIndexToFolderIndexMap[i] = folderIndex;
    if (emptyStream)
      continue;
    indexInFolder++;
    if (indexInFolder >= p->db.Folders[folderIndex].NumUnpackStreams)
    {
      folderIndex++;
      indexInFolder = 0;
    }
  }
  return SZ_OK;
}


STATIC UInt64 SzArEx_GetFolderStreamPos(const CSzArEx *p, UInt32 folderIndex, UInt32 indexInFolder)
{
  return p->dataPos +
    p->PackStreamStartPositions[p->FolderStartPackStreamIndex[folderIndex] + indexInFolder];
}

typedef struct _CSzState
{
  Byte *Data;
  size_t Size;
}CSzData;

static SRes SzReadByte(CSzData *sd, Byte *b)
{
  if (sd->Size == 0)
    return SZ_ERROR_ARCHIVE;
  sd->Size--;
  *b = *sd->Data++;
  return SZ_OK;
}

static SRes SzReadBytes(CSzData *sd, Byte *data, size_t size)
{
  size_t i;
  for (i = 0; i < size; i++)
  {
    RINOK(SzReadByte(sd, data + i));
  }
  return SZ_OK;
}

static SRes SzReadUInt32(CSzData *sd, UInt32 *value)
{
  int i;
  *value = 0;
  for (i = 0; i < 4; i++)
  {
    Byte b;
    RINOK(SzReadByte(sd, &b));
    *value |= ((UInt32)(b) << (8 * i));
  }
  return SZ_OK;
}

static SRes SzReadNumber(CSzData *sd, UInt64 *value)
{
  Byte firstByte;
  Byte mask = 0x80;
  int i;
  RINOK(SzReadByte(sd, &firstByte));
  *value = 0;
  for (i = 0; i < 8; i++)
  {
    Byte b;
    if ((firstByte & mask) == 0)
    {
      UInt64 highPart = firstByte & (mask - 1);
      *value += (highPart << (8 * i));
      return SZ_OK;
    }
    RINOK(SzReadByte(sd, &b));
    *value |= ((UInt64)b << (8 * i));
    mask >>= 1;
  }
  return SZ_OK;
}

static SRes SzReadNumber32(CSzData *sd, UInt32 *value)
{
  UInt64 value64;
  RINOK(SzReadNumber(sd, &value64));
  if (value64 >= 0x80000000)
    return SZ_ERROR_UNSUPPORTED;
  if (value64 >= ((UInt64)(1) << ((sizeof(size_t) - 1) * 8 + 2)))
    return SZ_ERROR_UNSUPPORTED;
  *value = (UInt32)value64;
  return SZ_OK;
}

static SRes SzReadID(CSzData *sd, UInt64 *value)
{
  return SzReadNumber(sd, value);
}

static SRes SzSkeepDataSize(CSzData *sd, UInt64 size)
{
  if (size > sd->Size)
    return SZ_ERROR_ARCHIVE;
  sd->Size -= (size_t)size;
  sd->Data += (size_t)size;
  return SZ_OK;
}

static SRes SzSkeepData(CSzData *sd)
{
  UInt64 size;
  RINOK(SzReadNumber(sd, &size));
  return SzSkeepDataSize(sd, size);
}

static SRes SzReadArchiveProperties(CSzData *sd)
{
  for (;;)
  {
    UInt64 type;
    RINOK(SzReadID(sd, &type));
    if (type == k7zIdEnd)
      break;
    SzSkeepData(sd);
  }
  return SZ_OK;
}

static SRes SzWaitAttribute(CSzData *sd, UInt64 attribute)
{
  for (;;)
  {
    UInt64 type;
    RINOK(SzReadID(sd, &type));
    if (type == attribute)
      return SZ_OK;
    if (type == k7zIdEnd)
      return SZ_ERROR_ARCHIVE;
    RINOK(SzSkeepData(sd));
  }
}

static SRes SzReadBoolVector(CSzData *sd, size_t numItems, Byte **v)
{
  Byte b = 0;
  Byte mask = 0;
  size_t i;
  MY_ALLOC(Byte, *v, numItems);
  for (i = 0; i < numItems; i++)
  {
    if (mask == 0)
    {
      RINOK(SzReadByte(sd, &b));
      mask = 0x80;
    }
    (*v)[i] = (Byte)(((b & mask) != 0) ? 1 : 0);
    mask >>= 1;
  }
  return SZ_OK;
}

static SRes SzReadBoolVector2(CSzData *sd, size_t numItems, Byte **v)
{
  Byte allAreDefined;
  size_t i;
  RINOK(SzReadByte(sd, &allAreDefined));
  if (allAreDefined == 0)
    return SzReadBoolVector(sd, numItems, v);
  MY_ALLOC(Byte, *v, numItems);
  for (i = 0; i < numItems; i++)
    (*v)[i] = 1;
  return SZ_OK;
}

static SRes SzReadHashDigests(
    CSzData *sd,
    size_t numItems,
    Byte **digestsDefined,
    UInt32 **digests)
{
  size_t i;
  RINOK(SzReadBoolVector2(sd, numItems, digestsDefined));
  MY_ALLOC(UInt32, *digests, numItems);
  for (i = 0; i < numItems; i++)
    if ((*digestsDefined)[i])
    {
      RINOK(SzReadUInt32(sd, (*digests) + i));
    }
  return SZ_OK;
}

static SRes SzReadPackInfo(
    CSzData *sd,
    UInt64 *dataOffset,
    UInt32 *numPackStreams,
    UInt64 **packSizes,
    Byte **packCRCsDefined,
    UInt32 **packCRCs)
{
  UInt32 i;
  RINOK(SzReadNumber(sd, dataOffset));
  RINOK(SzReadNumber32(sd, numPackStreams));

  RINOK(SzWaitAttribute(sd, k7zIdSize));

  MY_ALLOC(UInt64, *packSizes, (size_t)*numPackStreams);

  for (i = 0; i < *numPackStreams; i++)
  {
    RINOK(SzReadNumber(sd, (*packSizes) + i));
  }

  for (;;)
  {
    UInt64 type;
    RINOK(SzReadID(sd, &type));
    if (type == k7zIdEnd)
      break;
    if (type == k7zIdCRC)
    {
      RINOK(SzReadHashDigests(sd, (size_t)*numPackStreams, packCRCsDefined, packCRCs));
      continue;
    }
    RINOK(SzSkeepData(sd));
  }
  if (*packCRCsDefined == 0)
  {
    MY_ALLOC(Byte, *packCRCsDefined, (size_t)*numPackStreams);
    MY_ALLOC(UInt32, *packCRCs, (size_t)*numPackStreams);
    for (i = 0; i < *numPackStreams; i++)
    {
      (*packCRCsDefined)[i] = 0;
      (*packCRCs)[i] = 0;
    }
  }
  return SZ_OK;
}

static SRes SzReadSwitch(CSzData *sd)
{
  Byte external;
  RINOK(SzReadByte(sd, &external));
  return (external == 0) ? SZ_OK: SZ_ERROR_UNSUPPORTED;
}

static SRes SzGetNextFolderItem(CSzData *sd, CSzFolder *folder)
{
  UInt32 numCoders, numBindPairs, numPackStreams, i;
  UInt32 numInStreams = 0, numOutStreams = 0;

  RINOK(SzReadNumber32(sd, &numCoders));
  if (numCoders > NUM_FOLDER_CODERS_MAX)
    return SZ_ERROR_UNSUPPORTED;
  folder->NumCoders = numCoders;

  MY_ALLOC(CSzCoderInfo, folder->Coders, (size_t)numCoders);

  for (i = 0; i < numCoders; i++)
    SzCoderInfo_Init(folder->Coders + i);

  for (i = 0; i < numCoders; i++)
  {
    Byte mainByte;
    CSzCoderInfo *coder = folder->Coders + i;
    {
      unsigned idSize, j;
      Byte longID[15];
      RINOK(SzReadByte(sd, &mainByte));
      idSize = (unsigned)(mainByte & 0xF);
      RINOK(SzReadBytes(sd, longID, idSize));
      if (idSize > sizeof(coder->MethodID))
        return SZ_ERROR_UNSUPPORTED;
      coder->MethodID = 0;
      for (j = 0; j < idSize; j++)
        coder->MethodID |= (UInt64)longID[idSize - 1 - j] << (8 * j);

      if ((mainByte & 0x10) != 0)
      {
        RINOK(SzReadNumber32(sd, &coder->NumInStreams));
        RINOK(SzReadNumber32(sd, &coder->NumOutStreams));
        if (coder->NumInStreams > NUM_CODER_STREAMS_MAX ||
            coder->NumOutStreams > NUM_CODER_STREAMS_MAX)
          return SZ_ERROR_UNSUPPORTED;
      }
      else
      {
        coder->NumInStreams = 1;
        coder->NumOutStreams = 1;
      }
      if ((mainByte & 0x20) != 0)
      {
        UInt64 propertiesSize = 0;
        RINOK(SzReadNumber(sd, &propertiesSize));
        if (!Buf_Create(&coder->Props, (size_t)propertiesSize))
          return SZ_ERROR_MEM;
        RINOK(SzReadBytes(sd, coder->Props.data, (size_t)propertiesSize));
      }
    }
    while ((mainByte & 0x80) != 0)
    {
      RINOK(SzReadByte(sd, &mainByte));
      RINOK(SzSkeepDataSize(sd, (mainByte & 0xF)));
      if ((mainByte & 0x10) != 0)
      {
        UInt32 n;
        RINOK(SzReadNumber32(sd, &n));
        RINOK(SzReadNumber32(sd, &n));
      }
      if ((mainByte & 0x20) != 0)
      {
        UInt64 propertiesSize = 0;
        RINOK(SzReadNumber(sd, &propertiesSize));
        RINOK(SzSkeepDataSize(sd, propertiesSize));
      }
    }
    numInStreams += coder->NumInStreams;
    numOutStreams += coder->NumOutStreams;
  }

  if (numOutStreams == 0)
    return SZ_ERROR_UNSUPPORTED;

  folder->NumBindPairs = numBindPairs = numOutStreams - 1;
  MY_ALLOC(CSzBindPair, folder->BindPairs, (size_t)numBindPairs);

  for (i = 0; i < numBindPairs; i++)
  {
    CSzBindPair *bp = folder->BindPairs + i;
    RINOK(SzReadNumber32(sd, &bp->InIndex));
    RINOK(SzReadNumber32(sd, &bp->OutIndex));
  }

  if (numInStreams < numBindPairs)
    return SZ_ERROR_UNSUPPORTED;

  folder->NumPackStreams = numPackStreams = numInStreams - numBindPairs;
  MY_ALLOC(UInt32, folder->PackStreams, (size_t)numPackStreams);

  if (numPackStreams == 1)
  {
    for (i = 0; i < numInStreams ; i++)
      if (SzFolder_FindBindPairForInStream(folder, i) < 0)
        break;
    if (i == numInStreams)
      return SZ_ERROR_UNSUPPORTED;
    folder->PackStreams[0] = i;
  }
  else
    for (i = 0; i < numPackStreams; i++)
    {
      RINOK(SzReadNumber32(sd, folder->PackStreams + i));
    }
  return SZ_OK;
}

static SRes SzReadUnpackInfo(
    CSzData *sd,
    UInt32 *numFolders,
    CSzFolder **folders)  /* for alloc */
{
  UInt32 i;
  RINOK(SzWaitAttribute(sd, k7zIdFolder));
  RINOK(SzReadNumber32(sd, numFolders));
  {
    RINOK(SzReadSwitch(sd));

    MY_ALLOC(CSzFolder, *folders, (size_t)*numFolders);

    for (i = 0; i < *numFolders; i++)
      SzFolder_Init((*folders) + i);

    for (i = 0; i < *numFolders; i++)
    {
      RINOK(SzGetNextFolderItem(sd, (*folders) + i));
    }
  }

  RINOK(SzWaitAttribute(sd, k7zIdCodersUnpackSize));

  for (i = 0; i < *numFolders; i++)
  {
    UInt32 j;
    CSzFolder *folder = (*folders) + i;
    UInt32 numOutStreams = SzFolder_GetNumOutStreams(folder);

    MY_ALLOC(UInt64, folder->UnpackSizes, (size_t)numOutStreams);

    for (j = 0; j < numOutStreams; j++)
    {
      RINOK(SzReadNumber(sd, folder->UnpackSizes + j));
    }
  }

  for (;;)
  {
    UInt64 type;
    RINOK(SzReadID(sd, &type));
    if (type == k7zIdEnd)
      return SZ_OK;
    if (type == k7zIdCRC)
    {
      SRes res;
      Byte *crcsDefined = 0;
      UInt32 *crcs = 0;
      res = SzReadHashDigests(sd, *numFolders, &crcsDefined, &crcs);
      if (res == SZ_OK)
      {
        for (i = 0; i < *numFolders; i++)
        {
          CSzFolder *folder = (*folders) + i;
          folder->UnpackCRCDefined = crcsDefined[i];
          folder->UnpackCRC = crcs[i];
        }
      }
      SzFree(crcs);
      SzFree(crcsDefined);
      RINOK(res);
      continue;
    }
    RINOK(SzSkeepData(sd));
  }
}

static SRes SzReadSubStreamsInfo(
    CSzData *sd,
    UInt32 numFolders,
    CSzFolder *folders,
    UInt32 *numUnpackStreams,
    UInt64 **unpackSizes,
    Byte **digestsDefined,
    UInt32 **digests)
{
  UInt64 type = 0;
  UInt32 i;
  UInt32 si = 0;
  UInt32 numDigests = 0;

  for (i = 0; i < numFolders; i++)
    folders[i].NumUnpackStreams = 1;
  *numUnpackStreams = numFolders;

  for (;;)
  {
    RINOK(SzReadID(sd, &type));
    if (type == k7zIdNumUnpackStream)
    {
      *numUnpackStreams = 0;
      for (i = 0; i < numFolders; i++)
      {
        UInt32 numStreams;
        RINOK(SzReadNumber32(sd, &numStreams));
        folders[i].NumUnpackStreams = numStreams;
        *numUnpackStreams += numStreams;
      }
      continue;
    }
    if (type == k7zIdCRC || type == k7zIdSize)
      break;
    if (type == k7zIdEnd)
      break;
    RINOK(SzSkeepData(sd));
  }

  if (*numUnpackStreams == 0)
  {
    *unpackSizes = 0;
    *digestsDefined = 0;
    *digests = 0;
  }
  else
  {
    *unpackSizes = (UInt64 *)SzAlloc((size_t)*numUnpackStreams * sizeof(UInt64));
    RINOM(*unpackSizes);
    *digestsDefined = (Byte *)SzAlloc((size_t)*numUnpackStreams * sizeof(Byte));
    RINOM(*digestsDefined);
    *digests = (UInt32 *)SzAlloc((size_t)*numUnpackStreams * sizeof(UInt32));
    RINOM(*digests);
  }

  for (i = 0; i < numFolders; i++)
  {
    /*
    v3.13 incorrectly worked with empty folders
    v4.07: we check that folder is empty
    */
    UInt64 sum = 0;
    UInt32 j;
    UInt32 numSubstreams = folders[i].NumUnpackStreams;
    if (numSubstreams == 0)
      continue;
    if (type == k7zIdSize)
    for (j = 1; j < numSubstreams; j++)
    {
      UInt64 size;
      RINOK(SzReadNumber(sd, &size));
      (*unpackSizes)[si++] = size;
      sum += size;
    }
    (*unpackSizes)[si++] = SzFolder_GetUnpackSize(folders + i) - sum;
  }
  if (type == k7zIdSize)
  {
    RINOK(SzReadID(sd, &type));
  }

  for (i = 0; i < *numUnpackStreams; i++)
  {
    (*digestsDefined)[i] = 0;
    (*digests)[i] = 0;
  }


  for (i = 0; i < numFolders; i++)
  {
    UInt32 numSubstreams = folders[i].NumUnpackStreams;
    if (numSubstreams != 1 || !folders[i].UnpackCRCDefined)
      numDigests += numSubstreams;
  }


  si = 0;
  for (;;)
  {
    if (type == k7zIdCRC)
    {
      int digestIndex = 0;
      Byte *digestsDefined2 = 0;
      UInt32 *digests2 = 0;
      SRes res = SzReadHashDigests(sd, numDigests, &digestsDefined2, &digests2);
      if (res == SZ_OK)
      {
        for (i = 0; i < numFolders; i++)
        {
          CSzFolder *folder = folders + i;
          UInt32 numSubstreams = folder->NumUnpackStreams;
          if (numSubstreams == 1 && folder->UnpackCRCDefined)
          {
            (*digestsDefined)[si] = 1;
            (*digests)[si] = folder->UnpackCRC;
            si++;
          }
          else
          {
            UInt32 j;
            for (j = 0; j < numSubstreams; j++, digestIndex++)
            {
              (*digestsDefined)[si] = digestsDefined2[digestIndex];
              (*digests)[si] = digests2[digestIndex];
              si++;
            }
          }
        }
      }
      SzFree(digestsDefined2);
      SzFree(digests2);
      RINOK(res);
    }
    else if (type == k7zIdEnd)
      return SZ_OK;
    else
    {
      RINOK(SzSkeepData(sd));
    }
    RINOK(SzReadID(sd, &type));
  }
}


static SRes SzReadStreamsInfo(
    CSzData *sd,
    UInt64 *dataOffset,
    CSzAr *p,
    UInt32 *numUnpackStreams,
    UInt64 **unpackSizes, /* allocTemp */
    Byte **digestsDefined,   /* allocTemp */
    UInt32 **digests)        /* allocTemp */
{
  for (;;)
  {
    UInt64 type;
    RINOK(SzReadID(sd, &type));
    if ((UInt64)(int)type != type)
      return SZ_ERROR_UNSUPPORTED;
    switch((int)type)
    {
      case k7zIdEnd:
        return SZ_OK;
      case k7zIdPackInfo:
      {
        RINOK(SzReadPackInfo(sd, dataOffset, &p->NumPackStreams,
            &p->PackSizes, &p->PackCRCsDefined, &p->PackCRCs));
        break;
      }
      case k7zIdUnpackInfo:
      {
        RINOK(SzReadUnpackInfo(sd, &p->NumFolders, &p->Folders));
        break;
      }
      case k7zIdSubStreamsInfo:
      {
        RINOK(SzReadSubStreamsInfo(sd, p->NumFolders, p->Folders,
            numUnpackStreams, unpackSizes, digestsDefined, digests));
        break;
      }
      default:
        return SZ_ERROR_UNSUPPORTED;
    }
  }
}

STATIC size_t SzArEx_GetFileNameUtf16(const CSzArEx *p, size_t fileIndex, UInt16 *dest)
{
  size_t len = p->FileNameOffsets[fileIndex + 1] - p->FileNameOffsets[fileIndex];
  if (dest != 0)
  {
    size_t i;
    const Byte *src = p->FileNames.data + (p->FileNameOffsets[fileIndex] * 2);
    for (i = 0; i < len; i++)
      dest[i] = GetUi16(src + i * 2);
  }
  return len;
}

static SRes SzReadFileNames(const Byte *p, size_t size, UInt32 numFiles, size_t *sizes)
{
  UInt32 i;
  size_t pos = 0;
  for (i = 0; i < numFiles; i++)
  {
    sizes[i] = pos;
    for (;;)
    {
      if (pos >= size)
        return SZ_ERROR_ARCHIVE;
      if (p[pos * 2] == 0 && p[pos * 2 + 1] == 0)
        break;
      pos++;
    }
    pos++;
  }
  sizes[i] = pos;
  return (pos == size) ? SZ_OK : SZ_ERROR_ARCHIVE;
}

static SRes SzReadHeader2(
    CSzArEx *p,   /* allocMain */
    CSzData *sd,
    UInt64 **unpackSizes,  /* allocTemp */
    Byte **digestsDefined,    /* allocTemp */
    UInt32 **digests,         /* allocTemp */
    Byte **emptyStreamVector, /* allocTemp */
    Byte **emptyFileVector,   /* allocTemp */
    Byte **lwtVector)         /* allocTemp */
{
  UInt64 type;
  UInt32 numUnpackStreams = 0;
  UInt32 numFiles = 0;
  CSzFileItem *files = 0;
  UInt32 numEmptyStreams = 0;
  UInt32 i;

  RINOK(SzReadID(sd, &type));

  if (type == k7zIdArchiveProperties)
  {
    RINOK(SzReadArchiveProperties(sd));
    RINOK(SzReadID(sd, &type));
  }


  if (type == k7zIdMainStreamsInfo)
  {
    RINOK(SzReadStreamsInfo(sd,
        &p->dataPos,
        &p->db,
        &numUnpackStreams,
        unpackSizes,
        digestsDefined,
        digests));
    p->dataPos += p->startPosAfterHeader;
    RINOK(SzReadID(sd, &type));
  }

  if (type == k7zIdEnd)
    return SZ_OK;
  if (type != k7zIdFilesInfo)
    return SZ_ERROR_ARCHIVE;

  RINOK(SzReadNumber32(sd, &numFiles));
  p->db.NumFiles = numFiles;

  MY_ALLOC(CSzFileItem, files, (size_t)numFiles);

  p->db.Files = files;
  for (i = 0; i < numFiles; i++)
    SzFile_Init(files + i);

  for (;;)
  {
    UInt64 type;
    UInt64 size;
    RINOK(SzReadID(sd, &type));
    if (type == k7zIdEnd)
      break;
    RINOK(SzReadNumber(sd, &size));
    if (size > sd->Size)
      return SZ_ERROR_ARCHIVE;
    if ((UInt64)(int)type != type)
    {
      RINOK(SzSkeepDataSize(sd, size));
    }
    else
    switch((int)type)
    {
      case k7zIdName:
      {
        size_t namesSize;
        RINOK(SzReadSwitch(sd));
        namesSize = (size_t)size - 1;
        if ((namesSize & 1) != 0)
          return SZ_ERROR_ARCHIVE;
        if (!Buf_Create(&p->FileNames, namesSize))
          return SZ_ERROR_MEM;
        MY_ALLOC(size_t, p->FileNameOffsets, numFiles + 1);
        memcpy(p->FileNames.data, sd->Data, namesSize);
        RINOK(SzReadFileNames(sd->Data, namesSize >> 1, numFiles, p->FileNameOffsets))
        RINOK(SzSkeepDataSize(sd, namesSize));
        break;
      }
      case k7zIdEmptyStream:
      {
        RINOK(SzReadBoolVector(sd, numFiles, emptyStreamVector));
        numEmptyStreams = 0;
        for (i = 0; i < numFiles; i++)
          if ((*emptyStreamVector)[i])
            numEmptyStreams++;
        break;
      }
      case k7zIdEmptyFile:
      {
        RINOK(SzReadBoolVector(sd, numEmptyStreams, emptyFileVector));
        break;
      }
      case k7zIdWinAttributes:
      {
        RINOK(SzReadBoolVector2(sd, numFiles, lwtVector));
        RINOK(SzReadSwitch(sd));
        for (i = 0; i < numFiles; i++)
        {
          CSzFileItem *f = &files[i];
          Byte defined = (*lwtVector)[i];
          f->AttribDefined = defined;
          f->Attrib = 0;
          if (defined)
          {
            RINOK(SzReadUInt32(sd, &f->Attrib));
          }
        }
        SzFree(*lwtVector);
        *lwtVector = NULL;
        break;
      }
      case k7zIdMTime:
      {
        RINOK(SzReadBoolVector2(sd, numFiles, lwtVector));
        RINOK(SzReadSwitch(sd));
        for (i = 0; i < numFiles; i++)
        {
          CSzFileItem *f = &files[i];
          Byte defined = (*lwtVector)[i];
          f->MTimeDefined = defined;
          f->MTime.Low = f->MTime.High = 0;
          if (defined)
          {
            RINOK(SzReadUInt32(sd, &f->MTime.Low));
            RINOK(SzReadUInt32(sd, &f->MTime.High));
          }
        }
        SzFree(*lwtVector);
        *lwtVector = NULL;
        break;
      }
      default:
      {
        RINOK(SzSkeepDataSize(sd, size));
      }
    }
  }

  {
    UInt32 emptyFileIndex = 0;
    UInt32 sizeIndex = 0;
    for (i = 0; i < numFiles; i++)
    {
      CSzFileItem *file = files + i;
      file->IsAnti = 0;
      if (*emptyStreamVector == 0)
        file->HasStream = 1;
      else
        file->HasStream = (Byte)((*emptyStreamVector)[i] ? 0 : 1);
      if (file->HasStream)
      {
        file->IsDir = 0;
        file->Size = (*unpackSizes)[sizeIndex];
        file->Crc = (*digests)[sizeIndex];
        file->CrcDefined = (Byte)(*digestsDefined)[sizeIndex];
        sizeIndex++;
      }
      else
      {
        if (*emptyFileVector == 0)
          file->IsDir = 1;
        else
          file->IsDir = (Byte)((*emptyFileVector)[emptyFileIndex] ? 0 : 1);
        emptyFileIndex++;
        file->Size = 0;
        file->Crc = 0;
        file->CrcDefined = 0;
      }
    }
  }
  return SzArEx_Fill(p);
}

static SRes SzReadHeader(
    CSzArEx *p,
    CSzData *sd)
{
  UInt64 *unpackSizes = 0;
  Byte *digestsDefined = 0;
  UInt32 *digests = 0;
  Byte *emptyStreamVector = 0;
  Byte *emptyFileVector = 0;
  Byte *lwtVector = 0;
  SRes res = SzReadHeader2(p, sd,
      &unpackSizes, &digestsDefined, &digests,
      &emptyStreamVector, &emptyFileVector, &lwtVector);
  SzFree(unpackSizes);
  SzFree(digestsDefined);
  SzFree(digests);
  SzFree(emptyStreamVector);
  SzFree(emptyFileVector);
  SzFree(lwtVector);
  return res;
}

static SRes SzReadAndDecodePackedStreams2(
    CLookToRead *inStream,
    CSzData *sd,
    CBuf *outBuffer,
    UInt64 baseOffset,
    CSzAr *p,
    UInt64 **unpackSizes,
    Byte **digestsDefined,
    UInt32 **digests)
{

  UInt32 numUnpackStreams = 0;
  UInt64 dataStartPos;
  CSzFolder *folder;
  UInt64 unpackSize;
  SRes res;

  RINOK(SzReadStreamsInfo(sd, &dataStartPos, p,
      &numUnpackStreams,  unpackSizes, digestsDefined, digests));

  dataStartPos += baseOffset;
  if (p->NumFolders != 1)
    return SZ_ERROR_ARCHIVE;

  folder = p->Folders;
  unpackSize = SzFolder_GetUnpackSize(folder);

#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEKN 4\n");
#endif
  RINOK(LookInStream_SeekTo(inStream, dataStartPos));

  if (!Buf_Create(outBuffer, (size_t)unpackSize))
    return SZ_ERROR_MEM;

  res = SzFolder_Decode(folder, p->PackSizes,
          inStream, dataStartPos,
          outBuffer->data, (size_t)unpackSize);
  RINOK(res);
  if (folder->UnpackCRCDefined)
    if (CrcCalc(outBuffer->data, (size_t)unpackSize) != folder->UnpackCRC)
      return SZ_ERROR_CRC;
  return SZ_OK;
}

static SRes SzReadAndDecodePackedStreams(
    CLookToRead *inStream,
    CSzData *sd,
    CBuf *outBuffer,
    UInt64 baseOffset)
{
  CSzAr p;
  UInt64 *unpackSizes = 0;
  Byte *digestsDefined = 0;
  UInt32 *digests = 0;
  SRes res;
  SzAr_Init(&p);
  res = SzReadAndDecodePackedStreams2(inStream, sd, outBuffer, baseOffset,
    &p, &unpackSizes, &digestsDefined, &digests);
  SzAr_Free(&p);
  SzFree(unpackSizes);
  SzFree(digestsDefined);
  SzFree(digests);
  return res;
}

/* TODO(pts): Make this fast. */
static Int64 FindStartArcPos(CLookToRead *inStream, Byte **buf_out) {
  Int64 ofs = k7zStartHeaderSize;
  Byte *buf, *p, *pend;
  size_t size;
  /* Find k7zSignature in the beginning. */
  while (ofs < (2 << 20)) {  /* 2 MB. */
    size = LookToRead_BUF_SIZE;
    if (LookToRead_Look_Exact(inStream, (const void**)&buf, &size) != SZ_OK ||
        size < k7zStartHeaderSize) {
      break;
    }
    size -= k7zStartHeaderSize - 1;
    for (p = buf, pend = buf + size;
         p != pend && !IS_7Z_SIGNATURE(p);
         ++p) {}
    if (p != pend) {
      *buf_out = p + k7zSignatureSize;
      return ofs + (p - buf);
    }
    LOOKTOREAD_SKIP(inStream, size);  /* No need for error checking. */
    ofs += size;
  }
  return 0;
}

static SRes SzArEx_Open2(
    CSzArEx *p,
    CLookToRead *inStream)
{
  Int64 startArcPos;
  UInt64 nextHeaderOffset, nextHeaderSize;
  size_t nextHeadersize_t;
  UInt32 nextHeaderCRC;
  SRes res;
  Byte *buf = 0;
  size_t size;
  CBuf buffer;

  startArcPos = FindStartArcPos(inStream, &buf);
  if (startArcPos == 0) return SZ_ERROR_NO_ARCHIVE;
  if (buf[0] != k7zMajorVersion) return SZ_ERROR_UNSUPPORTED;

  nextHeaderOffset = GetUi64(buf + 6);
  nextHeaderSize = GetUi64(buf + 14);
  nextHeaderCRC = GetUi32(buf + 22);

  p->startPosAfterHeader = startArcPos;

  if (CrcCalc(buf + 6, 20) != GetUi32(buf + 2))
    return SZ_ERROR_CRC;

  nextHeadersize_t = (size_t)nextHeaderSize;
  if (nextHeadersize_t != nextHeaderSize)
    return SZ_ERROR_MEM;
  if (nextHeadersize_t == 0)
    return SZ_OK;
  if (nextHeaderOffset > nextHeaderOffset + startArcPos)  /* Check for overflow. */
    return SZ_ERROR_NO_ARCHIVE;

  /* If the file is not long enough, we want to return SZ_ERROR_INPUT_EOF, which LookInStream_Read will do for us, so there is no need to check the file size here. */
  /* This is a real seek, actually changing the file offset. */
#ifdef _SZ_SEEK_DEBUG
  fprintf(stderr, "SEEKN 1\n");
#endif
  RINOK(LookInStream_SeekTo(inStream, startArcPos + nextHeaderOffset));


#ifdef _SZ_HEADER_DEBUG
  /* Typically only 36..39 bytes */
  fprintf(stderr, "HEADER read_next size=%ld\n", (long)nextHeadersize_t);
#endif
  size = nextHeadersize_t;
  if (!Buf_Create(&buffer, nextHeadersize_t)) return SZ_ERROR_MEM;
  /* We need a loop here (implemented by LookToRead_ReadAll) to read
   * nextHeadersize_t bytes to buffer.data, because LookToRead_Look_Exact is
   * able to read about 16 kB at once, and we may ned 500 kB or more for the
   * archive header.
   */
  res = LookToRead_ReadAll(inStream, buffer.data, &size);
  if (res == SZ_OK) {
    res = SZ_ERROR_ARCHIVE;
    if (CrcCalc(buffer.data, nextHeadersize_t) == nextHeaderCRC)
    {
      CSzData sd;
      UInt64 type;
      sd.Data = buffer.data;
      sd.Size = buffer.size;
      res = SzReadID(&sd, &type);
      if (res == SZ_OK)
      {
        if (type == k7zIdEncodedHeader)
        {
          CBuf outBuffer;
          Buf_Init(&outBuffer);
#ifdef _SZ_HEADER_DEBUG
          /* Typically happens. */
          fprintf(stderr, "HEADER found_encoded_header\n");
#endif
          res = SzReadAndDecodePackedStreams(inStream, &sd, &outBuffer, p->startPosAfterHeader);
          if (res != SZ_OK)
            Buf_Free(&outBuffer);
          else
          {
            Buf_Free(&buffer);
            buffer.data = outBuffer.data;
            buffer.size = outBuffer.size;
#ifdef _SZ_HEADER_DEBUG
            /* Typical value: 521688 bytes */
            fprintf(stderr, "HEADER encoded_header unpacked size=%ld\n", (long)outBuffer.size);
#endif
            sd.Data = buffer.data;
            sd.Size = buffer.size;
            res = SzReadID(&sd, &type);
          }
        }
      }
      if (res == SZ_OK)
      {
        if (type == k7zIdHeader) {
#ifdef _SZ_HEADER_DEBUG
          fprintf(stderr, "HEADER found_header\n");
#endif
          res = SzReadHeader(p, &sd);
        } else {
          res = SZ_ERROR_UNSUPPORTED;
        }
      }
    }
  }
  Buf_Free(&buffer);
  return res;
}

STATIC SRes SzArEx_Open(CSzArEx *p, CLookToRead *inStream)
{
  SRes res = SzArEx_Open2(p, inStream);
  if (res != SZ_OK)
    SzArEx_Free(p);
  return res;
}

STATIC SRes SzArEx_Extract(
    const CSzArEx *p,
    CLookToRead *inStream,
    UInt32 fileIndex,
    UInt32 *blockIndex,
    Byte **outBuffer,
    size_t *outBufferSize,
    size_t *offset,
    size_t *outSizeProcessed)
{
  UInt32 folderIndex = p->FileIndexToFolderIndexMap[fileIndex];
  SRes res = SZ_OK;
  *offset = 0;
  *outSizeProcessed = 0;
  if (folderIndex == (UInt32)-1)
  {
    SzFree(*outBuffer);
    *blockIndex = folderIndex;
    *outBuffer = 0;
    *outBufferSize = 0;
    return SZ_OK;
  }

  if (*outBuffer == 0 || *blockIndex != folderIndex)
  {
    CSzFolder *folder = p->db.Folders + folderIndex;
    UInt64 unpackSizeSpec = SzFolder_GetUnpackSize(folder);
    size_t unpackSize = (size_t)unpackSizeSpec;
    UInt64 startOffset = SzArEx_GetFolderStreamPos(p, folderIndex, 0);

    if (unpackSize != unpackSizeSpec)
      return SZ_ERROR_MEM;
    *blockIndex = folderIndex;
    SzFree(*outBuffer);
    *outBuffer = 0;

#ifdef _SZ_SEEK_DEBUG
    fprintf(stderr, "SEEKN 5\n");
#endif
    RINOK(LookInStream_SeekTo(inStream, startOffset));

    if (res == SZ_OK)
    {
      *outBufferSize = unpackSize;
      if (unpackSize != 0)
      {
        *outBuffer = (Byte *)SzAlloc(unpackSize);
        if (*outBuffer == 0)
          res = SZ_ERROR_MEM;
      }
      if (res == SZ_OK)
      {
        res = SzFolder_Decode(folder,
          p->db.PackSizes + p->FolderStartPackStreamIndex[folderIndex],
          inStream, startOffset,
          *outBuffer, unpackSize);
        if (res == SZ_OK)
        {
          if (folder->UnpackCRCDefined)
          {
            if (CrcCalc(*outBuffer, unpackSize) != folder->UnpackCRC)
              res = SZ_ERROR_CRC;
          }
        }
      }
    }
  }
  if (res == SZ_OK)
  {
    UInt32 i;
    CSzFileItem *fileItem = p->db.Files + fileIndex;
    *offset = 0;
    for (i = p->FolderStartFileIndex[folderIndex]; i < fileIndex; i++)
      *offset += (UInt32)p->db.Files[i].Size;
    *outSizeProcessed = (size_t)fileItem->Size;
    if (*offset + *outSizeProcessed > *outBufferSize)
      return SZ_ERROR_FAIL;
    if (fileItem->CrcDefined && CrcCalc(*outBuffer + *offset, *outSizeProcessed) != fileItem->Crc)
      res = SZ_ERROR_CRC;
  }
  return res;
}
