#include "rsa_i.h"
#include "lib-c/crypt/utils/random.h"

/* Raw RSA public-key operation. Output has same length as modulus.

   Assumes inputLen < length of modulus.
   Requires input < modulus.
 */
BOOL RSA_Public(BYTE *output, DWORD *outputLen, const BYTE *input, DWORD inputLen, const RSA_PUBLIC_KEY_ST *publicKey)
{
  BI_DIGIT c[MAX_BI_DIGITS], e[MAX_BI_DIGITS], m[MAX_BI_DIGITS], n[MAX_BI_DIGITS];
  DWORD eDigits, nDigits;

  BI_Decode (m, MAX_BI_DIGITS, input, inputLen);
  BI_Decode (n, MAX_BI_DIGITS, publicKey->modulus, MAX_RSA_MODULUS_LEN);
  BI_Decode (e, MAX_BI_DIGITS, publicKey->exponent, MAX_RSA_PUBLIC_EXP_LEN);
  nDigits = BI_Digits (n, MAX_BI_DIGITS);
  eDigits = BI_Digits (e, MAX_BI_DIGITS);
  
  if (BI_Cmp (m, n, nDigits) >= 0)
    return FALSE;
  
  /* Compute c = m^e mod n.
   */
  BI_ModExp (c, m, e, eDigits, n, nDigits);

  *outputLen = (publicKey->bits + 7) / 8;
  BI_Encode (output, *outputLen, c, nDigits);
  
  /* Zeroize sensitive information.
   */
  memset (c, 0, sizeof (c));
  memset (m, 0, sizeof (m));

  return TRUE;
}

/* Raw RSA private-key operation. Output has same length as modulus.

   Assumes inputLen < length of modulus.
   Requires input < modulus.
 */
BOOL RSA_Private(BYTE *output, DWORD *outputLen, const BYTE *input, DWORD inputLen, const RSA_PRIVATE_KEY_ST *privateKey)
{
  BI_DIGIT c[MAX_BI_DIGITS], cP[MAX_BI_DIGITS], cQ[MAX_BI_DIGITS],
    dP[MAX_BI_DIGITS], dQ[MAX_BI_DIGITS], mP[MAX_BI_DIGITS],
    mQ[MAX_BI_DIGITS], n[MAX_BI_DIGITS], p[MAX_BI_DIGITS], q[MAX_BI_DIGITS],
    qInv[MAX_BI_DIGITS], t[MAX_BI_DIGITS];
  DWORD cDigits, nDigits, pDigits;
  
  BI_Decode(c, MAX_BI_DIGITS, input, inputLen);
  BI_Decode(n, MAX_BI_DIGITS, privateKey->public_part.modulus, MAX_RSA_MODULUS_LEN);
  BI_Decode(p, MAX_BI_DIGITS, privateKey->private_part.prime[0], MAX_RSA_PRIME_LEN);
  BI_Decode(q, MAX_BI_DIGITS, privateKey->private_part.prime[1], MAX_RSA_PRIME_LEN);
  BI_Decode(dP, MAX_BI_DIGITS, privateKey->private_part.primeExponent[0], MAX_RSA_PRIME_LEN);
  BI_Decode(dQ, MAX_BI_DIGITS, privateKey->private_part.primeExponent[1], MAX_RSA_PRIME_LEN);
  BI_Decode(qInv, MAX_BI_DIGITS, privateKey->private_part.coefficient, MAX_RSA_PRIME_LEN);
  cDigits = BI_Digits (c, MAX_BI_DIGITS);
  nDigits = BI_Digits (n, MAX_BI_DIGITS);
  pDigits = BI_Digits (p, MAX_BI_DIGITS);

  if (BI_Cmp (c, n, nDigits) >= 0)
    return FALSE;
  
  /* Compute mP = cP^dP mod p  and  mQ = cQ^dQ mod q. (Assumes q has
     length at most pDigits, i.e., p > q.)
   */
  BI_Mod (cP, c, cDigits, p, pDigits);
  BI_Mod (cQ, c, cDigits, q, pDigits);
  BI_ModExp (mP, cP, dP, pDigits, p, pDigits);
  BI_ASSIGN_ZERO (mQ, nDigits);
  BI_ModExp (mQ, cQ, dQ, pDigits, q, pDigits);
  
  /* Chinese Remainder Theorem:
       m = ((((mP - mQ) mod p) * qInv) mod p) * q + mQ.
   */
  if (BI_Cmp (mP, mQ, pDigits) >= 0)
    BI_Sub (t, mP, mQ, pDigits);
  else {
    BI_Sub (t, mQ, mP, pDigits);
    BI_Sub (t, p, t, pDigits);
  }
  BI_ModMult (t, t, qInv, p, pDigits);
  BI_Mult (t, t, q, pDigits);
  BI_Add (t, t, mQ, nDigits);

  *outputLen = (privateKey->public_part.bits + 7) / 8;
  BI_Encode (output, *outputLen, t, nDigits);

  /* Zeroize sensitive information.
   */
  memset (c, 0, sizeof (c));
  memset (cP, 0, sizeof (cP));
  memset (cQ, 0, sizeof (cQ));
  memset (dP, 0, sizeof (dP));
  memset (dQ, 0, sizeof (dQ));
  memset (mP, 0, sizeof (mP));
  memset (mQ, 0, sizeof (mQ));
  memset (p, 0, sizeof (p));
  memset (q, 0, sizeof (q));
  memset (qInv, 0, sizeof (qInv));
  memset (t, 0, sizeof (t));

  return TRUE;
}

