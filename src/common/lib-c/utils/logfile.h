/*
 *  common/lib-c/utils/logfile, loggin utility
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

#ifndef LOG_FILE_H
#define LOG_FILE_H

BOOL FLOG_AddLine(char *log_file, char *log_line);
BOOL FLOG_RotateSz(char *log_file, unsigned long max_size);
extern char *global_log_file;

#endif
