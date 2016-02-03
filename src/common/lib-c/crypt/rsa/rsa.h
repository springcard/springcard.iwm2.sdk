#ifndef __RSA_H__
#define __RSA_H__

#include "../utils/bignum.h"

#define MIN_RSA_MODULUS_BITS 508
#define MAX_RSA_MODULUS_BITS 1024
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)
#define MAX_RSA_PUBLIC_EXP_LEN 4

/* Maximum lengths of encoded and encrypted content, as a function of
   content length len. Also, inverse functions.
 */
#define ENCODED_CONTENT_LEN(len) (4*(len)/3 + 3)
#define ENCRYPTED_CONTENT_LEN(len) ENCODED_CONTENT_LEN ((len)+8)
#define DECODED_CONTENT_LEN(len) (3*(len)/4 + 1)
#define DECRYPTED_CONTENT_LEN(len) (DECODED_CONTENT_LEN (len) - 1)

/* Maximum lengths of signatures, encrypted keys, encrypted
   signatures, and message digests.
 */
#define MAX_SIGNATURE_LEN MAX_RSA_MODULUS_LEN
#define MAX_PEM_SIGNATURE_LEN ENCODED_CONTENT_LEN (MAX_SIGNATURE_LEN)
#define MAX_ENCRYPTED_KEY_LEN MAX_RSA_MODULUS_LEN
#define MAX_PEM_ENCRYPTED_KEY_LEN ENCODED_CONTENT_LEN (MAX_ENCRYPTED_KEY_LEN)
#define MAX_PEM_ENCRYPTED_SIGNATURE_LEN ENCRYPTED_CONTENT_LEN (MAX_SIGNATURE_LEN)
#define MAX_DIGEST_LEN 16

/* Maximum length of Diffie-Hellman parameters.
 */
#define DH_PRIME_LEN(bits) (((bits) + 7) / 8)

/* Error codes.
 */


/* RSA public and private key.
 */
typedef struct
{
  DWORD bits;                          /* length in bits of modulus */
  BYTE  exponent[MAX_RSA_PUBLIC_EXP_LEN];        /* public exponent */
  BYTE  modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
} RSA_PUBLIC_KEY_ST;

typedef struct
{
  RSA_PUBLIC_KEY_ST public_part;
  struct
  {
    BYTE exponent[MAX_RSA_MODULUS_LEN];          /* private exponent */
    BYTE prime[2][MAX_RSA_PRIME_LEN];               /* prime factors */
    BYTE primeExponent[2][MAX_RSA_PRIME_LEN];   /* exponents for CRT */
    BYTE coefficient[MAX_RSA_PRIME_LEN];          /* CRT coefficient */
  } private_part;
} RSA_PRIVATE_KEY_ST;

#ifdef __cplusplus
extern "C" {
#endif

BOOL RSA_GenerateKeyPair(RSA_PRIVATE_KEY_ST *privateKey,
                         DWORD bits,
                         BOOL use_fermat4);

BOOL RSA_BuildPrivateKey_BI(RSA_PRIVATE_KEY_ST *privateKey,
                            const BI_DIGIT *p,
                            const BI_DIGIT *q,
                            const BI_DIGIT *e,
                            DWORD nDigits,
                            DWORD pqDigits,
                            DWORD bits);

BOOL RSA_PublicEncrypt(BYTE *output,
                       DWORD *outputLen,
                       const BYTE *input,
                       DWORD inputLen,
                       const RSA_PUBLIC_KEY_ST *publicKey);

BOOL RSA_PublicDecrypt(BYTE *output,
                       DWORD *outputLen,
                       const BYTE *input,
                       DWORD inputLen,
                       const RSA_PUBLIC_KEY_ST *publicKey);

BOOL RSA_Public(BYTE *output,
                DWORD *outputLen,
                const BYTE *input,
                DWORD inputLen,
                const RSA_PUBLIC_KEY_ST *publicKey);

BOOL RSA_Public_BI(BI_DIGIT *output,
                   const BI_DIGIT *input,
                   BI_DIGIT exponent,
                   const BI_DIGIT *modulus,
                   DWORD modDigits);

BOOL RSA_PrivateEncrypt(BYTE *output,
                        DWORD *outputLen,
                        const BYTE *input,
                        DWORD inputLen,
                        const RSA_PRIVATE_KEY_ST *privateKey);

BOOL RSA_PrivateDecrypt(BYTE *output,
                        DWORD *outputLen,
                        const BYTE *input,
                        DWORD inputLen,
                        const RSA_PRIVATE_KEY_ST *privateKey);

BOOL RSA_Private(BYTE *output,
                 DWORD *outputLen,
                 const BYTE *input,
                 DWORD inputLen,
                 const RSA_PRIVATE_KEY_ST *privateKey);

BOOL RSA_Private_BI(BI_DIGIT *output,
                    const BI_DIGIT *input,
                    DWORD digits,
                    const RSA_PRIVATE_KEY_ST *privateKey);

BOOL RSA_Private_BINoCrt(BI_DIGIT *output,
                         const BI_DIGIT *input,
                         const BI_DIGIT *secret,
                         const BI_DIGIT *modulus,
                         DWORD modDigits);

#ifdef __cplusplus
}
#endif


#endif
