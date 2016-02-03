#include "lib-c/utils/types.h"
#include "lib-c/crypt/block/des.h"
#include "lib-c/crypt/hash/md5.h"
#include "random.h"

#ifdef WIN32


#include <wincrypt.h>
#include <windows.h>

static void RAND_GenerateKey(BYTE key[16])
{
  MD5_CTX_ST md5_ctx; 
  HINSTANCE pid;

  GetSystemTimeAsFileTime((FILETIME *) &key[0]);

  pid = GetModuleHandle(NULL);
  memcpy(&key[8], &pid, 4);
  /* On laisse volontairement 4 octets non definis pour profiter naivement de l'entropie de la pile (s'il y en a...) */

  MD5_Init(&md5_ctx);
  MD5_Update(&md5_ctx, key, 16);
  MD5_Final(&md5_ctx, key);
}

#else

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

static void RAND_GenerateKey(BYTE key[16])
{
  MD5_CTX_ST md5_ctx; 
  pid_t pid;

	gettimeofday((struct timeval *) &key[0], NULL);

  pid = getpid();
  memcpy(&key[8], &pid, 4);
  /* On laisse volontairement 4 octets non definis pour profiter naivement de l'entropie de la pile (s'il y en a...) */  
  
  MD5_Init(&md5_ctx);
  MD5_Update(&md5_ctx, key, 16);
  MD5_Final(&md5_ctx, key);
}

#endif

/*
ANSI X9.31 ALGORITHM:
Given

    * D, a 64-bit representation of the current date-time
    * S, a secret 64-bit seed that will be updated by this process
    * K, a secret (Triple DES) key

Step 1. Compute the 64-bit block X = G(S, K, D) as follows:

   1. I = E(D, K)
   2. X = E(I XOR S, K)
   3. S' = E(X XOR I, K)

where E(p, K) is the (Triple DES) encryption of the 64-bit block p using key K.

Step 2. Return X and set S = S' for the next cycle. 
*/


/* CAUTION: We use a static structure to store our values in. */
static struct
{
	BOOL        seeded;
	BYTE        seed[8];
	TDES_CTX_ST tdes_ctx;
} rand_ctx;

void RAND_Init(DWORD seed)
{
  /* Genere une cle aleatoire */
  BYTE k[16];
  RAND_GenerateKey(k);

  /* Initialise la cle 3DES */
  TDES_Init(&rand_ctx.tdes_ctx, &k[0], &k[8], &k[0]);

  /* Initialise la graine */
  memcpy(rand_ctx.seed, &seed, 4);
}

void RAND_GetBuffer(BYTE buffer[8])
{
	BYTE I[8], X[8];
  int i;

  if (buffer == NULL) return;

	/* Set seed if not already seeded */
	if (!rand_ctx.seeded)
	{
		RAND_Init((DWORD) time(NULL));
	  rand_ctx.seeded = TRUE;
	}

	/* I = E(D, K) */
#ifdef WIN32
  GetSystemTimeAsFileTime((FILETIME *) I);
#endif
#ifdef unix
  gettimeofday((struct timeval *) I, NULL);
#endif
	
  TDES_Encrypt(&rand_ctx.tdes_ctx, I);

	/* X = E(I XOR S, K) */
  for (i=0; i<8; i++)
    X[i] = I[i] ^ rand_ctx.seed[i];
  TDES_Encrypt(&rand_ctx.tdes_ctx, X);

  /* return X */
  memcpy(buffer, X, 8);

	/* S' = E(X XOR I, K) */
  for (i=0; i<8; i++)
    rand_ctx.seed[i] = I[i] ^ X[i];
  TDES_Encrypt(&rand_ctx.tdes_ctx, rand_ctx.seed);
}

void RAND_GetBytes(BYTE buffer[], DWORD size)
{
  BYTE *p;

  p = buffer;
  while (size > 8)
  {
    RAND_GetBuffer(p);
    p += 8;
    size -= 8;
  }

  if (size)
  {
    BYTE t[8];
    RAND_GetBuffer(t);
    memcpy(p, t, size);
  }
}

DWORD RAND_GetDWord(void)
{
	BYTE buffer[8];
  DWORD rc;

  RAND_GetBuffer(buffer);
  memcpy(&rc, buffer, sizeof(DWORD));
  return rc;
}


