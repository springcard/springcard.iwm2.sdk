#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#ifndef WIN32
  #include <fcntl.h>
  #include <unistd.h>
#else
  #include <direct.h>
#endif

#include "lib-c/types.h"
#include "fileinfo.h"

BOOL create_directory(const char *filename)
{
#ifndef WIN32
  mkdir(filename, 0);
#else
  mkdir(filename);
#endif
  return TRUE;
}

BOOL file_exists(const char *filename)
{
  struct stat fs;

  if (stat(filename, &fs) < 0)
	{
		return FALSE;
  }
  return TRUE;
}

DWORD file_size(const char *filename)
{
  struct stat fs;

  if (stat(filename, &fs) < 0)
	{
		return 0;
  }
  return fs.st_size;
}
