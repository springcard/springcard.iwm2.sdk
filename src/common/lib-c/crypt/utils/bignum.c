#include "bignum.h"
#include <stdio.h>

__inline static BI_DIGIT BI_AddDigitMult(BI_DIGIT *a, const BI_DIGIT *b, BI_DIGIT c, const BI_DIGIT *d, DWORD digits);
__inline static BI_DIGIT BI_SubDigitMult(BI_DIGIT *a, const BI_DIGIT *b, BI_DIGIT c, const BI_DIGIT *d, DWORD digits);
__inline static void BI_DigitMult(BI_DIGIT [2], BI_DIGIT, BI_DIGIT);
__inline static void BI_DigitDiv(BI_DIGIT *, BI_DIGIT [2], BI_DIGIT);
__inline static unsigned int BI_DigitBits(BI_DIGIT a);

void BI_Disp(char *msg, BI_DIGIT *t, DWORD digits)
{
  signed long i;

  if (msg != NULL)
    printf("%s", msg);
  for (i=digits-1; i>=0; i--)
    printf("%08lX", t[i]);
  if (msg != NULL)
    printf("\n");
}

/* Decodes character string b into a, where character string is ordered
   from most to least significant.

   Length: a[digits], b[len].
   Assumes b[i] = 0 for i < len - digits * BI_DIGIT_LEN. (Otherwise most
   significant bytes are truncated.)
 */
void BI_Decode(BI_DIGIT *a, DWORD digits, const BYTE *b, DWORD len)
{
  register unsigned int i, u;
  register signed int j;

  BI_ASSIGN_ZERO(a, digits);
  
  for (i = 0, j = len - 1; j >= 0; i++)
  {
    register BI_DIGIT t;
    t = 0;
    for (u = 0; j >= 0 && u < BI_DIGIT_BITS; j--, u += 8)
      t |= ((BI_DIGIT) b[j]) << u;
    a[i] = t;
  }
}

/* Encodes b into character string a, where character string is ordered
   from most to least significant.

   Lengths: a[len], b[digits].
   Assumes BI_Bits (b, digits) <= 8 * len. (Otherwise most significant
   digits are truncated.)
 */
void BI_Encode (BYTE *a, DWORD len, const BI_DIGIT *b, DWORD digits)
{
  BI_DIGIT t;
  int j;
  unsigned int i, u;

  for (i = 0, j = len - 1; i < digits; i++) {
    t = b[i];
    for (u = 0; j >= 0 && u < BI_DIGIT_BITS; j--, u += 8)
      a[j] = (unsigned char)(t >> u);
  }

  for (; j >= 0; j--)
    a[j] = 0;
}

/* Assigns a = 0.

   Lengths: a[digits], b[digits].
 */
void BI_Assign (BI_DIGIT *a, const BI_DIGIT *b, DWORD digits)
{
  unsigned int i;

  for (i = 0; i < digits; i++)
    a[i] = b[i];
}

/* Assigns a = 2^b.

   Lengths: a[digits].
   Requires b < digits * BI_DIGIT_BITS.
 */
void BI_Assign2Exp (BI_DIGIT *a, DWORD b, DWORD digits)
{
  BI_ASSIGN_ZERO (a, digits);

  if (b >= digits * BI_DIGIT_BITS)
    return;

  a[b / BI_DIGIT_BITS] = (BI_DIGIT)1 << (b % BI_DIGIT_BITS);
}

/* Computes a = b + c. Returns carry.

   Lengths: a[digits], b[digits], c[digits].
 */
BI_DIGIT BI_Add (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits)
{
  BI_DIGIT ai, carry;
  unsigned int i;

  carry = 0;

  for (i = 0; i < digits; i++) {
    if ((ai = b[i] + carry) < carry)
      ai = c[i];
    else if ((ai += c[i]) < c[i])
      carry = 1;
    else
      carry = 0;
    a[i] = ai;
  }

  return (carry);
}

