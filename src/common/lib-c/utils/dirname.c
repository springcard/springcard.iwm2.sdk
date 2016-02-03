#include <stdlib.h>
#include <string.h>

#include "dirname.h"

/* Return the length of `dirname (PATH)', or zero if PATH is
   in the working directory.  Works properly even if
   there are trailing slashes (by effectively ignoring them).  */
size_t ut_dirlen (char const *path)
{
  size_t prefix_length = FILESYSTEM_PREFIX_LEN (path);
  size_t length;

  /* Strip the basename and any redundant slashes before it.  */
  for (length = basename (path) - path;  prefix_length < length;  length--)
    if (! ISSLASH (path[length - 1]))
      return length;

  /* But don't strip the only slash from "/".  */
  return prefix_length + ISSLASH (path[prefix_length]);
}

/* Return the leading directories part of PATH,
   allocated with xmalloc.
   Works properly even if there are trailing slashes
   (by effectively ignoring them).  */

char *ut_dirname (char const *path)
{
  size_t length = ut_dirlen (path);
  int append_dot = (length == FILESYSTEM_PREFIX_LEN (path));
  char *newpath = malloc (length + append_dot + 1);
  memcpy (newpath, path, length);
  if (append_dot)
    newpath[length++] = '.';
  newpath[length] = 0;
  return newpath;
}

