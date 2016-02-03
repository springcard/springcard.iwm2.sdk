/*
 *  common/lib-c/utils/verbose, verbose and log utility
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

#ifndef __VERBOSE_H__
#define __VERBOSE_H__

/* Messages always displayed */
#define VRB_ALWAYS  0

/* Messages leading to abnormal program termination, for example unhandled exceptions or missing functions */
#define VRB_MAJOR   1
#define VRB_MINOR   2

/* Messages showing that a non-fatal error has occured */
#define VRB_ERROR   3
#define VRB_WARNING 4

/* Messages for information only */
#define VRB_INFO    5
#define VRB_TRACE   6
#define VRB_DEBUG   7

#ifdef WIN32
__declspec( dllexport ) void _Verbose(BYTE level, const char *fmt, ...);
__declspec( dllexport ) void _SetVerbose(BYTE level, char *filename);
#else
void _Verbose(BYTE level, const char *fmt, ...);
void _VerboseError(BYTE level, int errcode, const char *fmt, ...);
void _VerbosePError(BYTE level, const char *fmt, ...);
void _SetVerbose(BYTE level, char *filename);
#endif

extern BYTE verbose_level;

#endif

