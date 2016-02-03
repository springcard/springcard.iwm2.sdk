#include <stdio.h>
#include <string.h>
#include "strl.h"

#if (defined(__MISSING_STRL) || defined(__MISSING_STRLCPY))

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
*/
size_t strlcpy(char *dst, const char *src, size_t dstsize)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = dstsize;
	/* Copy as many bytes as will fit */
	if (n != 0 && --n != 0) {
	do {
		if ((*d++ = *s++) == 0)
			break;
		} while (--n != 0);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (dstsize != 0)
			*d = '\0';      /* NUL-terminate dst */
		while (*s++);
	}

	return(s - src - 1);    /* count does not include NUL */
}

#endif

#if (defined(__MISSING_STRL) || defined(__MISSING_STRLCAT))

size_t strlcat(char *dst, const char *src, size_t dstsize)
{
	char *df = dst;
  size_t left = dstsize;
  size_t l1;
  size_t l2 = strlen(src);
  size_t copied;

	while (left-- != 0 && *df != '\0')
  	df++;
  l1 = df - dst;
  if (dstsize == l1)
  	return (l1 + l2);

  copied = l1 + l2 >= dstsize ? dstsize - l1 - 1 : l2;
  (void) memcpy(dst + l1, src, copied);
  dst[l1+copied] = '\0';
  return (l1 + l2);
}

#endif

#if (defined(__MISSING_STRTOK_R))

char *strtok_r(char *string, const char *sepset, char **lasts)
{
  char	*q, *r;

  /* first or subsequent call */
  if (string == NULL)
    string = *lasts;

  if (string == NULL)		/* return if no tokens remaining */
    return (NULL);

  q = string + strspn(string, sepset);	/* skip leading separators */

  if (*q == '\0')		/* return if no tokens remaining */
    return (NULL);

  if ((r = strpbrk(q, sepset)) == NULL)	/* move past token */
    *lasts = NULL;	/* indicate this is last token */
  else {
    *r = '\0';
    *lasts = r + 1;
  }
  return (q);
}

#endif
