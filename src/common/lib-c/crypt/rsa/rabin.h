#ifndef __RABIN_H__
#define __RABIN_H__

#include "rsa.h"

/*
 * Compute s as 2.s - 1 = 1/2 GCM(p-1;q-1)
 * ---------------------------------------
 */
BOOL RABIN_ComputePrivateKey_BI(BI_DIGIT *s,
                                DWORD nDigits,
                                BI_DIGIT *p,
                                BI_DIGIT *q,
                                DWORD pqDigits);

BOOL RABIN_BuildPrivateKey_BI(RSA_PRIVATE_KEY_ST *privateKey,
                              BI_DIGIT *p,
															DWORD pDigits,
                              BI_DIGIT *q,
															DWORD qDigits,
                              DWORD nDigits,
                              DWORD nBits);

/*
 * Crible pour valider que p et q sont utilisables en Rabin
 * --------------------------------------------------------
 */
BOOL RABIN_ValidatePrimes_BI(BI_DIGIT *p,
                             DWORD pDigits,
                             BI_DIGIT *q,
														 DWORD qDigits,
                             DWORD nDigits,
                             DWORD nBits);

/*
 * Generation de P et Q utilisables en Rabin
 * -----------------------------------------
 */
BOOL RABIN_GeneratePrimes_BI(BI_DIGIT *p,
                             BI_DIGIT *q,
                             DWORD nDigits,
                             DWORD nBits);



BOOL RABIN_SelfTest(void);

#endif
