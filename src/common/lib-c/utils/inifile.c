/*
 *  common/lib-c/utils/inifile, config file utility
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

#include "inifile.h"
#include "strl.h"

int INIFILE_ReadSection(const char *fname, int num, char *section)
{
  char buffer[INIFILE_MAXLINE_LEN];
  char *ptr, *t;
  int  sectionNum = -1;
  FILE *fd = NULL;

  /* Open file */
  fd = fopen(fname, "r");
  if (fd == NULL) return 0;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    /* Find line start */
    ptr = buffer;
    while (*ptr <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if ((*ptr == '#') || (*ptr == ';')) continue;
    /* If line is a session bloc... */
    if (*ptr == '[')
    {
      ptr++;
      sectionNum++;
      if (sectionNum == num)
      {
        strtok_r(ptr, "]", &t);
        strcpy(section, ptr);
        fclose(fd);
        return 1;
      }
    }
  }
  /* Not found */
  fclose(fd);
  return 0;
}

int INIFILE_ReadNames(const char *fname, const char *section, int num, char *name)
{
  char buffer[INIFILE_MAXLINE_LEN];
  char *ptr, *t;
  int  lineNum = -1;
  int  in_section = 0;
  FILE *fd = NULL;

  /* Check section */
  if (!strlen(section)) return 0;
  /* Check name */
  if (name == NULL) return 0;

  /* Open file */
  fd = fopen(fname, "r");
  if (fd == NULL) return -1;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    /* Find line start */
    ptr = buffer;
    if ((*ptr == '\r') || (*ptr == '\n') || (*ptr == '\0')) continue;
    while (*ptr <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if ((*ptr == '#') || (*ptr == ';')) continue;

    /* If line is a session bloc... */
    if (*ptr == '[')
    {
      if (in_section)
      {
        /* Leaving the right section, no need to continue */
        in_section = 0;
        fclose(fd);
        return 0;
      } else
      {
        /* Check if we're on the right section */
        ptr++;
        if (strncmp(ptr, section, strlen(section))) continue;
        ptr += strlen(section);
        if (*ptr != ']') continue;
        /* Yes !!! */
        in_section = 1;
        continue;
      }      
    }
    if (in_section)
    {
      /* Check if we're on the right line */
      lineNum++;
      if (lineNum == num)
      {
        /* Looks great; copy the name ! */
        strtok_r(ptr, " =:\t\r\n", &t);
        strcpy(name, ptr);
        /* OK, that's it ! */
        fclose(fd);
        return 1;
      }
    }
  }

  /* Not found */
  fclose(fd);
  return 0;
}

int INIFILE_FindString(const char *fname, const char *section, const char *name, char *value, int len)
{
  char buffer[INIFILE_MAXLINE_LEN];
  char *ptr;
  int  lineNum = -1;
  int  in_section = 0;
  FILE *fd = NULL;

  /* Clear result buffer */
  if (value!=NULL) *value='\0';

  /* Check section */
  if (section == NULL)
  {
    in_section = 1;
  } else
  {
    if (!strlen(section)) return -1;
  }
  /* Check name */
  if (!strlen(name)) return -1;

  /* Open file */
  fd = fopen(fname, "r");
  if (fd == NULL) return -1;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    lineNum++;
    /* Find line start */
    ptr = buffer;
    while (*ptr <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if ((*ptr == '#') || (*ptr == ';')) continue;
    /* If line is a session bloc... */
    if (*ptr == '[')
    {
      if (section == NULL)
      {
        continue;
      } else
      if (in_section)
      {
        in_section = 0;
        continue; 
      } else
      {
        /* Check if we're on the right section */
        ptr++;
        if (strncmp(ptr, section, strlen(section))) continue;
        ptr += strlen(section);
        if (*ptr != ']') continue;
        /* Yes !!! */
        in_section = 1;
        continue;
      }      
    }
    if (in_section)
    {
      /* Check if we're on the right line */
      if (strncmp(ptr, name, strlen(name))) continue;
      /* Looks great; skip name and go to data block */
      ptr += strlen(name);
      /* Check if we've got a separator */
      if ((*ptr != ' ') && (*ptr != '\t') && (*ptr != ':') && (*ptr != '=')) continue;
      /* OK, that's it ! */
      if ((value==NULL) || (len==0))
      {
        fclose(fd);
        return lineNum;
      }
      /* Remove other separators */
      while ((*ptr != '\0') && ((*ptr <= ' ') || (*ptr == ':') || (*ptr == '='))) ptr++;
      /* Copy value */
      strlcpy(value, ptr, len);
      /* Remove any comment */
      ptr = value;
      while ((*ptr != '\0') && (*ptr != '#') && (*ptr != ';')) ptr++;
      *ptr='\0';
      /* Remove any trailing separators */
      ptr--;
      while (*ptr <= ' ') ptr--;
      ptr++;
      *ptr='\0';
      /* OK, done */
      fclose(fd);
      return lineNum;
    }
  }
  /* Not found */
  fclose(fd);
  return -1;
}

void INIFILE_Delete(const char *fname, const char *section, const char *name)
{
  int lineNum = INIFILE_FindString(fname, name, section, NULL, 0);
  if (lineNum != -1)
    INIFILE_DropLine(fname, lineNum);
}
    
void INIFILE_ReadString(const char *fname, const char *section, const char *name, char *value, const char *def, int len)
{
  if (value == NULL) return;
  if (INIFILE_FindString(fname, section, name, value, len) == -1)
  {
    /* Set default value */
    if (def != NULL)
      strlcpy(value, def, len);
  }
}

