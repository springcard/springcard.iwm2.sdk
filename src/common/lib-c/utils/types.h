/*
 *  types.h
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */
 
#ifndef __LIB_C_TYPES_H__
#define __LIB_C_TYPES_H__

#ifdef __MC68K__
  #define __palmos__
#endif

/* Translating target to size */
/* -------------------------- */

#ifdef unix
  #ifndef UNIX
    #define UNIX
  #endif
#endif
#if (defined (__linux__) || defined (__linux) || defined (linux))
  #ifndef UNIX
    #define UNIX
  #endif
#endif

#ifdef UNIX
  #define HAS_LIB_C_TYPES
 
  #include <stdint.h>

	typedef uint8_t  BOOL;
  typedef uint8_t  BYTE;
  typedef int8_t   SBYTE;
  typedef uint16_t WORD;
  typedef int16_t  SWORD;
  typedef uint32_t DWORD;
  typedef int32_t  SDWORD;
  typedef uint64_t QWORD;
  typedef int64_t  SQWORD;
  typedef signed long    LONG;
  
  #define _T(x) x
  #define TCHAR char
  
#endif

#ifdef __palmos__
  #define HAS_LIB_C_TYPES
  #include <PalmOS.h>
  /* ------------ */
  typedef unsigned char	 BYTE;
  typedef signed char	   SBYTE;
  typedef unsigned char	 BOOL;
  typedef signed short   SWORD;
  typedef signed long		 SDWORD;
  typedef unsigned short WORD;
  typedef unsigned long  DWORD;
  typedef signed long    LONG;

  #define _T(x) x
  #define TCHAR char

  #define memset(a,b,c) MemSet(a,c,b)
  #define memcpy(a,b,c) MemCpy(a,b,c)
  #define htons(a) NetHToNS(a)
  #define htonl(a) NetHToNL(a)
  #define ntohs(a) NetNToHS(a)
  #define ntohl(a) NetNToHL(a)  
#endif

#ifdef NEURON
  #define HAS_LIB_C_TYPES
  /* ------------ */
  typedef unsigned char  BYTE;
  typedef signed char    SBYTE;
  typedef unsigned char  BITS;
  typedef unsigned char  BOOL;
  typedef unsigned long  WORD;
  typedef signed long    SWORD;
#endif

#if (defined(UNDER_CE) || defined(_WIN32_WCE))
  /* WinCE is only a subset of Win32 */
  #ifndef WIN32
    #define WIN32
  #endif
  #ifndef WINCE
    #define WINCE
  #endif
#endif

#ifdef WIN32

  #define HAS_LIB_C_TYPES
  /* ------------ */
#if (!defined(UNDER_CE) && !defined(WIN32_NO_WINSOCK))
  #include <winsock2.h>
#endif

  #include <windows.h>
  #include <tchar.h>
  #include <time.h>
  typedef unsigned char  BYTE;
  typedef signed char    SBYTE;
  typedef signed short   SWORD;
  typedef signed long    SDWORD;
  typedef signed long    LONG;
  typedef unsigned char  BITS;
  typedef unsigned __int64 QWORD;
  typedef signed __int64 SQWORD;
  #define StrNPrintF _sntprintf
  #define StrNCat(x, y, z) _tcsncat(x, y, z)
  #define StrCat(x, y) _tcscat(x, y)
  #define StrCopy(x, y) _tcscpy(x, y)
  #define StrLen _tcslen
#endif

#ifndef HAS_LIB_C_TYPES
  typedef unsigned char  BYTE;
  typedef signed char    SBYTE;
  typedef unsigned short WORD;
  typedef signed short   SWORD;
  typedef signed long    DWORD;
  typedef signed long    SDWORD;
  typedef unsigned char  BITS;
  typedef unsigned char  BOOL;
  #define HAS_LIB_C_TYPES
#endif

#ifndef TRUE
  #define TRUE 1
#endif
#ifndef FALSE
  #define FALSE 0
#endif

#endif
