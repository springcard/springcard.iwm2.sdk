/*
 *  common/lib-c/utils/cfgfile, config file utility
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

#ifndef _CFGFILE_H
#define _CFGFILE_H

void CFGFILE_ReadFile(const char *fname, int (*handler)(int, char *));

int CFGFILE_FindString(const char *fname, const char *name, char *value, int len);
void CFGFILE_WriteLine(const char *fname, int nline, char *sline);
int CFGFILE_ReadLine(const char *fname, int nline, char *sline, int len);
void CFGFILE_DropLine(const char *fname, int nline);

void CFGFILE_ReadString(const char *fname, const char *name, char *value, const char *def, int len);
void CFGFILE_WriteString(const char *fname, const char *name, const char *value);
void CFGFILE_ReadInteger(const char *fname, const char *name, signed long *value, signed long def);
void CFGFILE_WriteInteger(const char *fname, const char *name, signed long value);

void CFGFILE_WriteString_Ex(const char *fname, const char *name, const char *value, char sep);
void CFGFILE_WriteInteger_Ex(const char *fname, const char *name, signed long value, char sep);

void CFGFILE_Delete(const char *fname, const char *name);

#define CFGFILE_EasyReadString(a,b,c) CFGFILE_ReadString(a,b,c,"",sizeof(c))

#ifndef CFGFILE_MAXLINE_LEN
	#define CFGFILE_MAXLINE_LEN 640
#endif

#ifndef CFGFILE_DEFAULT_SEPARATOR
	#define CFGFILE_DEFAULT_SEPARATOR '='
#endif

extern char CFGFILE_SEPARATOR;

#endif
