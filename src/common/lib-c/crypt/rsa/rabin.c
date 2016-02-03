#include "rsa_i.h"
#include "rabin.h"

/*
 * Compute s as 2.s - 1 = 1/2 GCM(p-1;q-1)
 * ---------------------------------------
 */
BOOL RABIN_ComputePrivateKey_BI(BI_DIGIT *s, DWORD nDigits, BI_DIGIT *p, BI_DIGIT *q, DWORD pqDigits)
{
  int k;
  BI_DIGIT phi[MAX_BI_DIGITS], GCD[MAX_BI_DIGITS], GCM[MAX_BI_DIGITS];
  BI_DIGIT pM1[MAX_BI_DIGITS/2], qM1[MAX_BI_DIGITS/2];
  BI_DIGIT t[MAX_BI_DIGITS], u[2*MAX_BI_DIGITS], v[MAX_BI_DIGITS];

  BI_ASSIGN_DIGIT (t, 1, pqDigits);
  /* Compute p-1 */
  BI_Sub (pM1, p, t, pqDigits);
  /* Compute q-1 */
  BI_Sub (qM1, q, t, pqDigits);
  /* Compute phi=(p-1)(q-1) */
  BI_ASSIGN_ZERO(phi, pqDigits); /* JDA */
  BI_Mult(phi, pM1, qM1, pqDigits);

  /* Compute GCM(p-1;q-1) = (p-1).(q-1)/GCD(p-1;q-1) = phi/GCD(p-1;q-1) */
  BI_ASSIGN_ZERO(GCD, pqDigits); /* JDA */
  BI_Gcd(GCD, pM1, qM1, pqDigits);

  BI_ASSIGN_ZERO(GCM, nDigits); /* JDA */
  BI_Div(GCM, t, phi, nDigits, GCD, pqDigits);

  /* Find the smallest s */
  for (k=1; k<256; k++)
  {
    BI_ASSIGN_DIGIT(t, k, nDigits);        /* t = k */
    BI_Mult(u, GCM, t, nDigits);           /* u = t.GCM = k.GCM */

    BI_ASSIGN_DIGIT(t, 2, nDigits);        /* t = 2 */
    BI_Div(v, t, u, nDigits, t, nDigits);  /* v = k.GCM/2 , modulus is t */

    if (BI_Zero(t, nDigits))
    {
      /* modulus is zero -> I can choose this k */
      BI_ASSIGN_DIGIT(t, 1, nDigits);      /* t = 1 */
      BI_Add(v, v, t, nDigits);            /* v = v + t = v + 1 = k.GCM/2 + 1 */
      BI_ASSIGN_DIGIT(t, 2, nDigits);      /* t = 2 */
      BI_Div(s, t, v, nDigits, t, nDigits);   /* this is s, modulus is t */

      if (BI_Zero(t, nDigits))
      {
        /* modulus is zero -> I have found s ! */
        return TRUE;
      }
    }
  }

  return FALSE;
}

