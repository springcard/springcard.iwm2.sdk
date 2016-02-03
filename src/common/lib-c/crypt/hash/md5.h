#ifndef __CRYPT_HASH_MD5_H__
#define __CRYPT_HASH_MD5_H__

#include "lib-c/utils/types.h"

/*
 * MD5
 * ---
 */
typedef struct
{
  DWORD state[4];        /* Current state (ABCD) */
  DWORD count[2];        /* Number of bits, modulo 2^64 (LSB first) */
  BYTE  buffer[64];      /* Input buffer */
} MD5_CTX_ST;

void MD5_Init(MD5_CTX_ST *md5_ctx);
void MD5_Update(MD5_CTX_ST *md5_ctx, const BYTE *input, DWORD size);
void MD5_Final(MD5_CTX_ST *md5_ctx, BYTE digest[16]);

BOOL MD5_SelfTest(void);

#endif
