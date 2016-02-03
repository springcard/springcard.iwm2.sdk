#ifndef __CRYPT_HMAC_SHA1_H__
#define __CRYPT_HMAC_SHA1_H__

#include "sha1.h"

void HMAC_SHA1_Init(SHA1_CTX_ST *ctx, const BYTE key[], size_t key_size);
void HMAC_SHA1_Final(SHA1_CTX_ST *ctx, BYTE digest[16], const BYTE key[], size_t key_size);

BOOL HMAC_SHA1_SelfTest(void);

#endif
