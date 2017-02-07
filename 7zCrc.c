/* 7zCrc.c -- CRC32 init
2010-12-01 : Igor Pavlov : Public domain */

#include "7zCrc.h"
#include "CpuArch.h"

#define kCrcPoly 0xEDB88320

/* Based on crc32h in: http://www.hackersdelight.org/hdcodetxt/crc.c.txt */
STATIC UInt32 MY_FAST_CALL CrcCalc(const void *data, size_t size) {
  const UInt32 g0 = kCrcPoly, g1 = g0>>1,
      g2 = g0>>2, g3 = g0>>3, g4 = g0>>4, g5 = g0>>5,
      g6 = (g0>>6)^g0, g7 = ((g0>>6)^g0)>>1;
  register const Byte *p = data;
  register const Byte *pend = p + size;
  register Int32 crc = -1;
  if (p != pend) {
    do {
      crc ^= *p++;
      crc = ((UInt32)crc >> 8) ^
         ((crc<<31>>31) & g7) ^ ((crc<<30>>31) & g6) ^
         ((crc<<29>>31) & g5) ^ ((crc<<28>>31) & g4) ^
         ((crc<<27>>31) & g3) ^ ((crc<<26>>31) & g2) ^
         ((crc<<25>>31) & g1) ^ ((crc<<24>>31) & g0);
    } while (p != pend);
  }
  return ~crc;
}