/* Computes a = b - c. Returns borrow.

   Lengths: a[digits], b[digits], c[digits].
 */
BI_DIGIT BI_Sub(BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits)
{
  BI_DIGIT borrow;
  register unsigned int i;

  borrow = 0;

  for (i = 0; i < digits; i++)
  {
    register BI_DIGIT ai;

    ai = b[i] - borrow;
    if (ai > (MAX_BI_DIGIT - borrow))
    {
      ai = MAX_BI_DIGIT - c[i];
    } else
    {
      ai -= c[i];
      if (ai > (MAX_BI_DIGIT - c[i]))
        borrow = 1;
      else
        borrow = 0;
    }
    a[i] = ai;
  }

  return borrow;
}

/* Computes a = b * c.

   Lengths: a[2*digits], b[digits], c[digits].
   Assumes digits < MAX_BI_DIGITS.
 */
void BI_Mult(BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits)
{
  BI_DIGIT t[2*MAX_BI_DIGITS];
  BI_DIGIT bDigits, cDigits;
  register unsigned int i;

  BI_ASSIGN_ZERO(t, 2 * digits);
  
  bDigits = BI_Digits(b, digits);
  cDigits = BI_Digits(c, digits);

  for (i = 0; i < bDigits; i++)
    t[i+cDigits] += BI_AddDigitMult(&t[i], &t[i], b[i], c, cDigits);
  
  BI_Assign(a, t, 2 * digits);
  
  /* Zeroize potentially sensitive information.
   */
  memset(t, 0, sizeof (t));
}

/* Computes a = b mod c.

   Lengths: a[cDigits], b[bDigits], c[cDigits].
   Assumes c > 0, bDigits < 2 * MAX_BI_DIGITS, cDigits < MAX_BI_DIGITS.
 */
void BI_Mod (BI_DIGIT *a, const BI_DIGIT *b, DWORD bDigits, const BI_DIGIT *c, DWORD cDigits)
{
  BI_DIGIT t[2 * MAX_BI_DIGITS];
  
  BI_Div (t, a, b, bDigits, c, cDigits);
  
  /* Zeroize potentially sensitive information.
   */
  memset (t, 0, sizeof (t));
}

/* Computes a = b * c mod d.

   Lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes d > 0, digits < MAX_BI_DIGITS.
 */
void BI_ModMult (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, const BI_DIGIT *d, DWORD digits)
{
  BI_DIGIT t[2*MAX_BI_DIGITS];

  BI_Mult (t, b, c, digits);
  BI_Mod (a, t, 2 * digits, d, digits);
  
  /* Zeroize potentially sensitive information.
   */
  memset (t, 0, sizeof (t));
}


/* Computes a = b^c mod d.

   Lengths: a[dDigits], b[dDigits], c[cDigits], d[dDigits].
   Assumes b < d, d > 0, cDigits > 0, dDigits > 0,
           dDigits < MAX_BI_DIGITS.
 */
void BI_ModExp(BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD cDigits, const BI_DIGIT *d, DWORD dDigits)
{
  BI_DIGIT bPower[3][MAX_BI_DIGITS], ci, t[MAX_BI_DIGITS];
  unsigned int ciBits, j, s;
  register signed int i;

  /* Store b, b^2 mod d, and b^3 mod d.
   */
  BI_Assign(bPower[0], b, dDigits);
  BI_ModMult(bPower[1], bPower[0], b, d, dDigits);
  BI_ModMult(bPower[2], bPower[1], b, d, dDigits);
  
  BI_ASSIGN_DIGIT(t, 1, dDigits);

  cDigits = BI_Digits (c, cDigits);
  for (i = cDigits - 1; i >= 0; i--)
  {
    ci = c[i];
    ciBits = BI_DIGIT_BITS;
    
    /* Scan past leading zero bits of most significant digit.
     */
    if (i == (int)(cDigits - 1))
    {
      while (!DIGIT_2MSB(ci))
      {
        ci <<= 2;
        ciBits -= 2;
      }
    }

    for (j = 0; j < ciBits; j += 2, ci <<= 2)
    {
      /* Compute t = t^4 * b^s mod d, where s = two MSB's of d.
       */
      BI_ModMult (t, t, t, d, dDigits);
      BI_ModMult (t, t, t, d, dDigits);
      s = DIGIT_2MSB (ci);
      if (s)
        BI_ModMult (t, t, bPower[s-1], d, dDigits);
    }
  }
  
  BI_Assign(a, t, dDigits);
  
  /* Zeroize potentially sensitive information.
   */
  memset(bPower, 0, sizeof (bPower));
  memset(t, 0, sizeof (t));
}

