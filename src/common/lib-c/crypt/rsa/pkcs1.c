#include "rsa_i.h"
#include "../utils/random.h"

/*
 * RSA public-key encryption, according to PKCS #1.
 */
BOOL RSA_PublicEncrypt (BYTE *output, DWORD *outputLen, BYTE *input, DWORD inputLen, RSA_PUBLIC_KEY_ST *publicKey)
{
  BYTE byte, pkcsBlock[MAX_RSA_MODULUS_LEN];
  DWORD i, modulusLen;
  
  modulusLen = (publicKey->bits + 7) / 8;
  if (inputLen + 11 > modulusLen)
    return FALSE;
  
  pkcsBlock[0] = 0;
  /* block type 2 */
  pkcsBlock[1] = 2;

  for (i = 2; i < modulusLen - inputLen - 1; i++)
  {
    /* Find nonzero random byte.
     */
    do {
      RAND_GetBytes(&byte, 1);
    } while (byte == 0);
    pkcsBlock[i] = byte;
  }
  /* separator */
  pkcsBlock[i++] = 0;
  
  memcpy (&pkcsBlock[i], input, inputLen);
  
  if (!RSA_Public (output, outputLen, pkcsBlock, modulusLen, publicKey))
    return FALSE;
  
  /* Zeroize sensitive information.
   */
  byte = 0;
  memset (pkcsBlock, 0, sizeof (pkcsBlock));
  
  return TRUE;
}

/* RSA public-key decryption, according to PKCS #1.
 */
BOOL RSA_PublicDecrypt (BYTE *output, DWORD *outputLen, BYTE *input, DWORD inputLen, RSA_PUBLIC_KEY_ST *publicKey)
{
  BYTE pkcsBlock[MAX_RSA_MODULUS_LEN];
  DWORD i, modulusLen, pkcsBlockLen;
  
  modulusLen = (publicKey->bits + 7) / 8;
  if (inputLen > modulusLen)
    return FALSE;
  
  if (!RSA_Public(pkcsBlock, &pkcsBlockLen, input, inputLen, publicKey))
    return FALSE;
  
  if (pkcsBlockLen != modulusLen)
    return FALSE;
  
  /* Require block type 1.
   */
  if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 1))
   return FALSE;

  for (i = 2; i < modulusLen-1; i++)
    if (pkcsBlock[i] != 0xff)
      break;
    
  /* separator */
  if (pkcsBlock[i++] != 0)
    return FALSE;
  
  *outputLen = modulusLen - i;
  
  if (*outputLen + 11 > modulusLen)
    return FALSE;

  memcpy (output, &pkcsBlock[i], *outputLen);
  
  /* Zeroize potentially sensitive information.
   */
  memset (pkcsBlock, 0, sizeof (pkcsBlock));
  
  return TRUE;
}

/* RSA private-key encryption, according to PKCS #1.
 */
BOOL RSA_PrivateEncrypt (BYTE *output, DWORD *outputLen, BYTE *input, DWORD inputLen, RSA_PRIVATE_KEY_ST *privateKey)
{
  BYTE pkcsBlock[MAX_RSA_MODULUS_LEN];
  DWORD i, modulusLen;
  
  modulusLen = (privateKey->public_part.bits + 7) / 8;
  if (inputLen + 11 > modulusLen)
    return FALSE;
  
  pkcsBlock[0] = 0;
  /* block type 1 */
  pkcsBlock[1] = 1;

  for (i = 2; i < modulusLen - inputLen - 1; i++)
    pkcsBlock[i] = 0xff;

  /* separator */
  pkcsBlock[i++] = 0;
  
  memcpy (&pkcsBlock[i], input, inputLen);
  
  if (!RSA_Private (output, outputLen, pkcsBlock, modulusLen, privateKey))
    return FALSE;

  /* Zeroize potentially sensitive information.
   */
  memset (pkcsBlock, 0, sizeof (pkcsBlock));

  return TRUE;
}

/* RSA private-key decryption, according to PKCS #1.
 */
BOOL RSA_PrivateDecrypt (BYTE *output, DWORD *outputLen, BYTE *input, DWORD inputLen, RSA_PRIVATE_KEY_ST *privateKey)
{
  BYTE pkcsBlock[MAX_RSA_MODULUS_LEN];
  DWORD i, modulusLen, pkcsBlockLen;
  
  modulusLen = (privateKey->public_part.bits + 7) / 8;
  if (inputLen > modulusLen)
    return FALSE;
  
  if (!RSA_Private(pkcsBlock, &pkcsBlockLen, input, inputLen, privateKey))
    return FALSE;
  
  if (pkcsBlockLen != modulusLen)
    return FALSE;
  
  /* Require block type 2.
   */
  if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 2))
   return FALSE;

  for (i = 2; i < modulusLen-1; i++)
    /* separator */
    if (pkcsBlock[i] == 0)
      break;
    
  i++;
  if (i >= modulusLen)
    return FALSE;
    
  *outputLen = modulusLen - i;
  
  if (*outputLen + 11 > modulusLen)
    return FALSE;

  memcpy (output, &pkcsBlock[i], *outputLen);
  
  /* Zeroize sensitive information.
   */
  memset (pkcsBlock, 0, sizeof (pkcsBlock));
  
  return TRUE;
}
