/*
 *  common/lib-c/utils/verbose, verbose and log utility
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

/*
 * C lib
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

/*
 * SpringCard includes
 */
#include "../types.h"
#include "verbose.h"

#define MAX_LOG_BUFFER_SIZE 256

unsigned char verbose_level = 6;

static char sbuffer[200];
static char tbuffer[50];
  
static void _Verbose1(BYTE level)
{
  time_t tt;
  struct tm *t;
  tt = time(NULL);
  t = localtime(&tt); 
  sprintf(tbuffer, "%02d/%02d/%02d %02d:%02d:%02d %d", t->tm_mday,
                                                        t->tm_mon+1,
                                                        t->tm_year - 100,
                                                        t->tm_hour,
                                                        t->tm_min,
                                                        t->tm_sec,
                                                        level);   
}

static void _Verbose2(void)
{
  fprintf(stderr, "%s %s\r\n", tbuffer, sbuffer);
  fflush(stderr);
}

void _Verbose(BYTE level, const char *fmt, ...)
{ 
  va_list arg_ptr;

  if (verbose_level < level) return;

  _Verbose1(level);
  va_start(arg_ptr, fmt);
  vsprintf(sbuffer, fmt, arg_ptr);
  va_end(arg_ptr);
  _Verbose2();
}

void _VerboseError(BYTE level, int errcode, const char *fmt, ...)
{
  va_list arg_ptr;

  if (verbose_level < level) return;

  _Verbose1(level);
  va_start(arg_ptr, fmt);
  vsprintf(sbuffer, fmt, arg_ptr);
  va_end(arg_ptr);
  _Verbose2();
  
  sprintf("\t%s", strerror(errcode));
  _Verbose2();
}

void _VerbosePError(BYTE level, const char *fmt, ...)
{
  va_list arg_ptr;

  if (verbose_level < level)
  {
    errno = 0;
    return;
  }

  _Verbose1(level);
  va_start(arg_ptr, fmt);
  vsprintf(sbuffer, fmt, arg_ptr);
  va_end(arg_ptr);
  _Verbose2();
  
  sprintf("\t%s", strerror(errno));
  _Verbose2();
  errno = 0;
}