/* Compute a = 1/b mod c, assuming inverse exists.
   
   Lengths: a[digits], b[digits], c[digits].
   Assumes gcd (b, c) = 1, digits < MAX_BI_DIGITS.
 */
void BI_ModInv (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits)
{
  BI_DIGIT q[MAX_BI_DIGITS], t1[MAX_BI_DIGITS], t3[MAX_BI_DIGITS],
    u1[MAX_BI_DIGITS], u3[MAX_BI_DIGITS], v1[MAX_BI_DIGITS],
    v3[MAX_BI_DIGITS], w[2*MAX_BI_DIGITS];
  int u1Sign;

  /* Apply extended Euclidean algorithm, modified to avoid negative
     numbers.
   */
  BI_ASSIGN_DIGIT(u1, 1, digits);
  BI_ASSIGN_ZERO(v1, digits);
  BI_Assign(u3, b, digits);
  BI_Assign(v3, c, digits);
  u1Sign = 1;

  while (!BI_Zero(v3, digits))
  {
    BI_Div(q, t3, u3, digits, v3, digits);
    BI_Mult(w, q, v1, digits);
    BI_Add(t1, u1, w, digits);
    BI_Assign(u1, v1, digits);
    BI_Assign(v1, t1, digits);
    BI_Assign(u3, v3, digits);
    BI_Assign(v3, t3, digits);
    u1Sign = -u1Sign;
  }
  
  /* Negate result if sign is negative.
    */
  if (u1Sign < 0)
    BI_Sub (a, c, u1, digits);
  else
    BI_Assign (a, u1, digits);

  /* Zeroize potentially sensitive information.
   */
  memset(q, 0, sizeof (q));
  memset(t1, 0, sizeof (t1));
  memset(t3, 0, sizeof (t3));
  memset(u1, 0, sizeof (u1));
  memset(u3, 0, sizeof (u3));
  memset(v1, 0, sizeof (v1));
  memset(v3, 0, sizeof (v3));
  memset(w, 0, sizeof (w));
}

/* Computes a = gcd(b, c).

   Lengths: a[digits], b[digits], c[digits].
   Assumes b > c, digits < MAX_BI_DIGITS.
 */
void BI_Gcd (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits)
{
  BI_DIGIT t[MAX_BI_DIGITS], u[MAX_BI_DIGITS], v[MAX_BI_DIGITS];

  BI_Assign (u, b, digits);
  BI_Assign (v, c, digits);

  while (! BI_Zero (v, digits)) {
    BI_Mod (t, u, digits, v, digits);
    BI_Assign (u, v, digits);
    BI_Assign (v, t, digits);
  }

  BI_Assign (a, u, digits);

  /* Zeroize potentially sensitive information.
   */
  memset (t, 0, sizeof (t));
  memset (u, 0, sizeof (u));
  memset (v, 0, sizeof (v));
}

/* Returns sign of a - b.

   Lengths: a[digits], b[digits].
 */
int BI_Cmp (const BI_DIGIT *a, const BI_DIGIT *b, DWORD digits)
{
  int i;
  
  for (i = digits - 1; i >= 0; i--) {
    if (a[i] > b[i])
      return (1);
    if (a[i] < b[i])
      return (-1);
  }

  return (0);
}

/* Returns nonzero iff a is zero.

   Lengths: a[digits].
 */
