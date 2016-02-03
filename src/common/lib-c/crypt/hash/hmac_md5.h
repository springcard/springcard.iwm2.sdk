#ifndef __CRYPT_HMAC_MD5_H__
#define __CRYPT_HMAC_MD5_H__

#include "md5.h"

void HMAC_MD5_Init(MD5_CTX_ST *ctx, const BYTE key[], size_t key_size);
void HMAC_MD5_Final(MD5_CTX_ST *ctx, BYTE digest[16], const BYTE key[], size_t key_size);

BOOL HMAC_MD5_SelfTest(void);

#endif
