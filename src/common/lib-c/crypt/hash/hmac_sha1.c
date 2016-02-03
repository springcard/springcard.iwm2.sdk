#include "hmac_sha1.h"

#define HMAC_DIGEST_SZ 20
#define HMAC_BLOCK_SZ  64
#define HMAC_IPAD      0x36
#define HMAC_OPAD      0x5C

void HMAC_SHA1_Init(SHA1_CTX_ST *ctx, const BYTE key[], size_t key_size)
{
  BYTE ipad[HMAC_BLOCK_SZ];
  size_t i;

  memset(ipad, HMAC_IPAD, HMAC_BLOCK_SZ);

  if (key_size > HMAC_BLOCK_SZ)
  {
    BYTE ikey[HMAC_DIGEST_SZ];  
    SHA1_CTX_ST key_ctx;

    SHA1_Init(&key_ctx);
    SHA1_Update(&key_ctx, key, key_size);
    SHA1_Final(&key_ctx, ikey);

    for (i=0; i<HMAC_DIGEST_SZ; i++)
      ipad[i] ^= ikey[i];

  } else
  {
    for (i=0; i<key_size; i++)
      ipad[i] ^= key[i];
  }

  SHA1_Init(ctx);
  SHA1_Update(ctx, ipad, HMAC_BLOCK_SZ);
}

void HMAC_SHA1_Final(SHA1_CTX_ST *ctx, BYTE digest[HMAC_DIGEST_SZ], const BYTE key[], size_t key_size)
{
  BYTE opad[HMAC_BLOCK_SZ];
  size_t i;

  SHA1_Final(ctx, digest);

  memset(opad, HMAC_OPAD, HMAC_BLOCK_SZ);

  if (key_size > HMAC_BLOCK_SZ)
  {
    BYTE okey[HMAC_DIGEST_SZ];  
    SHA1_CTX_ST key_ctx;

    SHA1_Init(&key_ctx);
    SHA1_Update(&key_ctx, key, key_size);
    SHA1_Final(&key_ctx, okey);

    for (i=0; i<HMAC_DIGEST_SZ; i++)
      opad[i] ^= okey[i];

  } else
  {
    for (i=0; i<key_size; i++)
      opad[i] ^= key[i];
  }

  SHA1_Init(ctx);
  SHA1_Update(ctx, opad, HMAC_BLOCK_SZ);
  SHA1_Update(ctx, digest, HMAC_DIGEST_SZ);
  SHA1_Final(ctx, digest);
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

static size_t s2b(BYTE dst[], const char *src)
{
  size_t i;
	size_t l = strlen(src) / 2;

	for (i=0; i<l; i++)
		dst[i] = (BYTE) ((c2b(src[2*i])<<4) + c2b(src[2*i+1]));

	return l;
}

BOOL HMAC_SHA1_TestEx(const BYTE text[], size_t text_len, const BYTE key[], size_t key_len, const BYTE expect[HMAC_DIGEST_SZ])
{
  SHA1_CTX_ST context;
  BYTE digest[HMAC_DIGEST_SZ];
  size_t i;

  HMAC_SHA1_Init(&context, key, key_len);
  if (text != NULL)
  {
    SHA1_Update(&context, text, text_len);
  } else
  {
    char *t = "aaaaaaaaaa";
    for (i=0; i<1000000; i+=10)
      SHA1_Update(&context, t, 10);
  }
  HMAC_SHA1_Final(&context, digest, key, key_len);

  if (memcmp(expect, digest, HMAC_DIGEST_SZ))
  {
    printf("HMAC SHA1 SelfTest failed\n");

    printf("Hash       : ");
    for (i=0; i<HMAC_DIGEST_SZ; i++)
      printf("%02X", digest[i]);
    printf("\n");
    printf("Instead of : ");
    for (i=0; i<HMAC_DIGEST_SZ; i++)
      printf("%02X", expect[i]);
    printf("\n");

    printf("Text       : ");
    if (text != NULL)
    {
      for (i=0; i<text_len; i++)
        printf("%02X", text[i]);
    } else
    {
      printf("1 million times 'a'");
    }
    printf("\n");

    printf("Key        : ");
    for (i=0; i<key_len; i++)
      printf("%02X", key[i]);
    printf("\n");

    return FALSE;
  }

  return TRUE;
}

BOOL HMAC_SHA1_Test(const void *text, BOOL text_asis, const void *key, BOOL key_asis, const char *expect)
{
  BOOL    rc;
  BYTE   *b_text = NULL;
  BYTE   *b_key  = NULL;
  BYTE    b_expect[HMAC_DIGEST_SZ];
  size_t  l_text = 0;
  size_t  l_key  = 0;

  if (text_asis)
  {
    b_text = (BYTE *) text;
    if (text != NULL)
      l_text = strlen(text);
  } else
  {
    l_text = strlen(text) / 2;
    b_text = malloc(l_text);
    s2b(b_text, text);   
  }

  if (key_asis)
  {
    b_key = (BYTE *) key;
    l_key = strlen(key);
  } else
  {
    l_key = strlen(key) / 2;
    b_key = malloc(l_key);
    s2b(b_key, key);   
  }

  s2b(b_expect, expect);

  rc = HMAC_SHA1_TestEx(b_text, l_text, b_key, l_key, b_expect);

  if (!key_asis)
    free(b_key);
  if (!text_asis)
    free(b_text);

  return rc;
}

BOOL HMAC_SHA1_SelfTest(void)
{
  const char key1[] = "00112233445566778899AABBCCDDEEFF0123456789ABCDEF00112233445566778899AABBCCDDEEFF0123456789ABCDEF00112233445566778899AABBCCDDEEFF";
  const char key2[] = "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

  /* From RFC2202 */
  /* ------------ */

  /* 1 */
  if (!HMAC_SHA1_Test("Hi There", TRUE,
                     "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b", FALSE,
                     "b617318655057264e28bc0b6fb378c8ef146be00")) return FALSE;

  /* 2 */
  if (!HMAC_SHA1_Test("what do ya want for nothing?", TRUE,
                     "Jefe", TRUE,
                     "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79")) return FALSE;

  /* 3 */
  if (!HMAC_SHA1_Test("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd", FALSE,
                     "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", FALSE,
                     "125d7342b9ac11cd91a39af48aa17b4f63f175d3")) return FALSE;

  /* 4 */
  if (!HMAC_SHA1_Test("cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd", FALSE,
                     "0102030405060708090a0b0c0d0e0f10111213141516171819", FALSE,
                     "4c9007f4026250c6bc8414f9bf50c86c2d7235da")) return FALSE;

  /* 5 */
  if (!HMAC_SHA1_Test("Test With Truncation", TRUE,
                     "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c", FALSE,
                     "4c1a03424b55e07fe7f27be1d58bb9324a9a5a04")) return FALSE;

  /* 6 */
  if (!HMAC_SHA1_Test("Test Using Larger Than Block-Size Key - Hash Key First", TRUE,
                     "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", FALSE,
                     "aa4ae5e15272d00e95705637ce8a3b55ed402112")) return FALSE;

  /* 7 */
  if (!HMAC_SHA1_Test("Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data", TRUE,
                     "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", FALSE,
                     "e8e99d0f45237d786d6bbaa7965c7808bbff1a91")) return FALSE;


  return TRUE;
}

#endif

