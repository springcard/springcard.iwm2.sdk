/* CONVERSIONS
   BI_Decode (a, digits, b, len)   Decodes character string b into a.
   BI_Encode (a, len, b, digits)   Encodes a into character string b.

   ASSIGNMENTS
   BI_Assign (a, b, digits)        Assigns a = b.
   BI_ASSIGN_DIGIT (a, b, digits)  Assigns a = b, where b is a digit.
   BI_AssignZero (a, b, digits)    Assigns a = 0.
   BI_Assign2Exp (a, b, digits)    Assigns a = 2^b.
     
   ARITHMETIC OPERATIONS
   BI_Add (a, b, c, digits)        Computes a = b + c.
   BI_Sub (a, b, c, digits)        Computes a = b - c.
   BI_Mult (a, b, c, digits)       Computes a = b * c.
   BI_LShift (a, b, c, digits)     Computes a = b * 2^c.
   BI_RShift (a, b, c, digits)     Computes a = b / 2^c.
   BI_Div (a, b, c, cDigits, d, dDigits)  Computes a = c div d and b = c mod d.

   NUMBER THEORY
   BI_Mod (a, b, bDigits, c, cDigits)  Computes a = b mod c.
   BI_ModMult (a, b, c, d, digits) Computes a = b * c mod d.
   BI_ModExp (a, b, c, cDigits, d, dDigits)  Computes a = b^c mod d.
   BI_ModInv (a, b, c, digits)     Computes a = 1/b mod c.
   BI_Gcd (a, b, c, digits)        Computes a = gcd (b, c).

   OTHER OPERATIONS
   BI_EVEN (a, digits)             Returns 1 iff a is even.
   BI_Cmp (a, b, digits)           Returns sign of a - b.
   BI_EQUAL (a, digits)            Returns 1 iff a = b.
   BI_Zero (a, digits)             Returns 1 iff a = 0.
   BI_Digits (a, digits)           Returns significant length of a in digits.
   BI_Bits (a, digits)             Returns significant length of a in bits.
 */

#ifndef __BINUM_H__
#define __BINUM_H__

#include "lib-c/types.h"

/* Type definitions.
 */
typedef DWORD BI_DIGIT;
typedef WORD  BI_HALF_DIGIT;

/* Length of digit in bits */
#define BI_DIGIT_BITS (8*sizeof(BI_DIGIT))
#define BI_HALF_DIGIT_BITS (8*sizeof(BI_HALF_DIGIT))
/* Length of digit in bytes */
#define BI_DIGIT_LEN (BI_DIGIT_BITS/8)
/* Maximum length in digits */
#define MAX_BI_DIGITS (2048/BI_DIGIT_BITS)
/* Maximum digits */
#define MAX_BI_DIGIT 0xFFFFFFFF
#define MAX_BI_HALF_DIGIT 0xFFFF

/* Macros.
 */
#define LOW_HALF(x) ((x) & MAX_BI_HALF_DIGIT)
#define HIGH_HALF(x) (((x) >> BI_HALF_DIGIT_BITS) & MAX_BI_HALF_DIGIT)
#define TO_HIGH_HALF(x) (((BI_DIGIT)(x)) << BI_HALF_DIGIT_BITS)
#define DIGIT_MSB(x) (unsigned int)(((x) >> (BI_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x) (unsigned int)(((x) >> (BI_DIGIT_BITS - 2)) & 3)

#define BI_ASSIGN_ZERO(a, digits) memset(a,0,digits*sizeof(BI_DIGIT))
#define BI_ASSIGN_DIGIT(a, b, digits) {BI_ASSIGN_ZERO(a,digits);a[0]=b;}
#define BI_EQUAL(a,b, digits) (!BI_Cmp (a, b, digits))
#define BI_EVEN(a,digits) (((digits)==0)||!(a[0]&1))
#define BI_ODD(a,digits) (((digits)==0)||(a[0]&1))
#define BI_MOD_DIGIT(a,b,digits) a[0]%b


void BI_Decode (BI_DIGIT *a, DWORD digits, const BYTE *b, DWORD len);
void BI_Encode (BYTE *a, DWORD len, const BI_DIGIT *b, DWORD digits);
void BI_Assign (BI_DIGIT *a, const BI_DIGIT *b, DWORD digits);
void BI_Assign2Exp (BI_DIGIT *a, DWORD b, DWORD digits);
BI_DIGIT BI_Add (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits);
BI_DIGIT BI_Sub (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits);
void BI_Mult (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits);
void BI_Mod (BI_DIGIT *a, const BI_DIGIT *b, DWORD bDigits, const BI_DIGIT *c, DWORD cDigits);
void BI_ModMult (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, const BI_DIGIT *d, DWORD digits);
void BI_ModExp (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD cDigits, const BI_DIGIT *d, DWORD dDigits);
void BI_ModInv (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits);
void BI_Gcd (BI_DIGIT *a, const BI_DIGIT *b, const BI_DIGIT *c, DWORD digits);
int BI_Cmp (const BI_DIGIT *a, const BI_DIGIT *b, DWORD digits);
int BI_Zero (BI_DIGIT *a, DWORD digits);
DWORD BI_Bits (const BI_DIGIT *a, DWORD digits);
DWORD BI_Digits (const BI_DIGIT *a, DWORD digits);
BI_DIGIT BI_LShift (BI_DIGIT *a, const BI_DIGIT *b, DWORD c, DWORD digits);
BI_DIGIT BI_RShift (BI_DIGIT *a, const BI_DIGIT *b, DWORD c, DWORD digits);
void BI_Div (BI_DIGIT *a, BI_DIGIT *b, const BI_DIGIT *c, DWORD cDigits, const BI_DIGIT *d, DWORD dDigits);






int BI_One(BI_DIGIT *a, DWORD digits);
int BI_SupOne(BI_DIGIT *a, DWORD digits);
int BI_Jacobi(const BI_DIGIT *a, const BI_DIGIT *n, DWORD digits);


void BI_Disp(char *msg, BI_DIGIT *t, DWORD digits);

#endif
