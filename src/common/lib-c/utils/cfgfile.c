/*
 *  common/lib-c/utils/cfgfile, config file utility
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

/*
 * C lib
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#ifndef WIN32
  #include <unistd.h>
#endif

#ifdef WIN32
  #pragma warning(disable : 4996)
#endif

#include "cfgfile.h"
#include "strl.h"

char CFGFILE_SEPARATOR = CFGFILE_DEFAULT_SEPARATOR;

FILE *CFGFILE_OpenReadOnly(char *filename);
FILE *CFGFILE_OpenWriteNew(char *filename);
void CFGFILE_Close(FILE *fd);

void CFGFILE_ReadFile(const char *fname, int (*handler)(int, char *))
{
  char buffer[CFGFILE_MAXLINE_LEN];
  int  lineNum = -1;
  char *t;
  FILE *fd = NULL;

  /* Open file */
  fd = fopen(fname, "rt");
  if (fd == NULL) return;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    lineNum++;
    strtok_r(buffer, "\r\n", &t);
    if (handler(lineNum, buffer))
      break;
  }
  fclose(fd);
}

int CFGFILE_ReadLine(const char *fname, int nline, char *sline, int len)
{
  char buffer[CFGFILE_MAXLINE_LEN];
  int  lineNum = -1;
  char *t;
  FILE *fd = NULL;

  /* Open file */
  fd = fopen(fname, "r");
  if (fd == NULL) return -1;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    lineNum++;
    if (lineNum == nline)
    {
      fclose(fd);
      strlcpy(sline, buffer, len);
      strtok_r(sline, "\r\n", &t);
      return lineNum;
    }
  }
  /* Not found */
  fclose(fd);
  return -1;
}

void CFGFILE_DropLine(const char *fname, int nline)
{
  char buffer[CFGFILE_MAXLINE_LEN];
  int lineNum = -1;
  FILE *fdIn  = NULL;
  FILE *fdOut = NULL;

  /* Remove old file (if exists) */
  strcpy(buffer, fname);
  strcat(buffer, "~");
  unlink(buffer);
  
  /* Backup current file as old file */
  rename(fname, buffer);

  /* Open old file */  
  fdIn  = fopen(buffer, "r");
  if (fdIn == NULL)
  {
    /* File doesn't exists */
    return;
  }
  
  /* Create new file */
  fdOut = fopen(fname, "w");
  if (fdOut == NULL)
  {
    /* Unable to create output */
    fclose(fdIn);
    /* Restore old file */
    rename(buffer, fname);
    return;
  }
  
  /* Read whole file */
  lineNum = -1;
  while (fgets(buffer, sizeof(buffer), fdIn) != NULL)
  {
    lineNum++;
    if (lineNum == nline)
    {
      /* DO NOTHING = Remove the line !!! */
    }
    else
    {
      /* Put existing line */
      fputs(buffer, fdOut);
    }
  }
 
  /* Done */
  fclose(fdIn);
  fflush(fdOut);
  fclose(fdOut);

  /* Remove old file */
  strcpy(buffer, fname);
  strcat(buffer, "~");
  unlink(buffer);
}

void CFGFILE_WriteLine(const char *fname, int nline, char *sline)
{
  char buffer[CFGFILE_MAXLINE_LEN];
  int lineNum = -1;
  FILE *fdIn  = NULL;
  FILE *fdOut = NULL;

  if (nline != -1)
  {
    /* Open file */
    fdIn = fopen(fname, "r");
    if (fdIn != NULL)
    {
      /* Read whole file */
      while (fgets(buffer, sizeof(buffer), fdIn) != NULL)
      {
        lineNum++;
        if (lineNum == nline)
        {
          /* This is the line */
          if (!strcmp(buffer, sline))
          {
            /* Already the good value */
            fclose(fdIn);
            return;
          }
          /* Not the good value... */
          break;
        }
      }
      /* Not found */
      fclose(fdIn);
    }
  }

  /* Remove old file (if exists) */
  strcpy(buffer, fname);
  strcat(buffer, "~");
  unlink(buffer);
  
  /* Backup current file as old file */
  rename(fname, buffer);

  /* Open old file */  
  fdIn  = fopen(buffer, "r");
  
  /* Create new file */
  fdOut = fopen(fname, "w");
  if (fdOut == NULL)
  {
    /* Unable to create output */
    if (fdIn != NULL)
      fclose(fdIn);
    /* Restore old file */
    rename(buffer, fname);
    return;
  }

  if (fdIn == NULL)
  {
    /* Old file doesn't exists */
    if (strlen(sline))
    {
      fprintf(fdOut, "%s\n", sline);
      fflush(fdOut);
    }
    fclose(fdOut);
    return;
  }
  
  /* Read whole file */
  lineNum = -1;
  while (fgets(buffer, sizeof(buffer), fdIn) != NULL)
  {
    lineNum++;
    if (lineNum == nline)
    {
      /* Put line here */
      if (strlen(sline))
      { 
        fprintf(fdOut, "%s\n", sline);
      }
    }
    else
    {
      /* Put existing line */
      fputs(buffer, fdOut);
    }
  }
  if ((nline == -1) || (lineNum < nline))
  {
    /* Put new line here, at the end of the file */
    fprintf(fdOut, "%s\n", sline);
  }
 
  /* Done */
  fclose(fdIn);
  fflush(fdOut);
  fclose(fdOut);

  /* Remove old file */
  strcpy(buffer, fname);
  strcat(buffer, "~");
  unlink(buffer);
}

