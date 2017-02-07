/* 7zDec.c -- Decoding from 7z folder
2010-11-02 : Igor Pavlov : Public domain */

#include "7zSys.h"

#include "7z.h"
#include "7zAlloc.h"

#include "Bcj2.h"
#include "Bra.h"
#include "CpuArch.h"
#include "LzmaDec.h"
#include "Lzma2Dec.h"

#define k_Copy 0
#define k_LZMA2 0x21
#define k_LZMA  0x30101
#define k_BCJ   0x03030103
#define k_PPC   0x03030205
#define k_ARM   0x03030501
#define k_ARMT  0x03030701
#define k_SPARC 0x03030805
#define k_BCJ2  0x0303011B

static SRes SzDecodeLzma(CSzCoderInfo *coder, UInt64 inSize, CLookToRead *inStream,
    Byte *outBuffer, size_t outSize)
{
  CLzmaDec state;
  SRes res = SZ_OK;

  LzmaDec_Construct(&state);
  RINOK(LzmaDec_AllocateProbs(&state, coder->Props.data, (unsigned)coder->Props.size));
  state.dic = outBuffer;
  state.dicBufSize = outSize;
  LzmaDec_Init(&state);

  for (;;)
  {
    Byte *inBuf = NULL;
    size_t inProcessed = inSize > LookToRead_BUF_SIZE ?
        LookToRead_BUF_SIZE : inSize;
    res = LookToRead_Look(inStream, (const void **)&inBuf, &inProcessed);
    if (res != SZ_OK)
      break;

    {
      size_t dicPos = state.dicPos;
      ELzmaStatus status;
      res = LzmaDec_DecodeToDic(&state, outSize, inBuf, &inProcessed, LZMA_FINISH_END, &status);
      inSize -= inProcessed;
      if (res != SZ_OK)
        break;
      if (state.dicPos == state.dicBufSize || (inProcessed == 0 && dicPos == state.dicPos))
      {
        if (state.dicBufSize != outSize || inSize != 0 ||
            (status != LZMA_STATUS_FINISHED_WITH_MARK &&
             status != LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK))
          res = SZ_ERROR_DATA;
        break;
      }
      LOOKTOREAD_SKIP(inStream, inProcessed);
    }
  }

  LzmaDec_FreeProbs(&state);
  return res;
}

#ifdef USE_LZMA2
static SRes SzDecodeLzma2(CSzCoderInfo *coder, UInt64 inSize, CLookToRead *inStream,
    Byte *outBuffer, size_t outSize)
{
  CLzma2Dec state;
  SRes res = SZ_OK;

  Lzma2Dec_Construct(&state);
  if (coder->Props.size != 1)
    return SZ_ERROR_DATA;
  RINOK(Lzma2Dec_AllocateProbs(&state, coder->Props.data[0]));
  state.decoder.dic = outBuffer;
  state.decoder.dicBufSize = outSize;
  Lzma2Dec_Init(&state);

  for (;;)
  {
    Byte *inBuf = NULL;
    size_t inProcessed = inSize > LookToRead_BUF_SIZE ?
        LookToRead_BUF_SIZE : inSize;
    res = LookToRead_Look(inStream, (const void **)&inBuf, &inProcessed);
    if (res != SZ_OK)
      break;

    {
      size_t dicPos = state.decoder.dicPos;
      ELzmaStatus status;
      res = Lzma2Dec_DecodeToDic(&state, outSize, inBuf, &inProcessed, LZMA_FINISH_END, &status);
      inSize -= inProcessed;
      if (res != SZ_OK)
        break;
      if (state.decoder.dicPos == state.decoder.dicBufSize || (inProcessed == 0 && dicPos == state.decoder.dicPos))
      {
        if (state.decoder.dicBufSize != outSize || inSize != 0 ||
            (status != LZMA_STATUS_FINISHED_WITH_MARK))
          res = SZ_ERROR_DATA;
        break;
      }
      LOOKTOREAD_SKIP(inStream, inProcessed);
    }
  }

  Lzma2Dec_FreeProbs(&state);
  return res;
}
#endif

static Bool IS_MAIN_METHOD(UInt32 m)
{
  switch(m)
  {
    case k_Copy:
    case k_LZMA:
#ifdef USE_LZMA2
    case k_LZMA2:
#endif
      return True;
  }
  return False;
}

