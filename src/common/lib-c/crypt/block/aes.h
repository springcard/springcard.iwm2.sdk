#ifndef __CRYPT_BLOCK_AES_H__
#define __CRYPT_BLOCK_AES_H__

#include "lib-c/utils/types.h"

typedef struct
{
  DWORD key_schd[60];  /* Key schedule                          */
  DWORD key_bits;      /* Size of the key (bits)                */
  DWORD rounds;        /* Key-length-dependent number of rounds */
  BOOL  cipher;        /* ? */  
} AES_CTX_ST;

void AES_Init(AES_CTX_ST *aes_ctx, const BYTE *key_data, DWORD key_bits, BOOL cipher);
void AES_Encrypt(AES_CTX_ST *aes_ctx, BYTE data[16]);
void AES_Decrypt(AES_CTX_ST *aes_ctx, BYTE data[16]);

BOOL AES_SeltTest(void);

#define AES_BLOCK_SIZE 16

#endif
