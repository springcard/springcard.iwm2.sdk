#ifndef __UTILS_STRINGS_H__
#define __UTILS_STRINGS_H__

#include <stdlib.h>

#ifdef WIN32
  #ifndef __MISSING_STRTOK_R
    #define __MISSING_STRTOK_R
  #endif
  #ifndef __MISSING_STRL
    #define __MISSING_STRL
  #endif
#endif

#ifdef __MISSING_STRTOK_R
char *strtok_r(char *str, const char *delim, char **saveptr);
#endif

#ifdef __MISSING_STRL
size_t strlcpy(char *dst, const char *src, size_t dstsize);
size_t strlcat(char *dst, const char *src, size_t dstsize);
#endif

#endif
