/*
 *  common/lib-c/utils/logfile, logging file utility
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

/*
 * C lib
 */

#include "lib-c/types.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#define MAX_PATH 256
#endif

#include "logfile.h"

char *global_log_file = NULL;

BOOL FLOG_AddLine(char *log_file, char *log_line)
{
  FILE *fd;
  struct tm *stm;
  unsigned long now;
  
  now = time(NULL);
  stm = localtime(&now);

  if (log_file == NULL)
    log_file = global_log_file;
  if (log_file == NULL)
    return FALSE;
  
  fd = fopen(log_file, "a+");
  if (fd == NULL)
    return FALSE;
  
  fprintf(fd, "%02d%02d%02d%02d%02d%02d:%s\n",
              stm->tm_mday,
              stm->tm_mon+1,
              stm->tm_year%100,
              stm->tm_hour,
              stm->tm_min,
              stm->tm_sec,
              log_line);
  fclose(fd);
  return TRUE;
}

BOOL FLOG_RotateSz(char *log_file, unsigned long max_size)
{
  struct stat st;
  
  if (log_file == NULL)
    log_file = global_log_file;
  if (log_file == NULL)
    return FALSE;

  if ((stat(log_file, &st) != -1) && (st.st_size >= (signed long) max_size))
  {
    /* Il va falloir vider ce fichier */
    char old_file[MAX_PATH];
    strcpy(old_file, log_file);
    strcat(old_file, ".old");
    /* Supprime l'ancien fichier */
    unlink(old_file);
    /* Renomme le fichier log */
    rename(log_file, old_file);
    /* Supprime le fichier log */
    unlink(log_file);
    /* Enregistre l'evenement */
    return FLOG_AddLine(log_file, "Vidage du fichier (taille)");
  }

  return FALSE;
}

