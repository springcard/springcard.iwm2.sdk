#ifndef __DIRNAME_H__
#define __DIRNAME_H__

#include <stdio.h>

# ifndef DIRECTORY_SEPARATOR
#   ifdef WIN32
#     define DIRECTORY_SEPARATOR '\\'
#   else
#     define DIRECTORY_SEPARATOR '/'
#   endif
# endif

# ifndef ISSLASH
#  define ISSLASH(C) (((C) == '/') || ((C) == '\\'))
# endif

# ifndef FILESYSTEM_PREFIX_LEN
#  define FILESYSTEM_PREFIX_LEN(Filename) 0
# endif

char *basename(char const *path);
char *dirname(char const *path);
size_t baselen(char const *path);
size_t dirlen(char const *path);
int strip_trailing_slashes(char *path);

#endif /* not DIRNAME_H_ */
