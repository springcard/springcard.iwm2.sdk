#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "stri.h"

char *sc_strdup(const char *String)
{
  int l;
  char *p;

  if (String == NULL)
    return NULL;

  l = strlen(String);

  p = malloc(l + 1);
  if (p != NULL)
  {
    memcpy(p, String, l);
    p[l] = '\0';
  }

  return p;
}

char *sc_stristr(const char *String, const char *Pattern)
{
      char *pptr, *sptr, *start;
      size_t  slen, plen;

      for (start = (char *)String,
           pptr  = (char *)Pattern,
           slen  = strlen(String),
           plen  = strlen(Pattern);

           /* while string length not shorter than pattern length */

           slen >= plen;

           start++, slen--)
      {
            /* find start of pattern in string */
            while (toupper(*start) != toupper(*Pattern))
            {
                  start++;
                  slen--;

                  /* if pattern longer than string */

                  if (slen < plen)
                        return(NULL);
            }

            sptr = start;
            pptr = (char *)Pattern;

            while (toupper(*sptr) == toupper(*pptr))
            {
                  sptr++;
                  pptr++;

                  /* if end of pattern then pattern was found */

                  if ('\0' == *pptr)
                        return (start);
            }
      }
      return(NULL);
}

int sc_stricmp(const char *String1, const char *String2)
{
  char *s1 = (char *) String1;
  char *s2 = (char *) String2;

  for (; toupper(*s1) == toupper(*s2); s1++, s2++)
    if (*s1 == '\0')
      return 0;

  return (*s1 < *s2) ? -1 : 1;
}

int sc_strnicmp(const char *String1, const char *String2, size_t Length)
{
  char *s1 = (char *) String1;
  char *s2 = (char *) String2;

  for (; (toupper(*s1) == toupper(*s2)) && (Length > 0); s1++, s2++, Length--)
    if (*s1 == '\0')
      return 0;

  if (Length == 0)
    return 0;

  return (*s1 < *s2) ? -1 : 1;
}
