#include "hmac_md5.h"

#define HMAC_DIGEST_SZ 16
#define HMAC_BLOCK_SZ  64
#define HMAC_IPAD      0x36
#define HMAC_OPAD      0x5C

void HMAC_MD5_Init(MD5_CTX_ST *ctx, const BYTE key[], size_t key_size)
{
  BYTE ipad[HMAC_BLOCK_SZ];
  size_t i;

  memset(ipad, HMAC_IPAD, HMAC_BLOCK_SZ);

  if (key_size > HMAC_BLOCK_SZ)
  {
    BYTE ikey[HMAC_DIGEST_SZ];  
    MD5_CTX_ST key_ctx;

    MD5_Init(&key_ctx);
    MD5_Update(&key_ctx, key, key_size);
    MD5_Final(&key_ctx, ikey);

    for (i=0; i<HMAC_DIGEST_SZ; i++)
      ipad[i] ^= ikey[i];

  } else
  {
    for (i=0; i<key_size; i++)
      ipad[i] ^= key[i];
  }

  MD5_Init(ctx);
  MD5_Update(ctx, ipad, HMAC_BLOCK_SZ);
}

void HMAC_MD5_Final(MD5_CTX_ST *ctx, BYTE digest[HMAC_DIGEST_SZ], const BYTE key[], size_t key_size)
{
  BYTE opad[HMAC_BLOCK_SZ];
  size_t i;

  MD5_Final(ctx, digest);

  memset(opad, HMAC_OPAD, HMAC_BLOCK_SZ);

  if (key_size > HMAC_BLOCK_SZ)
  {
    BYTE okey[HMAC_DIGEST_SZ];  
    MD5_CTX_ST key_ctx;

    MD5_Init(&key_ctx);
    MD5_Update(&key_ctx, key, key_size);
    MD5_Final(&key_ctx, okey);

    for (i=0; i<HMAC_DIGEST_SZ; i++)
      opad[i] ^= okey[i];

  } else
  {
    for (i=0; i<key_size; i++)
      opad[i] ^= key[i];
  }

  MD5_Init(ctx);
  MD5_Update(ctx, opad, HMAC_BLOCK_SZ);
  MD5_Update(ctx, digest, HMAC_DIGEST_SZ);
  MD5_Final(ctx, digest);
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

BOOL HMAC_MD5_TestEx(const BYTE text[], size_t text_len, const BYTE key[], size_t key_len, const BYTE expect[HMAC_DIGEST_SZ])
{
  MD5_CTX_ST context;
  BYTE digest[HMAC_DIGEST_SZ];
  size_t i;

  HMAC_MD5_Init(&context, key, key_len);
  if (text != NULL)
  {
    MD5_Update(&context, text, text_len);
  } else
  {
    char *t = "aaaaaaaaaa";
    for (i=0; i<1000000; i+=10)
      MD5_Update(&context, t, 10);
  }
  HMAC_MD5_Final(&context, digest, key, key_len);

  if (memcmp(expect, digest, HMAC_DIGEST_SZ))
  {
    printf("HMAC MD5 SelfTest failed\n");

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

BOOL HMAC_MD5_Test(const void *text, BOOL text_asis, const void *key, BOOL key_asis, const char *expect)
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

  rc = HMAC_MD5_TestEx(b_text, l_text, b_key, l_key, b_expect);

  if (!key_asis)
    free(b_key);
  if (!text_asis)
    free(b_text);

  return rc;
}

BOOL HMAC_MD5_SelfTest(void)
{
  const char key1[] = "00112233445566778899AABBCCDDEEFF0123456789ABCDEF00112233445566778899AABBCCDDEEFF0123456789ABCDEF00112233445566778899AABBCCDDEEFF";
  const char key2[] = "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";

  /* From RFC2202 */
  /* ------------ */

  /* 1 */
  if (!HMAC_MD5_Test("Hi There", TRUE,
                     "0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B", FALSE,
                     "9294727A3638BB1C13F48EF8158BFC9D")) return FALSE;

  /* 2 */
  if (!HMAC_MD5_Test("what do ya want for nothing?", TRUE,
                     "Jefe", TRUE,
                     "750C783E6AB0B503EAA86E310A5DB738")) return FALSE;

  /* 3 */
  if (!HMAC_MD5_Test("dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd", FALSE,
                     "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", FALSE,
                     "56be34521d144c88dbb8c733f0e8b3f6")) return FALSE;

  /* 4 */
  if (!HMAC_MD5_Test("cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd", FALSE,
                     "0102030405060708090a0b0c0d0e0f10111213141516171819", FALSE,
                     "697eaf0aca3a3aea3a75164746ffaa79")) return FALSE;

  /* 5 */
  if (!HMAC_MD5_Test("Test With Truncation", TRUE,
                     "0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c", FALSE,
                     "56461ef2342edc00f9bab995690efd4c")) return FALSE;

  /* 6 */
  if (!HMAC_MD5_Test("Test Using Larger Than Block-Size Key - Hash Key First", TRUE,
                     "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", FALSE,
                     "6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd")) return FALSE;

  /* 7 */
  if (!HMAC_MD5_Test("Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data", TRUE,
                     "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", FALSE,
                     "6f630fad67cda0ee1fb1f562db3aa53e")) return FALSE;


  /* From ???? */
  /* --------- */

  if (!HMAC_MD5_Test("", TRUE,
                     key1, FALSE,
                     "D52F69B5BB2C6B76AED78CFBDD03CDEC")) return FALSE;

  if (!HMAC_MD5_Test("a", TRUE,
                     key1, FALSE,
                     "F9B61A9E39E4D183195B2B5987D68F79")) return FALSE;

  if (!HMAC_MD5_Test("abc", TRUE,
                     key1, FALSE,
                     "95EE0FBA0B2DFAFE1C55FF3E0591DD13")) return FALSE;

  if (!HMAC_MD5_Test("message digest", TRUE,
                     key1, FALSE,
                     "73D316553451E06B5CDCB42D5058F401")) return FALSE;

  if (!HMAC_MD5_Test("abcdefghijklmnopqrstuvwxyz", TRUE,
                     key1, FALSE,
                     "CDC89D8E86BB6F6F9FAF1A24C6D1AFD3")) return FALSE;

  if (!HMAC_MD5_Test("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", TRUE,
                     key1, FALSE,
                     "65A40FFAC7A4611156AAD8F6E63E8FEF")) return FALSE;

  if (!HMAC_MD5_Test("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", TRUE,
                     key1, FALSE,
                     "57CF46867E7E8C186C59BD32D6E330BF")) return FALSE;

  if (!HMAC_MD5_Test("12345678901234567890123456789012345678901234567890123456789012345678901234567890", TRUE,
                     key1, FALSE,
                     "A00FC48A635D0DF1462E5CF9A90E0C1F")) return FALSE;

  if (!HMAC_MD5_Test(NULL, TRUE,
                     key1, FALSE,
                     "F06257DD01B3B8BE1B8E20C69F066C9E")) return FALSE;

  if (!HMAC_MD5_Test("", TRUE,
                     key2, FALSE,
                     "8DF2942E019C208F5A8AA3F80207E6DB")) return FALSE;

  if (!HMAC_MD5_Test("a", TRUE,
                     key2, FALSE,
                     "8566C0FFFE7F0A56C320223647BBF0A8")) return FALSE;

  if (!HMAC_MD5_Test("abc", TRUE,
                     key2, FALSE,
                     "D3F727A24DFF469083BA0EB2B7FD878E")) return FALSE;

  if (!HMAC_MD5_Test("message digest", TRUE,
                     key2, FALSE,
                     "31457DB43C576F3F1348A54AA9102969")) return FALSE;

  if (!HMAC_MD5_Test("abcdefghijklmnopqrstuvwxyz", TRUE,
                     key2, FALSE,
                     "60E0961A307314374210B117A275E18D")) return FALSE;

  if (!HMAC_MD5_Test("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", TRUE,
                     key2, FALSE,
                     "CCAEFAC8035313FD8E8D9E452284430A")) return FALSE;

  if (!HMAC_MD5_Test("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", TRUE,
                     key2, FALSE,
                     "B47505AD92310E81ABAF1ED299E34055")) return FALSE;

  if (!HMAC_MD5_Test("12345678901234567890123456789012345678901234567890123456789012345678901234567890", TRUE,
                     key2, FALSE,
                     "921DC8710687CEAF577984313CC98E4E")) return FALSE;

  if (!HMAC_MD5_Test(NULL, TRUE,
                     key2, FALSE,
                     "82FDDA30202CB6ACC6F24D4F8A50EB7A")) return FALSE;

  return TRUE;
}

#endif

