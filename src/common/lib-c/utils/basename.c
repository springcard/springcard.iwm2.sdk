/* basename.c -- return the last element in a path
   Copyright (C) 1990, 1998, 1999, 2000, 2001 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdlib.h>
#include <string.h>

#include "dirname.h"

char *basename (char const *name)
{
  char const *base = name + FILESYSTEM_PREFIX_LEN (name);
  char const *p;

  for (p = base; *p; p++)
  {
    if (ISSLASH (*p))
	  {
	    /* Treat multiple adjacent slashes like a single slash.  */
	    do p++;
	    while (ISSLASH (*p));

	    /* If the file name ends in slash, use the trailing slash as
	       the basename if no non-slashes have been found.  */
	    if (! *p)
	    {
	      if (ISSLASH (*base))
		      base = p - 1;
	      break;
	    }

	    /* *P is a non-slash preceded by a slash.  */
	    base = p;
	  }
  }

  return (char *) base;
}

/* Return the length of of the basename NAME.  Typically NAME is the
   value returned by base_name.  Act like strlen (NAME), except omit
   redundant trailing slashes.  */

size_t baselen(char const *name)
{
  size_t len;

  for (len = strlen (name);  1 < len && ISSLASH (name[len - 1]);  len--)
    continue;

  return len;
}
