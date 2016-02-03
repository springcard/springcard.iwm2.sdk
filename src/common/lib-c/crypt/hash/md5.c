#include "md5.h"

/*
 ****************************************************************************
 *
 *  MD5
 *
 ****************************************************************************
 *
 * This is a derivative work from original RSA implementation (see RSA's
 * copyright below).
 *
 * The main difference lies in final padding, where a static buffer holding
 * padding bytes has been replaced by 'on-the-fly' padding. This reduces
 * the RAM footprint. The ROM footprint is nearly 6,5kB on Renesas R8C .
 * Macros have been prefixed with MD5_ to prevent collision with other
 * defines used in some of our projects.
 *
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 */

/* MD5 internal functions */
/* ---------------------- */
static void MD5_transform(DWORD state[4], const BYTE block[64]);
static void MD5_arr_dw2b(BYTE *output, DWORD *input, DWORD len);
static void MD5_arr_b2dw(DWORD *output, const BYTE *input, DWORD len);

/* MD5 initialization */
/* ------------------ */
void MD5_Init(MD5_CTX_ST *ctx)
{
  /* Current size is 0 */
  ctx->count[0] = ctx->count[1] = 0;
  /* Initial values for state */
  ctx->state[0] = 0x67452301;
  ctx->state[1] = 0xefcdab89;
  ctx->state[2] = 0x98badcfe;
  ctx->state[3] = 0x10325476;
}

