#include "stack.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static BI_DIGIT *stack_last   = NULL;
static BI_DIGIT *stack_buffer = NULL;
static DWORD     stack_offset = 0;
static DWORD     stack_max_used = 0;
static DWORD     stack_max_size = 32*1024;

static void stack_cleanup(void)
{
  if (stack_buffer == NULL) return;
  if (stack_offset)
    printf("Warning: stack not empty\n");
  printf("max used stack : %ld\n", stack_max_used);
  printf("freeing stack\n");
  free(stack_buffer);
  stack_buffer = NULL;
}

static void stack_init(void)
{
  stack_offset   = 0;
  stack_max_used = 0;
  if (stack_buffer != NULL) free(stack_buffer);
  stack_buffer = calloc(sizeof(BI_DIGIT), stack_max_size);
  if (stack_buffer == NULL) return;
  atexit(stack_cleanup);
}

static void stack_dump(void)
{
  DWORD i;

  while (stack_max_used % 8) stack_max_used++;

  for (i=0; i<stack_max_used; i++)
  {
    printf("%08lX ", stack_buffer[i]);
    if (i % 8 == 7) printf("\n");
  }
}

BI_DIGIT *BI_New(DWORD digits)
{
  if (stack_buffer == NULL)
  {
    stack_init();
    if (stack_buffer == NULL)
    {
      printf("System memory exhausted\n");
      exit(EXIT_FAILURE);
    }
  }

  if ((stack_offset + digits + 1) >= stack_max_size)
  {
    printf("Allocated stack exhausted\n");
    exit(EXIT_FAILURE);
  }

#ifdef _DEBUG
  /* Place un marqueur de debut */
  stack_buffer[stack_offset] = 0xDEADDEAD;
  stack_offset++;
#endif

  /* Recupere l'adresse du prochain bloc */
  stack_last = &stack_buffer[stack_offset];

  /* Le bloc doit etre initialise a 0 */
  memset(stack_last, 0x00, digits * sizeof(BI_DIGIT));

  /* Se positionne a la fin du bloc */
  stack_offset += digits;

#ifdef _DEBUG
  /* Place un marqueur de fin */
  stack_buffer[stack_offset] = 0xBEEFBEEF;
  stack_offset++;
#endif

  /* Stocke la longueur */
  stack_buffer[stack_offset] = digits;

  /* Se place au debut du bloc suivant */
  stack_offset++;
#ifdef _DEBUG
  stack_buffer[stack_offset] = 0xCCCCCCCC;
#endif

  /* N'oublie pas les statistiques */
  if (stack_offset > stack_max_used)
    stack_max_used = stack_offset;

//  printf("Allocated %ld -> %p\n", digits, stack_last);
  return stack_last;
}

void BI_Free(BI_DIGIT *t)
{
  DWORD digits;
  if (t == NULL) return;

  /* Verifie qu'on veut bien liberer le dernier bloc alloue */
  if (t != stack_last)
  {
    printf("Stack release error\n");
#ifdef _DEBUG
    stack_dump();
#endif
    exit(EXIT_FAILURE);
  }

  /* Revient en arriere dans la pile pour retrouver la taille */
  stack_offset--;
  digits = stack_buffer[stack_offset];

  if (stack_offset - digits < 0)
  {
    printf("Stack content error\n");
#ifdef _DEBUG
    stack_dump();
#endif
    exit(EXIT_FAILURE);
  }

  /* Efface le bloc par securite */
  memset(stack_last, 0xFF, digits * sizeof(BI_DIGIT));

#ifdef _DEBUG
  /* Verifie le marqueur de fin */
  stack_offset--;
  if (stack_buffer[stack_offset] != 0xBEEFBEEF)
  {
    printf("Stack end marker error\n");
    stack_dump();
    exit(EXIT_FAILURE);
  }
#endif

  /* Se repositionne au debut du bloc */
  stack_offset -= digits;

#ifdef _DEBUG
  /* Verifie le marqueur de debut */
  stack_offset--;
  if (stack_buffer[stack_offset] != 0xDEADDEAD)
  {
    printf("Stack startup marker error\n");
    stack_dump();
    exit(EXIT_FAILURE);
  }
#endif

//  printf("Freed %ld <- %p ", digits, t);

  if (stack_offset > 1)
  {
    /* Reste a retrouver l'adresse du bloc precedent */
    digits = stack_buffer[stack_offset - 1];
#ifdef _DEBUG
    stack_last = &stack_buffer[stack_offset - digits - 2];
#else
    stack_last = &stack_buffer[stack_offset - digits - 1];
#endif
  } else
  {
    stack_last = &stack_buffer[0];
  }

//  printf(" -> %p\n", stack_last);
}


void *New(DWORD size)
{
  while (size % sizeof(BI_DIGIT)) size ++;
  return BI_New(size / sizeof(BI_DIGIT));
}

void Free(void *t)
{
  BI_Free(t);
}