BOOL RABIN_BuildPrivateKey_BI(RSA_PRIVATE_KEY_ST *privateKey, BI_DIGIT *p, DWORD pDigits, BI_DIGIT *q, DWORD qDigits, DWORD nDigits, DWORD nBits)
{
  BI_DIGIT d[MAX_BI_DIGITS], dP[MAX_BI_DIGITS], dQ[MAX_BI_DIGITS],
    n[MAX_BI_DIGITS], phiN[MAX_BI_DIGITS],
    pMinus1[MAX_BI_DIGITS], qInv[MAX_BI_DIGITS],
    qMinus1[MAX_BI_DIGITS], t[MAX_BI_DIGITS];
	DWORD pqDigits;
 
  /* Sort so that p > q. (p = q case is extremely unlikely.)
   */
	if (pDigits > qDigits) pqDigits = pDigits; else pqDigits = qDigits;
  if (BI_Cmp (p, q, pqDigits) < 0)
	{
    BI_Assign (t, p, pqDigits);
    BI_Assign (p, q, pqDigits);
    BI_Assign (q, t, pqDigits);
  }

  if (!RABIN_ComputePrivateKey_BI(d, nDigits, p, q, pqDigits))
    return FALSE;

  /* Compute n = pq, qInv = q^{-1} mod p, d = e^{-1} mod (p-1)(q-1),
     dP = d mod p-1, dQ = d mod q-1.
   */
  BI_Mult (n, p, q, pqDigits);
  BI_ModInv (qInv, q, p, pqDigits);
  
  BI_ASSIGN_DIGIT (t, 1, pqDigits);
  BI_Sub (pMinus1, p, t, pqDigits);
  BI_Sub (qMinus1, q, t, pqDigits);

  BI_Mod (dP, d, nDigits, pMinus1, pqDigits);
  BI_Mod (dQ, d, nDigits, qMinus1, pqDigits);
  
  privateKey->public_part.bits = nBits;
  BI_Encode (privateKey->public_part.modulus, MAX_RSA_MODULUS_LEN, n, nDigits);
  BI_ASSIGN_DIGIT (t, 2, 1);
  BI_Encode (privateKey->public_part.exponent, MAX_RSA_PUBLIC_EXP_LEN, t, 1);
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

/*
 * Crible pour valider que p et q sont utilisables en Rabin
 * --------------------------------------------------------
 */
BOOL RABIN_ValidatePrimes_BI(BI_DIGIT *p, DWORD pDigits, BI_DIGIT *q, DWORD qDigits, DWORD nDigits, DWORD nBits)
{
  BI_DIGIT n[MAX_BI_DIGITS];
  BYTE p_mod_8, q_mod_8;
 
  /* Verification des conditions sur p et q */
  p_mod_8 = (BYTE) BI_MOD_DIGIT(p, 8, pDigits);
  q_mod_8 = (BYTE) BI_MOD_DIGIT(q, 8, qDigits);

  if (q_mod_8 == p_mod_8)
    return FALSE;
  if ((p_mod_8 != 3) && (p_mod_8 != 7))
    return FALSE;
  if ((q_mod_8 != 3) && (q_mod_8 != 7))
    return FALSE;

  /* Calcul de n et verification de la taille */
	if (pDigits > qDigits)
    BI_Mult(n, p, q, pDigits);
	else
	  BI_Mult(n, p, q, qDigits);

  if (BI_Bits(n, nDigits) != nBits)
    return FALSE;

  return TRUE;
}

/*
 * Generation de P et Q utilisables en Rabin
 * -----------------------------------------
 */
BOOL RABIN_GeneratePrimes_BI(BI_DIGIT *p, BI_DIGIT *q, DWORD nDigits, DWORD nBits)
{
  BI_DIGIT t[MAX_BI_DIGITS];
  BI_DIGIT u[MAX_BI_DIGITS];
  BI_DIGIT v[MAX_BI_DIGITS];
  BI_DIGIT n[MAX_BI_DIGITS];
  DWORD pqBits, pqDigits;
  BYTE p_mod_8, q_mod_8;

  pqDigits = nDigits / 2;
  pqBits   = nBits / 2;

  /*
   * Generate prime p between 3*2^(pqBits-2) and 2^pqBits-1, searching
   *  in steps of 2, until one satisfies p mod 8 = 3 or p mod 8 = 7
   */
  BI_Assign2Exp(t, (WORD) (pqBits-1), pqDigits);
  BI_Assign2Exp(u, (WORD) (pqBits-2), pqDigits);
  BI_Add(t, t, u, pqDigits);
  BI_ASSIGN_DIGIT(v, 1, pqDigits);
  BI_Sub(v, t, v, pqDigits);
  BI_Add(u, u, v, pqDigits);
  BI_ASSIGN_DIGIT(v, 2, pqDigits);
  for (;;)
  {
    if (!GeneratePrime(p, t, u, v, pqDigits))
      return FALSE;

    p_mod_8 = (BYTE) BI_MOD_DIGIT(p, 8, pqDigits);
    if (p_mod_8 == 3) break;
    if (p_mod_8 == 7) break;
  } 
  if (BI_Bits(p, pqDigits) != pqBits)
    return FALSE;
   
  /*
   * Generate prime q between 3*2^(pqBits-2) and 2^pqBits-1, searching
   *  in steps of 2, until one satisfies q mod 8 = 3 or q mod 8 = 7 and
   *  q mod 8 != p mod 8
   */
  BI_Assign2Exp(t, (WORD) (pqBits-1), pqDigits);
  BI_Assign2Exp(u, (WORD) (pqBits-2), pqDigits);
  BI_Add(t, t, u, pqDigits);
  BI_ASSIGN_DIGIT(v, 1, pqDigits);
  BI_Sub(v, t, v, pqDigits);
  BI_Add(u, u, v, pqDigits);
  BI_ASSIGN_DIGIT(v, 2, pqDigits);
  for (;;)
  {
    if (!GeneratePrime(q, t, u, v, pqDigits))
      return FALSE;

    q_mod_8 = (BYTE) BI_MOD_DIGIT(q, 8, pqDigits);
    if (q_mod_8 == p_mod_8) continue;
    if (q_mod_8 == 3) break;
    if (q_mod_8 == 7) break;
  }
  if (BI_Bits(q, pqDigits) != pqBits)
    return FALSE;

  /*
   * Check size of generated n=p.q
   */
  BI_Mult(n, p, q, pqDigits);
  if (BI_Bits(n, nDigits) != nBits)
    return FALSE;
  
  /*
   * Sort so that p > q. (p = q case is extremely unlikely.)
   */
  if (BI_Cmp (p, q, pqDigits) < 0)
  {
    BI_Assign (t, p, pqDigits);
    BI_Assign (p, q, pqDigits);
    BI_Assign (q, t, pqDigits);
  }

  return TRUE;
}

#ifdef _DEBUG

#include <stdio.h>
#include <string.h>

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

static DWORD s2b(BYTE dst[], const char *src)
{
  DWORD i;
	DWORD l = strlen(src) / 2;

	for (i=0; i<l; i++)
		dst[i] = (BYTE) ((c2b(src[2*i])<<4) + c2b(src[2*i+1]));

	return l;
}

static const char sP[] = "ED684A7ED3F1DB56DD6A696F2E538B300F459FDAB0BAE1E3BAB75373E1E37250338B14768A4A5AF078E5826E49511D9F";
static const char sQ[] = "C3A1D5A55A5F66C930E7AA20AFAED1A38D9CDEEE37EA94D4D5017AA74A7F176B43690BC8CF41F94E08CE360331FF1E0B";
static const char sD[] = "16AD90AED266E64F6A054185D0AFE5BDCDC143813C907D0A31A12B1E41BAB999F537D1E6110C18EE9F33F046A13D0CCD7C6A4EE115BA0073509DF220D151E7DD68D52EEEDBD9D83B8FE663F7789C823AA047C2090F23A3A78C6F480703A0B586";
static const char sN[] = "B56C85769337327B502A0C2E857F2DEE6E0A1C09E483E8518D0958F20DD5CCCFA9BE8F308860C774F99F823509E8666D945C972CDC2145BA9341A49668919BBEE38BF63FC77438950EEBEDD6F1469B9079323087D2A9717AE52DF8A99855E7D5";

BOOL RABIN_SelfTest(void)
{
  BYTE t[128];
  DWORD l, nDigits, pqDigits;
  /* p, q, n et d */
  BI_DIGIT p[MAX_BI_DIGITS], q[MAX_BI_DIGITS], n[MAX_BI_DIGITS], d[MAX_BI_DIGITS];
  /* buffer de travail */
  BI_DIGIT w[MAX_BI_DIGITS];

  printf("Validate with the reference key\n");

  /* Recuperation de P, Q */
  l = s2b(t, sP);
  BI_Decode(p, MAX_BI_DIGITS, t, l);
  pqDigits = BI_Digits(p, MAX_BI_DIGITS);

  l = s2b(t, sQ);
  BI_Decode(q, MAX_BI_DIGITS, t, l);

  /* verification que p et q passent le crible */ 
  if (!RABIN_ValidatePrimes_BI(p, pqDigits, q, pqDigits, 2*pqDigits, 768))
  {
    printf("Rabin failed %s %d (ValidatePrimes)\n", __FILE__, __LINE__);
    return FALSE;
  }

  /* Recuperation de N et D */
  l = s2b(t, sN);
  BI_Decode(n, MAX_BI_DIGITS, t, l);
  nDigits = BI_Digits(n, MAX_BI_DIGITS);

  l = s2b(t, sD);
  BI_Decode(d, MAX_BI_DIGITS, t, l);

  /* Re-calcul de N */
  BI_Mult(w, p, q, pqDigits);
  if (!BI_EQUAL(w, n, nDigits))
  {
    printf("Rabin failed %s %d (calcul de N)\n", __FILE__, __LINE__);
    return FALSE;
  }

  /* Re-calcul de D */
  RABIN_ComputePrivateKey_BI(w, MAX_BI_DIGITS, p, q, MAX_BI_DIGITS / 2);
  if (!BI_EQUAL(w, d, nDigits))
  {
    printf("Rabin failed %s %d (calcul de D)\n", __FILE__, __LINE__);
    return FALSE;
  }

  /* Generation d'une cle */
  printf("Create a new 1024 bits key\n");

  nDigits = 1024/(8 * sizeof(DWORD));
  if (!RABIN_GeneratePrimes_BI(p, q, nDigits, 1024))
  {
    printf("Rabin failed %s %d (generation d'une cle)\n", __FILE__, __LINE__);
    return FALSE;
  }

  printf("Validate the new key\n");

  if (!RABIN_ValidatePrimes_BI(p, pqDigits, q, pqDigits, nDigits, 1024))
  {
    printf("Rabin failed %s %d (validation d'une cle)\n", __FILE__, __LINE__);
    return FALSE;
  }

  if (!RABIN_ComputePrivateKey_BI(w, nDigits, p, q, nDigits / 2))
  {
    printf("Rabin failed %s %d (assemblage d'une cle)\n", __FILE__, __LINE__);
    return FALSE;
  }

  return TRUE;
}

#endif