/* MD5 block update operation */
/* -------------------------- */
void MD5_Update(MD5_CTX_ST *ctx, const BYTE *input, DWORD inputLen)
{
  DWORD i, index, partLen;

  /* Compute number of bytes mod 64 */
  index = (DWORD)((ctx->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((ctx->count[0] += ((DWORD) inputLen << 3)) < ((DWORD)inputLen << 3))
    ctx->count[1]++;
  ctx->count[1] += ((DWORD) inputLen >> 29);
  
  partLen = 64 - index;
  
  /* Transform as many times as possible */
  if (inputLen >= partLen)
  {
    if (input != NULL)
    {
      memcpy(&ctx->buffer[index], input, partLen);
    } else
    {
      memset(&ctx->buffer[index], 0x00,  partLen);
      if (partLen)
        ctx->buffer[index] = 0x80;
    }

    MD5_transform(ctx->state, ctx->buffer);
 
    for (i = partLen; i + 63 < inputLen; i += 64)
    {
      if (input != NULL)
        MD5_transform(ctx->state, &input[i]);
      else
        MD5_transform(ctx->state, NULL);
    }
    
    index = 0;

  } else
    i = 0;

  /* Store remaining input in buffer */
  if (input != NULL)
  {
    memcpy(&ctx->buffer[index], &input[i], inputLen-i);
  } else
  {
    memset(&ctx->buffer[index], 0        , inputLen-i);
    if ((i == 0) && inputLen)
      ctx->buffer[index] = 0x80;
  }
}

/* MD5 finalization */
/* ---------------- */
void MD5_Final(MD5_CTX_ST *ctx, BYTE digest[16])
{
  BYTE nbits[8];
  DWORD index, padLen;

  /* Save number of bits */
  MD5_arr_dw2b(nbits, ctx->count, 8);

  /* Pad out to 56 mod 64 */
  index = (DWORD)((ctx->count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  MD5_Update(ctx, NULL, padLen);
  
  /* Append length (before padding) */
  MD5_Update(ctx, nbits, 8);

  /* Store state in digest */
  MD5_arr_dw2b(digest, ctx->state, 16);
}


/*
 * The MD5 transformation core
 * ---------------------------
 */

/* Constants for the MD5 transformation */
#define MD5_S11 7
#define MD5_S12 12
#define MD5_S13 17
#define MD5_S14 22
#define MD5_S21 5
#define MD5_S22 9
#define MD5_S23 14
#define MD5_S24 20
#define MD5_S31 4
#define MD5_S32 11
#define MD5_S33 16
#define MD5_S34 23
#define MD5_S41 6
#define MD5_S42 10
#define MD5_S43 15
#define MD5_S44 21

#if (defined(MD5_SMALL_ROM))
  /* ROTATE_LEFT rotates x left n bits */
  static DWORD ROTATE_LEFT(DWORD x, BYTE n)
  {
    return (((x) << (n)) | ((x) >> (32-(n))));
  }

  /* Basic functions F, G, H and I */
  static DWORD MD5_F(DWORD x, DWORD y, DWORD z)
  {
    return (((x) & (y)) | ((~x) & (z)));
  }
  static DWORD MD5_G(DWORD x, DWORD y, DWORD z)
  {
    return (((x) & (z)) | ((y) & (~z)));
  }
  static DWORD MD5_H(DWORD x, DWORD y, DWORD z)
  {
    return ((x) ^ (y) ^ (z));
  }
  static DWORD MD5_I(DWORD x, DWORD y, DWORD z)
  {
    return ((y) ^ ((x) | (~z)));
  }
#else
  /* ROTATE_LEFT rotates x left n bits */
  #define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

  /* Basic functions F, G, H and I */
  #define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
  #define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
  #define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
  #define MD5_I(x, y, z) ((y) ^ ((x) | (~z)))
#endif

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4
   Rotation is separate from addition to prevent recomputation */
#define MD5_FF(a, b, c, d, x, s, ac) {(a) += MD5_F ((b), (c), (d)) + (x) + (DWORD)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define MD5_GG(a, b, c, d, x, s, ac) {(a) += MD5_G ((b), (c), (d)) + (x) + (DWORD)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define MD5_HH(a, b, c, d, x, s, ac) {(a) += MD5_H ((b), (c), (d)) + (x) + (DWORD)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }
#define MD5_II(a, b, c, d, x, s, ac) {(a) += MD5_I ((b), (c), (d)) + (x) + (DWORD)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); }

/* MD5 basic transformation. Transforms state based on block */
static void MD5_transform(DWORD state[4], const BYTE block[64])
{
  DWORD a = state[0], b = state[1], c = state[2], d = state[3], x[16];

  if (block == NULL)  
    memset(x, 0x00, sizeof(x));
  else
    MD5_arr_b2dw(x, block, 64);

  /* Round 1 */
  MD5_FF (a, b, c, d, x[ 0], MD5_S11, 0xd76aa478); /* 1 */
  MD5_FF (d, a, b, c, x[ 1], MD5_S12, 0xe8c7b756); /* 2 */
  MD5_FF (c, d, a, b, x[ 2], MD5_S13, 0x242070db); /* 3 */
  MD5_FF (b, c, d, a, x[ 3], MD5_S14, 0xc1bdceee); /* 4 */
  MD5_FF (a, b, c, d, x[ 4], MD5_S11, 0xf57c0faf); /* 5 */
  MD5_FF (d, a, b, c, x[ 5], MD5_S12, 0x4787c62a); /* 6 */
  MD5_FF (c, d, a, b, x[ 6], MD5_S13, 0xa8304613); /* 7 */
  MD5_FF (b, c, d, a, x[ 7], MD5_S14, 0xfd469501); /* 8 */
  MD5_FF (a, b, c, d, x[ 8], MD5_S11, 0x698098d8); /* 9 */
  MD5_FF (d, a, b, c, x[ 9], MD5_S12, 0x8b44f7af); /* 10 */
  MD5_FF (c, d, a, b, x[10], MD5_S13, 0xffff5bb1); /* 11 */
  MD5_FF (b, c, d, a, x[11], MD5_S14, 0x895cd7be); /* 12 */
  MD5_FF (a, b, c, d, x[12], MD5_S11, 0x6b901122); /* 13 */
  MD5_FF (d, a, b, c, x[13], MD5_S12, 0xfd987193); /* 14 */
  MD5_FF (c, d, a, b, x[14], MD5_S13, 0xa679438e); /* 15 */
  MD5_FF (b, c, d, a, x[15], MD5_S14, 0x49b40821); /* 16 */

  /* Round 2 */
  MD5_GG (a, b, c, d, x[ 1], MD5_S21, 0xf61e2562); /* 17 */
  MD5_GG (d, a, b, c, x[ 6], MD5_S22, 0xc040b340); /* 18 */
  MD5_GG (c, d, a, b, x[11], MD5_S23, 0x265e5a51); /* 19 */
  MD5_GG (b, c, d, a, x[ 0], MD5_S24, 0xe9b6c7aa); /* 20 */
  MD5_GG (a, b, c, d, x[ 5], MD5_S21, 0xd62f105d); /* 21 */
  MD5_GG (d, a, b, c, x[10], MD5_S22,  0x2441453); /* 22 */
  MD5_GG (c, d, a, b, x[15], MD5_S23, 0xd8a1e681); /* 23 */
  MD5_GG (b, c, d, a, x[ 4], MD5_S24, 0xe7d3fbc8); /* 24 */
  MD5_GG (a, b, c, d, x[ 9], MD5_S21, 0x21e1cde6); /* 25 */
  MD5_GG (d, a, b, c, x[14], MD5_S22, 0xc33707d6); /* 26 */
  MD5_GG (c, d, a, b, x[ 3], MD5_S23, 0xf4d50d87); /* 27 */
  MD5_GG (b, c, d, a, x[ 8], MD5_S24, 0x455a14ed); /* 28 */
  MD5_GG (a, b, c, d, x[13], MD5_S21, 0xa9e3e905); /* 29 */
  MD5_GG (d, a, b, c, x[ 2], MD5_S22, 0xfcefa3f8); /* 30 */
  MD5_GG (c, d, a, b, x[ 7], MD5_S23, 0x676f02d9); /* 31 */
  MD5_GG (b, c, d, a, x[12], MD5_S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  MD5_HH (a, b, c, d, x[ 5], MD5_S31, 0xfffa3942); /* 33 */
  MD5_HH (d, a, b, c, x[ 8], MD5_S32, 0x8771f681); /* 34 */
  MD5_HH (c, d, a, b, x[11], MD5_S33, 0x6d9d6122); /* 35 */
  MD5_HH (b, c, d, a, x[14], MD5_S34, 0xfde5380c); /* 36 */
  MD5_HH (a, b, c, d, x[ 1], MD5_S31, 0xa4beea44); /* 37 */
  MD5_HH (d, a, b, c, x[ 4], MD5_S32, 0x4bdecfa9); /* 38 */
  MD5_HH (c, d, a, b, x[ 7], MD5_S33, 0xf6bb4b60); /* 39 */
  MD5_HH (b, c, d, a, x[10], MD5_S34, 0xbebfbc70); /* 40 */
  MD5_HH (a, b, c, d, x[13], MD5_S31, 0x289b7ec6); /* 41 */
  MD5_HH (d, a, b, c, x[ 0], MD5_S32, 0xeaa127fa); /* 42 */
  MD5_HH (c, d, a, b, x[ 3], MD5_S33, 0xd4ef3085); /* 43 */
  MD5_HH (b, c, d, a, x[ 6], MD5_S34,  0x4881d05); /* 44 */
  MD5_HH (a, b, c, d, x[ 9], MD5_S31, 0xd9d4d039); /* 45 */
  MD5_HH (d, a, b, c, x[12], MD5_S32, 0xe6db99e5); /* 46 */
  MD5_HH (c, d, a, b, x[15], MD5_S33, 0x1fa27cf8); /* 47 */
  MD5_HH (b, c, d, a, x[ 2], MD5_S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  MD5_II (a, b, c, d, x[ 0], MD5_S41, 0xf4292244); /* 49 */
  MD5_II (d, a, b, c, x[ 7], MD5_S42, 0x432aff97); /* 50 */
  MD5_II (c, d, a, b, x[14], MD5_S43, 0xab9423a7); /* 51 */
  MD5_II (b, c, d, a, x[ 5], MD5_S44, 0xfc93a039); /* 52 */
  MD5_II (a, b, c, d, x[12], MD5_S41, 0x655b59c3); /* 53 */
  MD5_II (d, a, b, c, x[ 3], MD5_S42, 0x8f0ccc92); /* 54 */
  MD5_II (c, d, a, b, x[10], MD5_S43, 0xffeff47d); /* 55 */
  MD5_II (b, c, d, a, x[ 1], MD5_S44, 0x85845dd1); /* 56 */
  MD5_II (a, b, c, d, x[ 8], MD5_S41, 0x6fa87e4f); /* 57 */
  MD5_II (d, a, b, c, x[15], MD5_S42, 0xfe2ce6e0); /* 58 */
  MD5_II (c, d, a, b, x[ 6], MD5_S43, 0xa3014314); /* 59 */
  MD5_II (b, c, d, a, x[13], MD5_S44, 0x4e0811a1); /* 60 */
  MD5_II (a, b, c, d, x[ 4], MD5_S41, 0xf7537e82); /* 61 */
  MD5_II (d, a, b, c, x[11], MD5_S42, 0xbd3af235); /* 62 */
  MD5_II (c, d, a, b, x[ 2], MD5_S43, 0x2ad7d2bb); /* 63 */
  MD5_II (b, c, d, a, x[ 9], MD5_S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  
  /* Zeroize sensitive information */
  memset (x, 0xCC, sizeof (x));
}

/* Encodes input (DWORD) into output (BYTE). Assumes len is a multiple of 4 */
static void MD5_arr_dw2b (BYTE *output, DWORD *input, DWORD len)
{
  DWORD i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
    output[j] = (BYTE)(input[i] & 0xff);
    output[j+1] = (BYTE)((input[i] >> 8) & 0xff);
    output[j+2] = (BYTE)((input[i] >> 16) & 0xff);
    output[j+3] = (BYTE)((input[i] >> 24) & 0xff);
  }
}

/* Decodes input (BYTE) into output (DWORD). Assumes len is a multiple of 4 */
static void MD5_arr_b2dw (DWORD *output, const BYTE *input, DWORD len)
{
  DWORD i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
    output[i] = ((DWORD)input[j]) | (((DWORD)input[j+1]) << 8) |
      (((DWORD)input[j+2]) << 16) | (((DWORD)input[j+3]) << 24);
}

#ifdef _DEBUG

/*
 *****************************************************************************
 *
 *                                 VALIDATION
 *
 *****************************************************************************
 */

#include <stdio.h>

static BYTE c2b(char c)
{
	if (c >= '0' && c <= '9')
		return (BYTE) (c - '0');
	if (c >= 'a' && c <= 'f')
		return (BYTE)(c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (BYTE)(c - 'A' + 10);
	return 0;
}

static DWORD s2b(BYTE dst[], const char *src)
{
  DWORD i;
	DWORD l = strlen(src) / 2;

	for (i=0; i<l; i++)
		dst[i] = (BYTE) ((c2b(src[2*i])<<4) + c2b(src[2*i+1]));

	return l;
}

BOOL MD5_Test(char *text, char *hash)
{
  MD5_CTX_ST context;
  BYTE digest[16], expect[16];
  DWORD i;
  
  MD5_Init(&context);
  if (text != NULL)
  {
    MD5_Update(&context, (const BYTE *) text, strlen(text));
  } else
  {
    memset(expect, 'a', 16);
    for (i=0; i<1000000; i+=10)
      MD5_Update(&context, expect, 10);
  }
  MD5_Final(&context, digest);

  s2b(expect, hash);
  if (memcmp(expect, digest, 16))
  {
    printf("MD5 SelfTest failed\n");
    if (text != NULL)
      printf("Vector   : \"%s\"\n", text);
    else
      printf("Vector   : 1 million times 'a'\n");
    printf("Hash     : ");
    for (i=0; i<16; i++)
      printf("%02X", digest[i]);
    printf("Expected : %s\n", hash);
    return FALSE;
  }
  return TRUE;
}

BOOL MD5_SelfTest(void)
{
  if (!MD5_Test("", "D41D8CD98F00B204E9800998ECF8427E")) return FALSE;
  if (!MD5_Test("a", "0CC175B9C0F1B6A831C399E269772661")) return FALSE;
  if (!MD5_Test("abc", "900150983CD24FB0D6963F7D28E17F72")) return FALSE;
  if (!MD5_Test("message digest", "F96B697D7CB7938D525A2F31AAF161D0")) return FALSE;
  if (!MD5_Test("abcdefghijklmnopqrstuvwxyz", "C3FCD3D76192E4007DFB496CCA67E13B")) return FALSE;
  if (!MD5_Test("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", "8215EF0796A20BCAAAE116D3876C664A")) return FALSE;
  if (!MD5_Test("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "D174AB98D277D9F5A5611C2C9F419D9F")) return FALSE;
  if (!MD5_Test("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57EDF4A22BE3C955AC49DA2E2107B67A")) return FALSE;
  if (!MD5_Test(NULL, "7707D6AE4E027C70EEA2A935C2296F21")) return FALSE;
  return TRUE;
}

#endif
