#include "rsa_i.h"

static unsigned int SMALL_PRIMES[] = { 3, 5, 7, 11 };
#define SMALL_PRIME_COUNT 4

static int RSAPrime(BI_DIGIT *, unsigned int, BI_DIGIT *, unsigned int);
static int SmallFactor(BI_DIGIT *, unsigned int);
static int FermatTest(BI_DIGIT *, unsigned int);
static int RelativelyPrime(BI_DIGIT *, unsigned int, BI_DIGIT *, unsigned int);

/* Find a probable prime a between 3*2^(b-2) and 2^b-1, starting at
   3*2^(b-2) + (c mod 2^(b-2)), such that gcd (a-1, d) = 1.

   Lengths: a[cDigits], c[cDigits], d[dDigits].
   Assumes b > 2, b < cDigits * BI_DIGIT_BITS, d is odd,
           cDigits < MAX_BI_DIGITS, dDigits < MAX_BI_DIGITS, and a
           probable prime can be found.
 */
void FindRSAPrime (BI_DIGIT *a, unsigned int b, BI_DIGIT *c, unsigned int cDigits, BI_DIGIT *d, unsigned int dDigits)
{
  BI_DIGIT t[MAX_BI_DIGITS], u[MAX_BI_DIGITS], v[MAX_BI_DIGITS],
    w[MAX_BI_DIGITS];
  
  /* Compute t = 2^(b-2), u = 3*2^(b-2).
   */
  BI_Assign2Exp (t, b-2, cDigits);
  BI_Assign2Exp (u, b-1, cDigits);
  BI_Add (u, u, t, cDigits);
  
  /* Compute v = 3*2^(b-2) + (c mod 2^(b-2)); add one if even.
   */
  BI_Mod (v, c, cDigits, t, cDigits);
  BI_Add (v, v, u, cDigits);
  if (BI_EVEN (v, cDigits)) {
    BI_ASSIGN_DIGIT (w, 1, cDigits);
    BI_Add (v, v, w, cDigits);
  }
  
  /* Compute w = 2, u = 2^b - 2.
   */
  BI_ASSIGN_DIGIT (w, 2, cDigits);
  BI_Sub (u, u, w, cDigits);
  BI_Add (u, u, t, cDigits);

  /* Search to 2^b-1 from starting point, then from 3*2^(b-2)+1.
   */
  while (! RSAPrime (v, cDigits, d, dDigits)) {
    if (BI_Cmp (v, u, cDigits) > 0)
      BI_Sub (v, v, t, cDigits);
    BI_Add (v, v, w, cDigits);
  }
  
  BI_Assign (a, v, cDigits);
  
  /* Zeroize sensitive information.
   */
  memset (v, 0, sizeof (v));
}

/* Returns nonzero iff a is a probable prime and GCD (a-1, b) = 1.

   Lengths: a[aDigits], b[bDigits].
   Assumes aDigits < MAX_BI_DIGITS, bDigits < MAX_BI_DIGITS.
 */
static int RSAPrime (BI_DIGIT *a, unsigned int aDigits, BI_DIGIT *b, unsigned int bDigits)
{
  int status;
  BI_DIGIT aMinus1[MAX_BI_DIGITS], t[MAX_BI_DIGITS];
  
  BI_ASSIGN_DIGIT (t, 1, aDigits);
  BI_Sub (aMinus1, a, t, aDigits);
  
  status = ProbablePrime (a, aDigits) &&
    RelativelyPrime (aMinus1, aDigits, b, bDigits);

  /* Zeroize sensitive information.
   */
  memset (aMinus1, 0, sizeof (aMinus1));
  
  return (status);
}

/* Returns nonzero iff a is a probable prime.

   Lengths: a[aDigits].
   Assumes aDigits < MAX_BI_DIGITS.
 */
int ProbablePrime (BI_DIGIT *a, unsigned int aDigits)
{
  return (! SmallFactor (a, aDigits) && FermatTest (a, aDigits));
}

/* Returns nonzero iff a has a prime factor in SMALL_PRIMES.

   Lengths: a[aDigits].
   Assumes aDigits < MAX_BI_DIGITS.
 */
static int SmallFactor (BI_DIGIT *a, unsigned int aDigits)
{
  int status;
  BI_DIGIT t[1];
  unsigned int i;
  
  status = 0;
  
  for (i = 0; i < SMALL_PRIME_COUNT; i++) {
    BI_ASSIGN_DIGIT (t, SMALL_PRIMES[i], 1);
    BI_Mod (t, a, aDigits, t, 1);
    if (BI_Zero (t, 1)) {
      status = 1;
      break;
    }
  }
  
  /* Zeroize sensitive information.
   */
  i = 0;
  memset (t, 0, sizeof (t));

  return (status);
}

/* Returns nonzero iff a passes Fermat's test for witness 2.
   (All primes pass the test, and nearly all composites fail.)
     
   Lengths: a[aDigits].
   Assumes aDigits < MAX_BI_DIGITS.
 */
static int FermatTest (BI_DIGIT *a, unsigned int aDigits)
{
  int status;
  BI_DIGIT t[MAX_BI_DIGITS], u[MAX_BI_DIGITS];
  
  BI_ASSIGN_DIGIT (t, 2, aDigits);
  BI_ModExp (u, t, a, aDigits, a, aDigits);
  
  status = BI_EQUAL (t, u, aDigits);
  
  /* Zeroize sensitive information.
   */
  memset (u, 0, sizeof (u));
  
  return (status);
}

/* Returns nonzero iff a and b are relatively prime.

   Lengths: a[aDigits], b[bDigits].
   Assumes aDigits >= bDigits, aDigits < MAX_BI_DIGITS.
 */
static int RelativelyPrime (BI_DIGIT *a, unsigned int aDigits, BI_DIGIT *b, unsigned int bDigits)
{
  int status;
  BI_DIGIT t[MAX_BI_DIGITS], u[MAX_BI_DIGITS];
  
  BI_ASSIGN_ZERO (t, aDigits);
  BI_Assign (t, b, bDigits);
  BI_Gcd (t, a, t, aDigits);
  BI_ASSIGN_DIGIT (u, 1, aDigits);

  status = BI_EQUAL (t, u, aDigits);
  
  /* Zeroize sensitive information.
   */
  memset (t, 0, sizeof (t));
  
  return (status);
}
