#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strl.h"
#include "csvfile.h"

int CSVFILE_Foreach(const char *filename, CSVFILE_PARSE_CALLBACK callback)
{
  char buffer[CSVFILE_MAXLINE_LEN];
  int line = -1;
  int r = 1;
  int i, l;
  char *t;
  FILE *fd = NULL;
  int columnc;
  int in_quotes;
  char **columnv;

  if ((filename == NULL) || (callback == NULL))
    return 0;

  /* Open file */
  fd = fopen(filename, "rt");
  if (fd == NULL)
  {
    return 0;
  }

  /* Read whole file */
  while ((fgets(buffer, sizeof(buffer), fd) != NULL) && (r))
  {
    line++;
    strtok_r(buffer, "\r\n", &t);
    l = strlen(buffer);

    /* Count the number of colums */
    columnc = 1;
    in_quotes = 0;
    for (i=0; i<l; i++)
    {
      if (buffer[i] == '"')
        in_quotes = !in_quotes;
      if ((buffer[i] == CSVFILE_SEPARATOR) && (!in_quotes))
        columnc++;
    }

    /* Allocate the pointers to the columns */
    columnv = malloc(columnc * sizeof(char *));
    if (columnv == NULL)
    {
      r = 0;
      break;
    }

    /* Populate the pointers (and remove the separators from the string) */
    columnc = 0;
    columnv[0] = buffer;
    in_quotes = 0;
    for (i=0; i<l; i++)
    {
      if (buffer[i] == '"')
        in_quotes = !in_quotes;
      if ((buffer[i] == CSVFILE_SEPARATOR) && (!in_quotes))
      {
        columnv[++columnc] = &buffer[i + 1];
        buffer[i] = '\0';
      }
    }

    /* Cleanup the columns (remove the quotes) */
    for (i=0; i<columnc; i++)
    {
      if ((columnv[i][0] == CSVFILE_SEPARATOR) && (columnv[i][strlen(columnv[i]) - 1] == CSVFILE_SEPARATOR))
      {
        columnv[i][strlen(columnv[i]) - 1] = '\0';
        columnv[i] = &columnv[i][0];
      }
    }

    if (!callback(line, columnc, columnv))
      r = 0;

    free(columnv);
  }

  fclose(fd);
  return r;
}
