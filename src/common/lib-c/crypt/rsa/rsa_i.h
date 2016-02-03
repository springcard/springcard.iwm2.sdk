#ifndef __RSA_I_H__
#define __RSA_I_H__

#include "lib-c/types.h"
#include "lib-c/crypt/utils/bignum.h"

void FindRSAPrime (BI_DIGIT *a, unsigned int b, BI_DIGIT *c, unsigned int cDigits, BI_DIGIT *d, unsigned int dDigits);
BOOL GeneratePrime(BI_DIGIT *a, BI_DIGIT *b, BI_DIGIT *c, BI_DIGIT *d, DWORD digits);
int ProbablePrime (BI_DIGIT *a, unsigned int aDigits);

/*
int RSAPrivateExponent2 (BI_DIGIT *s,
                          DWORD nDigits,
                          BI_DIGIT *p,
                          BI_DIGIT *q,
                          DWORD pDigits);


int RSAPublicBlockCore (BI_DIGIT *p,
                        BI_DIGIT *s,
                        BI_DIGIT e,
                        BI_DIGIT *n,
                        DWORD nDigits);

int RSAPrivateBlockCore2 (BI_DIGIT *s,
                          BI_DIGIT *m,
                          BI_DIGIT *k,
                          BI_DIGIT *n,
                          DWORD nDigits);

int RSAPrivateBlockCore (BI_DIGIT *s,
                         BI_DIGIT *m,
                         BI_DIGIT *p,
                         BI_DIGIT *q,
                         BI_DIGIT *qInv,
                         BI_DIGIT *dP,
                         BI_DIGIT *dQ,
                         DWORD nDigits,
                         DWORD pDigits);
*/
#include "rsa.h"

#endif