int BI_Zero (BI_DIGIT *a, DWORD digits)
{
  unsigned int i;
  
  for (i = 0; i < digits; i++)
    if (a[i])
      return (0);
    
  return (1);
}

int BI_One (BI_DIGIT *a, DWORD digits)
{
  unsigned int i;
  
  if (a[0] != 1)
    return (0);
  for (i = 1; i < digits; i++)
    if (a[i])
      return (0);
    
  return (1);
}

/* Returns the significant length of a in bits.

   Lengths: a[digits].
 */
DWORD BI_Bits(const BI_DIGIT *a, DWORD digits)
{
  digits = BI_Digits(a, digits);
  if (!digits)
    return 0;

  return ((digits - 1) * BI_DIGIT_BITS + BI_DigitBits(a[digits-1]));
}

/* Returns the significant length of a in digits.

   Lengths: a[digits].
 */
DWORD BI_Digits(const BI_DIGIT *a, DWORD digits)
{
  register signed int i;
  
  for (i = digits - 1; i >= 0; i--)
    if (a[i])
      break;

  return (i + 1);
}

/* Returns the significant length of a in bits, where a is a digit.
 */
__inline static unsigned int BI_DigitBits(BI_DIGIT a)
{
  register unsigned int i;
  
  for (i = 0; i < BI_DIGIT_BITS; i++, a >>= 1)
    if (a == 0)
      break;
    
  return (i);
}

/* Computes a = b * 2^c (i.e., shifts left c bits), returning carry.

   Lengths: a[digits], b[digits].
   Requires c < BI_DIGIT_BITS.
 */
BI_DIGIT BI_LShift (BI_DIGIT *a, const BI_DIGIT *b, DWORD c, DWORD digits)
{
  BI_DIGIT bi, carry;
  unsigned int i, t;
  
  if (c >= BI_DIGIT_BITS)
    return (0);
  
  t = BI_DIGIT_BITS - c;

  carry = 0;

  for (i = 0; i < digits; i++) {
    bi = b[i];
    a[i] = (bi << c) | carry;
    carry = c ? (bi >> t) : 0;
  }
  
  return (carry);
}

/* Computes a = c div 2^c (i.e., shifts right c bits), returning carry.

   Lengths: a[digits], b[digits].
   Requires: c < BI_DIGIT_BITS.
 */
BI_DIGIT BI_RShift (BI_DIGIT *a, const BI_DIGIT *b, DWORD c, DWORD digits)
{
  BI_DIGIT bi, carry;
  int i;
  unsigned int t;
  
  if (c >= BI_DIGIT_BITS)
    return (0);
  
  t = BI_DIGIT_BITS - c;

  carry = 0;

  for (i = digits - 1; i >= 0; i--) {
    bi = b[i];
    a[i] = (bi >> c) | carry;
    carry = c ? (bi << t) : 0;
  }
  
  return (carry);
}

/* Computes a = c div d and b = c mod d.

   Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
   Assumes d > 0, cDigits < 2 * MAX_BI_DIGITS,
           dDigits < MAX_BI_DIGITS.
 */
