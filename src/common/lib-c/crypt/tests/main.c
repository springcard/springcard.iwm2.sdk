#include "lib-c/types.h"

#include "lib-c/crypt/hash/md5.h"
#include "lib-c/crypt/hash/hmac_md5.h"
#include "lib-c/crypt/hash/sha1.h"
#include "lib-c/crypt/hash/hmac_sha1.h"

#include "lib-c/crypt/block/des.h"
#include "lib-c/crypt/block/aes.h"
#include "lib-c/crypt/block/blowfish.h"

#include "lib-c/crypt/rsa/rsa.h"
#include "lib-c/crypt/rsa/rabin.h"

#include "lib-c/crypt/vigik/iso9796.h"


#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  DWORD tests = 0x00000003;

  if (tests & 0x00000001)
  {
    printf("Validation MD5...\n");
    if (!MD5_SelfTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
    printf("Validation HMAC MD5...\n");
    if (!HMAC_MD5_SelfTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
  }

  if (tests & 0x00000002)
  {
    printf("Validation SHA1...\n");
    if (!SHA1_SelfTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
    printf("Validation HMAC SHA1...\n");
    if (!HMAC_SHA1_SelfTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
  }

  if (tests & 0x00000010)
  {
    printf("Validation DES...\n");
    if (!DES_SelfTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
  }

  if (tests & 0x00000020)
  {
    printf("Validation AES (Rijndael)...\n");
    if (!AES_SeltTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
  }

  if (tests & 0x00000040)
  {
    printf("Validation Blowfish...\n");
    if (!Blowfish_SelfTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
  }

  if (tests & 0x00000100)
  {
    printf("Validation RABIN...\n");
    if (!RABIN_SelfTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
  }

  if (tests & 0x00000200)
  {
    printf("Validation ISO9796...\n");
    if (!ISO9796_SelfTest())
    {
      printf("ECHEC !!!\n");
      return EXIT_FAILURE;
    }
  }

  printf("Succes !\n");
  return EXIT_SUCCESS;
}
