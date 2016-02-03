#ifndef __GETTIMEOFDAY_H__
#define __GETTIMEOFDAY_H__

#ifdef WIN32
#include <windows.h>
int gettimeofday(struct timeval* p, void* tz);
#endif

#endif