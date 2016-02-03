#ifndef __STACK_H__
#define __STACK_H__

#include "lib-c/crypt/utils/bignum.h"

void *New(DWORD size);
void Free(void *t);

BI_DIGIT *BI_New(DWORD digits);
void BI_Free(BI_DIGIT *t);

#endif