static Bool IS_SUPPORTED_CODER(const CSzCoderInfo *c)
{
  return
      c->NumInStreams == 1 &&
      c->NumOutStreams == 1 &&
      c->MethodID <= (UInt32)0xFFFFFFFF &&
      IS_MAIN_METHOD((UInt32)c->MethodID);
}

#define IS_BCJ2(c) ((c)->MethodID == k_BCJ2 && (c)->NumInStreams == 4 && (c)->NumOutStreams == 1)

static SRes CheckSupportedFolder(const CSzFolder *f)
{
  if (f->NumCoders < 1 || f->NumCoders > 4)
    return SZ_ERROR_UNSUPPORTED;
  if (!IS_SUPPORTED_CODER(&f->Coders[0]))
    return SZ_ERROR_UNSUPPORTED;
  if (f->NumCoders == 1)
  {
    if (f->NumPackStreams != 1 || f->PackStreams[0] != 0 || f->NumBindPairs != 0)
      return SZ_ERROR_UNSUPPORTED;
    return SZ_OK;
  }
  if (f->NumCoders == 2)
  {
    CSzCoderInfo *c = &f->Coders[1];
    if (c->MethodID > (UInt32)0xFFFFFFFF ||
        c->NumInStreams != 1 ||
        c->NumOutStreams != 1 ||
        f->NumPackStreams != 1 ||
        f->PackStreams[0] != 0 ||
        f->NumBindPairs != 1 ||
        f->BindPairs[0].InIndex != 1 ||
        f->BindPairs[0].OutIndex != 0)
      return SZ_ERROR_UNSUPPORTED;
    switch ((UInt32)c->MethodID)
    {
      case k_BCJ:
      case k_ARM:
        break;
      default:
        return SZ_ERROR_UNSUPPORTED;
    }
    return SZ_OK;
  }
  if (f->NumCoders == 4)
  {
    if (!IS_SUPPORTED_CODER(&f->Coders[1]) ||
        !IS_SUPPORTED_CODER(&f->Coders[2]) ||
        !IS_BCJ2(&f->Coders[3]))
      return SZ_ERROR_UNSUPPORTED;
    if (f->NumPackStreams != 4 ||
        f->PackStreams[0] != 2 ||
        f->PackStreams[1] != 6 ||
        f->PackStreams[2] != 1 ||
        f->PackStreams[3] != 0 ||
        f->NumBindPairs != 3 ||
        f->BindPairs[0].InIndex != 5 || f->BindPairs[0].OutIndex != 0 ||
        f->BindPairs[1].InIndex != 4 || f->BindPairs[1].OutIndex != 1 ||
        f->BindPairs[2].InIndex != 3 || f->BindPairs[2].OutIndex != 2)
      return SZ_ERROR_UNSUPPORTED;
    return SZ_OK;
  }
  return SZ_ERROR_UNSUPPORTED;
}

static UInt64 GetSum(const UInt64 *values, UInt32 index)
{
  UInt64 sum = 0;
  UInt32 i;
  for (i = 0; i < index; i++)
    sum += values[i];
  return sum;
}

#define CASE_BRA_CONV(isa) case k_ ## isa: isa ## _Convert(outBuffer, outSize, 0, 0); break;