void BI_Div(BI_DIGIT *a, BI_DIGIT *b, const BI_DIGIT *c, DWORD cDigits, const BI_DIGIT *d, DWORD dDigits)
{
  BI_DIGIT cc[2*MAX_BI_DIGITS+1], dd[MAX_BI_DIGITS];
  BI_DIGIT t, ddDigits, shift;
  register signed int i;
  
  ddDigits = BI_Digits (d, dDigits);
  if (ddDigits == 0)
    return;
  
  /* Normalize operands.
   */
  shift = BI_DIGIT_BITS - BI_DigitBits (d[ddDigits-1]);
  BI_ASSIGN_ZERO (cc, ddDigits);
  cc[cDigits] = BI_LShift (cc, c, shift, cDigits);
  BI_LShift (dd, d, shift, ddDigits);
  t = dd[ddDigits-1];
  
  BI_ASSIGN_ZERO (a, cDigits);

  for (i = cDigits-ddDigits; i >= 0; i--)
  {
    BI_DIGIT ai;

    /* Underestimate quotient digit and subtract.
     */
    if (t == MAX_BI_DIGIT)
      ai = cc[i+dDigits];
    else
      BI_DigitDiv(&ai, &cc[i+ddDigits-1], t + 1);

    cc[i+ddDigits] -= BI_SubDigitMult(&cc[i], &cc[i], ai, dd, ddDigits);

    /* Correct estimate.
     */
    while (cc[i+ddDigits] || (BI_Cmp (&cc[i], dd, ddDigits) >= 0))
    {
      ai++;
      cc[i+ddDigits] -= BI_Sub(&cc[i], &cc[i], dd, ddDigits);
    }
    
    a[i] = ai;
  }
  
  /* Restore result.
   */
  BI_ASSIGN_ZERO (b, dDigits);
  BI_RShift (b, cc, shift, ddDigits);

  /* Zeroize potentially sensitive information.
   */
  memset (cc, 0, sizeof (cc));
  memset (dd, 0, sizeof (dd));
}

/* Computes a = b + c*d, where c is a digit. Returns carry.

   Lengths: a[digits], b[digits], d[digits].
 */
__inline static BI_DIGIT BI_AddDigitMult (BI_DIGIT *a, const BI_DIGIT *b, BI_DIGIT c, const BI_DIGIT *d, DWORD digits)
{
  BI_DIGIT t[2];
  register BI_DIGIT carry = 0;
  register unsigned int i;

  if (c)
  {
    for (i = 0; i < digits; i++)
    {
      BI_DigitMult(t, c, d[i]);
      a[i] = b[i] + carry;
      carry = (a[i] < carry) ? 1 : 0;
      a[i] += t[0];
      if (a[i] < t[0])
        carry++;
      carry += t[1];
    }
  }
  
  return carry;
}

/* Computes a = b - c*d, where c is a digit. Returns borrow.

   Lengths: a[digits], b[digits], d[digits].
 */
__inline static BI_DIGIT BI_SubDigitMult (BI_DIGIT *a, const BI_DIGIT *b, BI_DIGIT c, const BI_DIGIT *d, DWORD digits)
{
  BI_DIGIT borrow, t[2];
  unsigned int i;

  if (c == 0)
    return (0);

  borrow = 0;
  for (i = 0; i < digits; i++) {
    BI_DigitMult (t, c, d[i]);
    if ((a[i] = b[i] - borrow) > (MAX_BI_DIGIT - borrow))
      borrow = 1;
    else
      borrow = 0;
    if ((a[i] -= t[0]) > (MAX_BI_DIGIT - t[0]))
      borrow++;
    borrow += t[1];
  }
  
  return (borrow);
}


/* Computes a = b * c, where b and c are digits.

   Lengths: a[2].
 */
__inline static void BI_DigitMult(BI_DIGIT a[2], BI_DIGIT b, BI_DIGIT c)
{
  register BI_DIGIT t, u;
  register BI_DIGIT bHigh, bLow, cHigh, cLow;

  bHigh = HIGH_HALF (b);
  bLow  = LOW_HALF (b);
  cHigh = HIGH_HALF (c);
  cLow  = LOW_HALF (c);

  a[0] = bLow * cLow;
  t = bLow * cHigh;
  u = bHigh * cLow;
  a[1] = bHigh * cHigh;
  
  t += u;
  if (t < u)
    a[1] += TO_HIGH_HALF(1);
  u = TO_HIGH_HALF(t);
  
  a[0] += u;
  if (a[0] < u)
    a[1]++;
  a[1] += HIGH_HALF (t);
}

/* Sets a = b / c, where a and c are digits.

   Lengths: b[2].
   Assumes b[1] < c and HIGH_HALF (c) > 0. For efficiency, c should be
   normalized.
 */
