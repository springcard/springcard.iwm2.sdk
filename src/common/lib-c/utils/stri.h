#ifndef __UTILS_STRI_H__
#define __UTILS_STRI_H__

#include <stdlib.h>

char *sc_strdup(const char *str);
char *sc_stristr(const char *str, const char *sub_str);
int sc_stricmp(const char *str1, const char *str2);
int sc_strnicmp(const char *str1, const char *str2, size_t max_len);

#endif
