#include "rsa_i.h"
#include "../utils/random.h"

static void GenerateDigits(BI_DIGIT *a, unsigned int digits);

BOOL RSA_BuildPrivateKey_BI(RSA_PRIVATE_KEY_ST *privateKey, const BI_DIGIT *p, const BI_DIGIT *q, const BI_DIGIT *e, DWORD nDigits, DWORD pqDigits, DWORD bits)
{
  BI_DIGIT d[MAX_BI_DIGITS], dP[MAX_BI_DIGITS], dQ[MAX_BI_DIGITS],
    n[MAX_BI_DIGITS], phiN[MAX_BI_DIGITS],
    pMinus1[MAX_BI_DIGITS], qInv[MAX_BI_DIGITS],
    qMinus1[MAX_BI_DIGITS], t[MAX_BI_DIGITS];
 
  /* Sort so that p > q. (p = q case is extremely unlikely.)
   */
  if (BI_Cmp (p, q, pqDigits) < 0)
	{
	  const BI_DIGIT *t;
		t = p; p = q; q = t;
  }

  /* Compute n = pq, qInv = q^{-1} mod p, d = e^{-1} mod (p-1)(q-1),
     dP = d mod p-1, dQ = d mod q-1.
   */
  BI_Mult (n, p, q, pqDigits);
  BI_ModInv (qInv, q, p, pqDigits);
  
  BI_ASSIGN_DIGIT (t, 1, pqDigits);
  BI_Sub (pMinus1, p, t, pqDigits);
  BI_Sub (qMinus1, q, t, pqDigits);
  BI_Mult (phiN, pMinus1, qMinus1, pqDigits);

  BI_ModInv (d, e, phiN, nDigits);
  BI_Mod (dP, d, nDigits, pMinus1, pqDigits);
  BI_Mod (dQ, d, nDigits, qMinus1, pqDigits);
  
  privateKey->public_part.bits = bits;
  BI_Encode (privateKey->public_part.modulus, MAX_RSA_MODULUS_LEN, n, nDigits);
  BI_Encode (privateKey->public_part.exponent, MAX_RSA_PUBLIC_EXP_LEN, e, 1);
  BI_Encode (privateKey->private_part.exponent, MAX_RSA_MODULUS_LEN, d, nDigits);
  BI_Encode (privateKey->private_part.prime[0], MAX_RSA_PRIME_LEN, p, pqDigits);
  BI_Encode (privateKey->private_part.prime[1], MAX_RSA_PRIME_LEN, q, pqDigits);
  BI_Encode (privateKey->private_part.primeExponent[0], MAX_RSA_PRIME_LEN, dP, pqDigits);
  BI_Encode (privateKey->private_part.primeExponent[1], MAX_RSA_PRIME_LEN, dQ, pqDigits);
  BI_Encode (privateKey->private_part.coefficient, MAX_RSA_PRIME_LEN, qInv, pqDigits);
    
  /* Zeroize sensitive information.
   */
  memset (d, 0, sizeof (d));
  memset (dP, 0, sizeof (dP));
  memset (dQ, 0, sizeof (dQ));
  memset (phiN, 0, sizeof (phiN));
  memset (pMinus1, 0, sizeof (pMinus1));
  memset (qInv, 0, sizeof (qInv));
  memset (qMinus1, 0, sizeof (qMinus1));
  memset (t, 0, sizeof (t));
  
  return TRUE;
}

/* Generates an RSA key pair with a given length and public exponent.
 */
BOOL RSA_GenerateKeyPair(RSA_PRIVATE_KEY_ST *privateKey, DWORD bits, BOOL use_fermat4)
{
  BI_DIGIT e[MAX_BI_DIGITS], p[MAX_BI_DIGITS], q[MAX_BI_DIGITS];
  DWORD nDigits, pqDigits;
  
  if ((bits < MIN_RSA_MODULUS_BITS) || 
      (bits > MAX_RSA_MODULUS_BITS))
    return FALSE;
  nDigits = (bits + BI_DIGIT_BITS - 1) / BI_DIGIT_BITS;
  pqDigits = (nDigits + 1) / 2;
  
  /* Generate random RSA primes p and q so that product has correct length.
   */
  GenerateDigits (p, pqDigits);
  GenerateDigits (q, pqDigits);

  /* NOTE: for 65537, this assumes BI_DIGIT is at least 17 bits. */
  if (use_fermat4) {
    BI_ASSIGN_DIGIT (e, 65537, nDigits);
  } else {
    BI_ASSIGN_DIGIT (e, 3, nDigits);
  }
  FindRSAPrime (p, (bits + 1) / 2, p, pqDigits, e, 1);
  FindRSAPrime (q, bits / 2, q, pqDigits, e, 1);

  if (!RSA_BuildPrivateKey_BI(privateKey, p, q, e, nDigits, pqDigits, bits))
    return FALSE;

  memset (p, 0, sizeof (p));
  memset (q, 0, sizeof (q));

  return TRUE;
}

static void GenerateDigits(BI_DIGIT *a, unsigned int digits)
{
  unsigned char t[MAX_RSA_MODULUS_LEN];
  RAND_GetBytes(t, digits * BI_DIGIT_LEN);
  BI_Decode (a, digits, t, digits * BI_DIGIT_LEN);
  memset (t, 0, sizeof (t));
}


/* Generates a probable prime a between b and c such that a-1 is
   divisible by d.

   Lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes b < c, digits < MAX_BI_DIGITS.
   
 */
BOOL GeneratePrime(BI_DIGIT *a, BI_DIGIT *b, BI_DIGIT *c, BI_DIGIT *d, DWORD digits)
{
  BI_DIGIT t[MAX_BI_DIGITS], u[MAX_BI_DIGITS];

  /* Generate random number between b and c.
   */
  GenerateDigits(a, digits);

  BI_Sub (t, c, b, digits);
  BI_ASSIGN_DIGIT (u, 1, digits);
  BI_Add (t, t, u, digits);
  BI_Mod (a, a, digits, t, digits);
  BI_Add (a, a, b, digits);

  /* Adjust so that a-1 is divisible by d.
   */
  BI_Mod (t, a, digits, d, digits);
  BI_Sub (a, a, t, digits);
  BI_Add (a, a, u, digits);
  if (BI_Cmp (a, b, digits) < 0)
    BI_Add (a, a, d, digits);
  if (BI_Cmp (a, c, digits) > 0)
    BI_Sub (a, a, d, digits);

  /* Search to c in steps of d.
   */
  BI_Assign (t, c, digits);
  BI_Sub (t, t, d, digits);

  while (!ProbablePrime (a, digits))
  {
    if (BI_Cmp (a, t, digits) > 0)
      return FALSE;
    BI_Add (a, a, d, digits);
  }

  return TRUE;
}
