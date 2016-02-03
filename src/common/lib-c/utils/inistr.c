#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#ifndef WIN32
  #include <unistd.h>
#endif

#include "inistr.h"
#include "strl.h"

#ifndef INISTR_COMMENTS
  #define INISTR_COMMENTS    "#;"
#endif

static int is_comment_char(char c)
{
  if (strchr(INISTR_COMMENTS, c) != NULL)
	  return 1;

  return 0;
}

static int get_line(char *buffer, size_t size, const char **lines_ptr)
{
  if (**lines_ptr == '\0') return 0;

  while (--size)
  {
    /* Assign */
    *buffer = **lines_ptr;

    /* End of buffer ? */
    if (*buffer == '\0') break;

    /* End of line ? */
    *lines_ptr = 1 + *lines_ptr;
    if (*buffer == '\n') break;

    buffer = 1 + buffer;
  }

  if ((size == 0) && (*buffer != '\n') && (*buffer != '\0'))
  {
    /* Truncated line */
    for (;;)
    {
      if (**lines_ptr == '\n') break;
      if (**lines_ptr == '\0') break;
      *lines_ptr = 1 + *lines_ptr;
    }
  }

  *buffer = '\0';
  return 1;
}

int INISTR_ReadSection(const char *lines, int num, char *section)
{
  char buffer[INISTR_MAXLINE_LEN];
  char *ptr, *t;
  int  sectionNum = -1;
  const char *lines_ptr = lines;

  /* Read whole file */
  while (get_line(buffer, sizeof(buffer), &lines_ptr))
  {
    /* Find line start */
    ptr = buffer;
    while (*ptr <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if (is_comment_char(*ptr)) continue;
    /* If line is a session bloc... */
    if (*ptr == '[')
    {
      ptr++;
      sectionNum++;
      if (sectionNum == num)
      {
        strtok_r(ptr, "\r\n", &t);
        t = ptr + strlen(ptr);
        do
        {
          if (*t == ']')
          {
            *t = '\0';
            break;
          }
        } while (--t > ptr);
        strcpy(section, ptr);
        return 1;
      }
    }
  }

  /* Not found */
  return 0;
}

int INISTR_ReadNames(const char *lines, const char *section, int num, char *name)
{
  char buffer[INISTR_MAXLINE_LEN];
  char *ptr, *t;
  int  lineNum = -1;
  int  in_section = 0;
  const char *lines_ptr = lines;

  /* Check section */
  if (!strlen(section)) return 0;
  /* Check name */
  if (name == NULL) return 0;

  /* Read whole file */
  while (get_line(buffer, sizeof(buffer), &lines_ptr))
  {
    /* Find line start */
    ptr = buffer;
    while (*ptr <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if (is_comment_char(*ptr)) continue;
    /* If line is a session bloc... */
    if (*ptr == '[')
    {
      if (in_section)
      {
        /* Leaving the right section, no need to continue */
        in_section = 0;
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
        return 1;
      }
    }
  }
  /* Not found */
  return 0;
}

int INISTR_FindString(const char *lines, const char *section, const char *name, char *value, int len)
{
  char buffer[INISTR_MAXLINE_LEN];
  char *ptr;
  int  lineNum = -1;
  int  in_section = 0;
  const char *lines_ptr = lines;

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
  if ((name==NULL) || !strlen(name)) return -1;

  /* Read whole file */
  while (get_line(buffer, sizeof(buffer), &lines_ptr))
  {
    lineNum++;
    /* Find line start */
    ptr = buffer;
    while (*ptr <= ' ') ptr++;
    /* If line is a comment then go to next one */
    if (is_comment_char(*ptr)) continue;
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
        return lineNum;
      }
      /* Remove other separators */
      while ((*ptr != '\0') && ((*ptr <= ' ') || (*ptr == ':') || (*ptr == '='))) ptr++;
      /* Copy value */
      strlcpy(value, ptr, len);
      /* Remove any comment */
      ptr = value;
      while ((*ptr != '\0') && (!is_comment_char(*ptr))) ptr++;
      *ptr='\0';
      /* Remove any trailing separators */
      ptr--;
      while (*ptr <= ' ') ptr--;
      ptr++;
      *ptr='\0';
      /* OK, done */
      return lineNum;
    }
  }
  /* Not found */
  return -1;
}

void INISTR_ReadString(const char *lines, const char *section, const char *name, char *value, const char *def, int len)
{
  if (value == NULL) return;
  if (INISTR_FindString(lines, section, name, value, len) == -1)
  {
    /* Set default value */
    if (def != NULL)
      strlcpy(value, def, len);
  }
}

void INISTR_ReadInteger(const char *lines, const char *section, const char *name, signed long *value, signed long def)
{
  char buffer[12];
  INISTR_ReadString(lines, section, name, buffer, NULL, 12);
  if (!strlen(buffer))
  {
    sprintf(buffer, "%ld", def);
  }
  *value = atol(buffer);
}