BOOL RSA_Private_BI(BI_DIGIT *output, const BI_DIGIT *input, DWORD digits, const RSA_PRIVATE_KEY_ST *privateKey)
{
  BI_DIGIT cP[MAX_BI_DIGITS], cQ[MAX_BI_DIGITS],
    dP[MAX_BI_DIGITS], dQ[MAX_BI_DIGITS], mP[MAX_BI_DIGITS],
    mQ[MAX_BI_DIGITS], n[MAX_BI_DIGITS], p[MAX_BI_DIGITS], q[MAX_BI_DIGITS],
    qInv[MAX_BI_DIGITS], t[MAX_BI_DIGITS];
  DWORD cDigits, nDigits, pqDigits;

  BI_Decode(n, MAX_BI_DIGITS, privateKey->public_part.modulus, MAX_RSA_MODULUS_LEN);
  BI_Decode(p, MAX_BI_DIGITS, privateKey->private_part.prime[0], MAX_RSA_PRIME_LEN);
  BI_Decode(q, MAX_BI_DIGITS, privateKey->private_part.prime[1], MAX_RSA_PRIME_LEN);
  BI_Decode(dP, MAX_BI_DIGITS, privateKey->private_part.primeExponent[0], MAX_RSA_PRIME_LEN);
  BI_Decode(dQ, MAX_BI_DIGITS, privateKey->private_part.primeExponent[1], MAX_RSA_PRIME_LEN);
  BI_Decode(qInv, MAX_BI_DIGITS, privateKey->private_part.coefficient, MAX_RSA_PRIME_LEN);
  cDigits  = BI_Digits (input, digits);
  nDigits  = BI_Digits (n, MAX_BI_DIGITS);
  pqDigits = BI_Digits (p, MAX_BI_DIGITS);

  if (BI_Cmp (input, n, cDigits) >= 0)
    return FALSE;
 
  /* Compute mP = cP^dP mod p  and  mQ = cQ^dQ mod q. (Assumes q has
     length at most pDigits, i.e., p > q.)
   */
  BI_Mod (cP, input, cDigits, p, pqDigits);
  BI_Mod (cQ, input, cDigits, q, pqDigits);
  BI_ModExp (mP, cP, dP, pqDigits, p, pqDigits);
  BI_ASSIGN_ZERO (mQ, nDigits);
  BI_ModExp (mQ, cQ, dQ, pqDigits, q, pqDigits);
  
  /* Chinese Remainder Theorem:
       m = ((((mP - mQ) mod p) * qInv) mod p) * q + mQ.
   */
  if (BI_Cmp (mP, mQ, pqDigits) >= 0)
    BI_Sub (t, mP, mQ, pqDigits);
  else {
    BI_Sub (t, mQ, mP, pqDigits);
    BI_Sub (t, p, t, pqDigits);
  }
  BI_ModMult (t, t, qInv, p, pqDigits);
  BI_Mult (t, t, q, pqDigits);
  BI_Add (output, t, mQ, nDigits);

  /* Zeroize sensitive information.
   */
  memset (cP, 0, sizeof (cP));
  memset (cQ, 0, sizeof (cQ));
  memset (dP, 0, sizeof (dP));
  memset (dQ, 0, sizeof (dQ));
  memset (mP, 0, sizeof (mP));
  memset (mQ, 0, sizeof (mQ));
  memset (p, 0, sizeof (p));
  memset (q, 0, sizeof (q));
  memset (qInv, 0, sizeof (qInv));
  memset (t, 0, sizeof (t));

  return TRUE;
}

/*
 * Compute p = s ^ e mod n
 * -----------------------
 */
BOOL RSA_Public_BI(BI_DIGIT *p, const BI_DIGIT *s, BI_DIGIT e, const BI_DIGIT *n, DWORD nDigits)
{
  BI_ModExp (p, s, &e, 1, n, nDigits);
  return TRUE;
}

/*
 * Compute s = m ^ k mod n
 * -----------------------
 */
BOOL RSA_Private_BINoCrt(BI_DIGIT *s, const BI_DIGIT *m, const BI_DIGIT *k, const BI_DIGIT *n, DWORD nDigits)
{
  BI_ModExp (s, m, k, nDigits, n, nDigits);
  return TRUE;
}

