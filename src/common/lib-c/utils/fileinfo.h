#ifndef __FILEINFO_H__
#define __FILEINFO_H__

#ifdef WIN32
	#include <windows.h>
#else
	#include <stdlib.h>
	#include <string.h>
#endif

BOOL create_directory(const char *filename);
BOOL file_exists(const char *filename);
DWORD file_size(const char *filename);

#endif