int INIFILE_FindSectionBegin(const char *fname, const char *section)
{
  char buffer[INIFILE_MAXLINE_LEN];
  char *ptr;
  int  lineNum = -1;
  //int  in_section = 0;
  //int last_section_line = -1;
  FILE *fd = NULL;

  /* Check section */
  if (!strlen(section)) return -1;

  /* Open file */
  fd = fopen(fname, "r");
  if (fd == NULL) return -1;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    lineNum++;
    /* Find line start */
    ptr = buffer;
    while (*ptr <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if ((*ptr == '#') || (*ptr == ';')) continue;
    /* If line is a session bloc... */
    if (*ptr == '[')
    {
      /* Check if we're on the right section */
      ptr++;
      if (strncmp(ptr, section, strlen(section))) continue;
      ptr += strlen(section);
      if (*ptr != ']') continue;
      /* Yes !!! */
      return lineNum;
    }
  }
  /* Not found */
  fclose(fd);
  return -1; 
}

int INIFILE_FindSectionEnd(const char *fname, const char *section)
{
  char buffer[INIFILE_MAXLINE_LEN];
  char *ptr;
  int  lineNum = -1;
  int  in_section = 0;
  int last_section_line = -1;
  FILE *fd = NULL;

  /* Check section */
  if (!strlen(section)) return -1;

  /* Open file */
  fd = fopen(fname, "r");
  if (fd == NULL) return -1;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    lineNum++;
    /* Find line start */
    ptr = buffer;
    while (*ptr <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if ((*ptr == '#') || (*ptr == ';')) continue;
    /* If line is a session bloc... */
    if (*ptr == '[')
    {
      if (in_section)
      {
        /* This is the end of the section ! */
        fclose(fd);
        return last_section_line + 1;
      } else
      {
        /* Check if we're on the right section */
        ptr++;
        if (strncmp(ptr, section, strlen(section))) continue;
        ptr += strlen(section);
        if (*ptr != ']') continue;
        /* Yes !!! */
        in_section = 1;
        last_section_line = lineNum;
        continue;
      }      
    }
    if (in_section)
    {
      last_section_line = lineNum;
    }
  }

  /* No explicit end found */
  fclose(fd);
  return last_section_line; 
}

void INIFILE_WriteString(const char *fname, const char *section, const char *name, const char *value)
{
  char buffer[INIFILE_MAXLINE_LEN];
  int lineNum;
  int lineIndex = -1;
  int lineSection = -1;
  FILE *fdIn  = NULL;
  FILE *fdOut = NULL;

  /* Find existing line in file */
  lineNum = INIFILE_FindString(fname, section, name, buffer, sizeof(buffer));
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
    lineSection = INIFILE_FindSectionEnd(fname, section);
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
    /* Old file doesn't exist */
    if (strlen(value))
    {
      fprintf(fdOut, "[%s]\n", section);
      fprintf(fdOut, "%s%c%s\n", name, INIFILE_SEPARATOR, value);
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
        fprintf(fdOut, "%s%c%s\n", name, INIFILE_SEPARATOR, value);
      }
    } else
    if (lineIndex == lineSection)
    {
      /* Put new line here, at the end of the section */
      if (strlen(value))
      { 
        fprintf(fdOut, "%s%c%s\n", name, INIFILE_SEPARATOR, value);
      }     
    }
    
    /* Put existing line */
    fputs(buffer, fdOut);
  }

  if ((lineSection == -1) && (lineNum == -1))
  {
    /* Put new section and new line here, at the end of the file */
    fprintf(fdOut, "[%s]\n", section);
    fprintf(fdOut, "%s%c%s\n", name, INIFILE_SEPARATOR, value);
  }
 
  /* Done */
  fclose(fdIn);
  fflush(fdOut);  
  fclose(fdOut);
#ifndef WIN32
  sync();
#endif

  /* Remove old file */
  strcpy(buffer, fname);
  strcat(buffer, "~");
  unlink(buffer);  
}

void INIFILE_ReadInteger(const char *fname, const char *section, const char *name, signed long *value, signed long def)
{
  char buffer[12];
  INIFILE_ReadString(fname, section, name, buffer, NULL, 12);
  if (!strlen(buffer))
  {
    sprintf(buffer, "%ld", def);
  }
  *value = atol(buffer);
}

void INIFILE_WriteInteger(const char *fname, const char *section, const char *name, signed long value)
{
  char buffer[12];
  sprintf(buffer, "%ld", value);
  INIFILE_WriteString(fname, section, name, buffer);
}

void INIFILE_DropLine(const char *fname, int nline)
{
  char buffer[INIFILE_MAXLINE_LEN];
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
#ifndef WIN32
  sync();
#endif

  /* Remove old file */
  strcpy(buffer, fname);
  strcat(buffer, "~");
  unlink(buffer);
}

char *INIFILE_ReadSection_Content(const char *fname, const char *section)
{
  char buffer[INIFILE_MAXLINE_LEN];
  char *result;
  FILE *fd = NULL;
  int b, e, l = 0;

  b = INIFILE_FindSectionBegin(fname, section);
  e = INIFILE_FindSectionEnd(fname, section);

  if ((b < 0) || (e < 0) || (e < b)) return NULL;

  fd = fopen(fname, "r");
  if (fd == NULL) return NULL;

  result = calloc(INIFILE_MAXLINE_LEN, (e - b));
  if (result == NULL) goto done;

  /* Read whole file */
  while (fgets(buffer, sizeof(buffer), fd) != NULL)
  {
    if ((l > b) && (l <= e))
    {
      strcat(result, buffer);
    }
    l++;
  }

done:
  fclose(fd);
  return result;
}