int CFGFILE_FindString(const char *fname, const char *name, char *value, int len)
{
  char buffer[CFGFILE_MAXLINE_LEN];
  char *ptr, *t;
  int  lineNum = -1;
  FILE *fd = NULL;

  /* Clear result buffer */
  if (value!=NULL) *value='\0';

  /* Check name */
  if (!strlen(name)) return -1;

  /* Open file */
  fd = fopen(fname, "r");
  if (fd == NULL) return -1;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    lineNum++;
    strtok_r(buffer, "\r\n", &t);
    /* Find line start */
    ptr = buffer;
    while (ptr[0] <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if ((ptr[0] == '#') || (ptr[0] == ';')) continue;
    if ((ptr[0] == '/') && (ptr[1] == '/')) continue;
    /* Check if we're on the right line */
    if (strncmp(ptr, name, strlen(name))) continue;
    /* Looks great; skip name and go to data block */
    ptr += strlen(name);
    /* Check if we've got a separator or the end of the line */
    if ((ptr[0] != '\n') && (ptr[0] != '\r') && (ptr[0] != '\0') && (ptr[0] != ' ') && (ptr[0] != '\t') && (ptr[0] != ':') && (ptr[0] != '=')) continue;
    /* OK, that's it ! */
    if ((value==NULL) || (len==0))
    {
      fclose(fd);
      return lineNum;
    }
    /* Remove other separators */
    while ((ptr[0] != '\0') && ((ptr[0] <= ' ') || (ptr[0] == ':') || (ptr[0] == '='))) ptr++;
    /* Copy value */
    strlcpy(value, ptr, len);
    /* OK, done */
    fclose(fd);
    return lineNum;
  }
  /* Not found */
  fclose(fd);
  return -1;
}

void CFGFILE_Delete(const char *fname, const char *name)
{
  int lineNum = CFGFILE_FindString(fname, name, NULL, 0);
  if (lineNum != -1)
    CFGFILE_DropLine(fname, lineNum);
}
    
void CFGFILE_ReadString(const char *fname, const char *name, char *value, const char *def, int len)
{
  if (value == NULL) return;
  if (CFGFILE_FindString(fname, name, value, len) == -1)
  {
    /* Set default value */
    if (def != NULL)
      strlcpy(value, def, len);
  }
}

void CFGFILE_WriteString_Ex(const char *fname, const char *name, const char *value, char sep)
{
  char buffer[CFGFILE_MAXLINE_LEN];
  int lineNum;
  int lineIndex = -1;
  FILE *fdIn  = NULL;
  FILE *fdOut = NULL;

  /* Find existing line in file */
  lineNum = CFGFILE_FindString(fname, name, buffer, sizeof(buffer));
  if (lineNum != -1)
  {
    /* Already exists */
    if (strcmp(value, buffer) == 0)
    {
      /* Already the good value !!! */
      return; 
    }
  } else
  {
    /* Doesn't exist */
    if (strlen(value) == 0)
    {
      /* And will not !!! */
      return; 
    } 
  }

  if (sep == '\0')
    sep = CFGFILE_SEPARATOR;
  
  /* Remove old file (if exists) */
  strcpy(buffer, fname);
  strcat(buffer, "~");
  unlink(buffer);
  
  /* Backup current file as old file */
  rename(fname, buffer);

  /* Open old file */  
  fdIn  = fopen(buffer, "r");
  
  /* Create new file */
  fdOut = fopen(fname, "w");
  if (fdOut == NULL)
  {
    /* Unable to create output */
    if (fdIn != NULL)
      fclose(fdIn);
    /* Restore old file */
    rename(buffer, fname);
    return;
  }

  if (fdIn == NULL)
  {
    /* Old file doesn't exists */
    if (strlen(value))
    {
      fprintf(fdOut, "%s%c%s\n", name, sep, value);
      fflush(fdOut);
    }
    fclose(fdOut);
    return;
  }

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fdIn) != NULL)
  {
    lineIndex++;
    if (lineIndex == lineNum)
    {
      /* Put new line here */
      if (strlen(value))
      { 
        fprintf(fdOut, "%s%c%s\n", name, sep, value);
      }
    }
    else
    {
      /* Put existing line */
      fputs(buffer, fdOut);
    }
  }
  if (lineNum == -1)
  {
    /* Put new line here, at the end of the file */
    fprintf(fdOut, "%s%c%s\n", name, sep, value);
  }
 
  /* Done */
  fclose(fdIn);
  fflush(fdOut);  
  fclose(fdOut);

  /* Remove old file */
  strcpy(buffer, fname);
  strcat(buffer, "~");
  unlink(buffer);
}

void CFGFILE_WriteString(const char *fname, const char *name, const char *value)
{
  CFGFILE_WriteString_Ex(fname, name, value, '\0');
}

void CFGFILE_ReadInteger(const char *fname, const char *name, signed long *value, signed long def)
{
  char buffer[64];
  CFGFILE_ReadString(fname, name, buffer, NULL, 64);
  if (!strlen(buffer))
  {
    sprintf(buffer, "%ld", def);
  }
  *value = atol(buffer);
}

void CFGFILE_WriteInteger_Ex(const char *fname, const char *name, signed long value, char sep)
{
  char buffer[64];
  sprintf(buffer, "%ld", value);
  CFGFILE_WriteString_Ex(fname, name, buffer, sep);
}

void CFGFILE_WriteInteger(const char *fname, const char *name, signed long value)
{
  CFGFILE_WriteInteger_Ex(fname, name, value, '\0');
}

