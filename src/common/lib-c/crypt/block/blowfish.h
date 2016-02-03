#ifndef __CRYPT_BLOCK_BLOWFISH_H__
#define __CRYPT_BLOCK_BLOWFISH_H__

#include "lib-c/utils/types.h"

typedef struct
{
  DWORD P[16 + 2];
  DWORD S[4][256];
} BLOWFISH_CTX_ST;

void Blowfish_Init(BLOWFISH_CTX_ST *blowfish_ctx, const BYTE *key_data, int key_bits);
void Blowfish_Encrypt(BLOWFISH_CTX_ST *blowfish_ctx, BYTE data[8]);
void Blowfish_Decrypt(BLOWFISH_CTX_ST *blowfish_ctx, BYTE data[8]);
void Blowfish_Encrypt_Ex(BLOWFISH_CTX_ST *blowfish_ctx, DWORD *xl, DWORD *xr);
void Blowfish_Decrypt_Ex(BLOWFISH_CTX_ST *blowfish_ctx, DWORD *xl, DWORD *xr);

BOOL Blowfish_SelfTest(void);

#endif