static SRes SzFolder_Decode2(const CSzFolder *folder, const UInt64 *packSizes,
    CLookToRead *inStream, UInt64 startPos,
    Byte *outBuffer, size_t outSize,
    Byte *tempBuf[])
{
  UInt32 ci;
  size_t tempSizes[3] = { 0, 0, 0};
  size_t tempSize3 = 0;
  Byte *tempBuf3 = 0;

  RINOK(CheckSupportedFolder(folder));

  for (ci = 0; ci < folder->NumCoders; ci++)
  {
    CSzCoderInfo *coder = &folder->Coders[ci];

    if (IS_MAIN_METHOD((UInt32)coder->MethodID))
    {
      UInt32 si = 0;
      UInt64 offset;
      UInt64 inSize;
      Byte *outBufCur = outBuffer;
      size_t outSizeCur = outSize;
      if (folder->NumCoders == 4)
      {
        UInt32 indices[] = { 3, 2, 0 };
        UInt64 unpackSize = folder->UnpackSizes[ci];
        si = indices[ci];
        if (ci < 2)
        {
          Byte *temp;
          outSizeCur = (size_t)unpackSize;
          if (outSizeCur != unpackSize)
            return SZ_ERROR_MEM;
          temp = (Byte *)SzAlloc(outSizeCur);
          if (temp == 0 && outSizeCur != 0)
            return SZ_ERROR_MEM;
          outBufCur = tempBuf[1 - ci] = temp;
          tempSizes[1 - ci] = outSizeCur;
        }
        else if (ci == 2)
        {
          if (unpackSize > outSize) /* check it */
            return SZ_ERROR_PARAM;
          tempBuf3 = outBufCur = outBuffer + (outSize - (size_t)unpackSize);
          tempSize3 = outSizeCur = (size_t)unpackSize;
        }
        else
          return SZ_ERROR_UNSUPPORTED;
      }
      offset = GetSum(packSizes, si);
      inSize = packSizes[si];
#ifdef _SZ_SEEK_DEBUG
      fprintf(stderr, "SEEKN 2\n");
#endif
      RINOK(LookInStream_SeekTo(inStream, startPos + offset));

      if (coder->MethodID == k_Copy)
      {
        size_t size = inSize;
#ifdef _SZ_CODER_DEBUG
      fprintf(stderr, "CODER Copy\n");
#endif
        if (inSize != outSizeCur) /* check it */
          return SZ_ERROR_DATA;
        if (inSize != size)
          return SZ_ERROR_MEM;
        RINOK(LookToRead_ReadAll(inStream, outBufCur, &size));
      }
      else if (coder->MethodID == k_LZMA)
      {
#ifdef _SZ_CODER_DEBUG
      fprintf(stderr, "CODER LZMA\n");
#endif
        RINOK(SzDecodeLzma(coder, inSize, inStream, outBufCur, outSizeCur));
      }
#ifdef USE_LZMA2
      else if (coder->MethodID == k_LZMA2)
      {
#ifdef _SZ_CODER_DEBUG
      fprintf(stderr, "CODER LZMA2\n");
#endif
        RINOK(SzDecodeLzma2(coder, inSize, inStream, outBufCur, outSizeCur));
      }
#endif
      else
      {
        return SZ_ERROR_UNSUPPORTED;
      }
    }
    else if (coder->MethodID == k_BCJ2)
    {
      UInt64 offset = GetSum(packSizes, 1);
      UInt64 s3Size = packSizes[1];
      size_t size;
#ifdef _SZ_CODER_DEBUG
      fprintf(stderr, "CODER BCJ2\n");
#endif
      if (ci != 3)
        return SZ_ERROR_UNSUPPORTED;
#ifdef _SZ_SEEK_DEBUG
      fprintf(stderr, "SEEKN 3\n");
#endif
      RINOK(LookInStream_SeekTo(inStream, startPos + offset));
      tempSizes[2] = size = (size_t)s3Size;
      if (size != s3Size)
        return SZ_ERROR_MEM;
      tempBuf[2] = (Byte *)SzAlloc(tempSizes[2]);
      if (tempBuf[2] == 0 && tempSizes[2] != 0)
        return SZ_ERROR_MEM;
      RINOK(LookToRead_ReadAll(inStream, tempBuf[2], &size));
      RINOK(Bcj2_Decode(
          tempBuf3, tempSize3,
          tempBuf[0], tempSizes[0],
          tempBuf[1], tempSizes[1],
          tempBuf[2], tempSizes[2],
          outBuffer, outSize));
    }
    else
    {
      if (ci != 1)
        return SZ_ERROR_UNSUPPORTED;
      switch(coder->MethodID)
      {
        case k_BCJ:
        {
          UInt32 state;
#ifdef _SZ_CODER_DEBUG
          fprintf(stderr, "CODER BCJ\n");
#endif
          x86_Convert_Init(state);
          x86_Convert(outBuffer, outSize, 0, &state, 0);
          break;
        }
        CASE_BRA_CONV(ARM)
        default:
          return SZ_ERROR_UNSUPPORTED;
      }
    }
  }
  return SZ_OK;
}

STATIC SRes SzFolder_Decode(const CSzFolder *folder, const UInt64 *packSizes,
    CLookToRead *inStream, UInt64 startPos,
    Byte *outBuffer, size_t outSize)
{
  Byte *tempBuf[3] = { 0, 0, 0};
  int i;
  SRes res = SzFolder_Decode2(folder, packSizes, inStream, startPos,
      outBuffer, (size_t)outSize, tempBuf);
  for (i = 0; i < 3; i++)
    SzFree(tempBuf[i]);
  return res;
}
