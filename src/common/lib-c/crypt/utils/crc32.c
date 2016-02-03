#include "../crypto_i.h"

static DWORD crc32Table[256];
static BOOL  crc32Ready = 0;

static void crc32Init(void)
{
  DWORD crc, poly;
  int   i, j;

  poly = 0xEDB88320L;
  for (i=0; i<256; i++)
  {
    crc = i;
    for (j=8; j>0; j--)
    {
      if (crc&1)
      {
        crc = (crc >> 1) ^ poly;
      } else
      {
        crc >>= 1;
      }
    }
    crc32Table[i] = crc;
  }
  
  crc32Ready = TRUE;
}

DWORD crc32(BYTE *data, DWORD length)
{
  register DWORD crc;
  DWORD i;
 
  if (!crc32Ready)
    crc32Init();

  crc = 0xFFFFFFFF;
  for (i=0; i<length; i++)
    crc = ((crc>>8) & 0x00FFFFFF) ^ crc32Table[ (crc^(data[i])) & 0xFF ];

  return crc^0xFFFFFFFF;
}