__inline static void BI_DigitDiv(BI_DIGIT *a, BI_DIGIT b[2], BI_DIGIT c)
{
  register BI_DIGIT t0;
  register BI_DIGIT t1;
  register BI_DIGIT aHigh, aLow, cHigh, cLow;

  cHigh = HIGH_HALF(c);
  cLow  = LOW_HALF(c);

  t0 = b[0];
  t1 = b[1];

  /* Underestimate high half of quotient and subtract.
   */
  if (cHigh == MAX_BI_HALF_DIGIT)
    aHigh = HIGH_HALF(t1);
  else
    aHigh = t1 / (cHigh + 1);

  {
    register BI_DIGIT u;
    register BI_DIGIT v;

    u = aHigh * cLow;
    v = aHigh * cHigh;
    t0 -= TO_HIGH_HALF(u);
    if (t0 > (MAX_BI_DIGIT - TO_HIGH_HALF(u)))
      t1--;
    t1 -= HIGH_HALF(u);
    t1 -= v;
  }

  /* Correct estimate.
   */
  while ((t1 > cHigh) || ((t1 == cHigh) && (t0 >= TO_HIGH_HALF(cLow))))
  {
    t0 -= TO_HIGH_HALF(cLow);
    if (t0 > MAX_BI_DIGIT - TO_HIGH_HALF(cLow))
      t1--;
    t1 -= cHigh;
    aHigh++;
  }

  /* Underestimate low half of quotient and subtract.
   */
  if (cHigh == MAX_BI_HALF_DIGIT)
    aLow = LOW_HALF(t1);
  else
    aLow = (TO_HIGH_HALF(t1) + HIGH_HALF(t0)) / (cHigh + 1);

  {
    register BI_DIGIT u;
    register BI_DIGIT v;

    u = aLow * cLow;
    v = aLow * cHigh;

    t0 -= u;
    if (t0 > (MAX_BI_DIGIT - u))
      t1--;
    t0 -= TO_HIGH_HALF (v);
    if (t0 > (MAX_BI_DIGIT - TO_HIGH_HALF(v)))
      t1--;
    t1 -= HIGH_HALF(v);
  }

  /* Correct estimate.
   */
  while ((t1 > 0) || ((t1 == 0) && t0 >= c))
  {
    t0 -= c;
    if (t0 > (MAX_BI_DIGIT - c))
      t1--;
    aLow++;
  }
  
  *a = TO_HIGH_HALF (aHigh) + aLow;
}


/*
  j := 1
  while (a not 0) do {
    while (a even) do {
      a := a/2
      if (n = 3 (mod 8) or n = 5 (mod 8)) then j := -j
    }
    interchange(a,n)
    if (a = 3 (mod 4) and n = 3 (mod 4)) then j := -j
    a := a mod n
  }
  if (n = 1) then return (j) else return(0)
*/

int BI_Jacobi(const BI_DIGIT *a, const BI_DIGIT *n, DWORD digits)
{
  BI_DIGIT T1[MAX_BI_DIGITS], T2[MAX_BI_DIGITS];
  BI_DIGIT *A, *N, *T;
  register signed int j = 1;

  A = T1;
  N = T2;

  BI_ASSIGN_ZERO(A, MAX_BI_DIGITS);
  BI_Assign(A, a, digits);
  BI_ASSIGN_ZERO(N, MAX_BI_DIGITS);
  BI_Assign(N, n, digits);

  while (!BI_Zero(A, digits))
  {
    while (BI_EVEN(A, digits))
    {
      BI_RShift(A, A, 1, digits);
      if ((BI_MOD_DIGIT(N, 8, digits) == 3) || (BI_MOD_DIGIT(N, 8, digits) == 5)) j = 0 - j;
    }
    T = A; A = N; N = T;
    if ((BI_MOD_DIGIT(A, 4, digits) == 3) || (BI_MOD_DIGIT(N, 4, digits) == 3)) j = 0 - j;
    BI_Mod(A, A, digits, N, digits);
  }

  if (BI_One(N, digits)) return j;

  return 0;
}

