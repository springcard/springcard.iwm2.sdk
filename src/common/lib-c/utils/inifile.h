/*
 *  common/lib-c/utils/cfgfile, config file utility
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

#ifndef _INIFILE_H
#define _INIFILE_H

int INIFILE_ReadSection(const char *fname, int num, char *section);
int INIFILE_ReadNames(const char *fname, const char *section, int num, char *name);

void INIFILE_ReadString(const char *fname, const char *section, const char *name, char *value, const char *def, int len);
void INIFILE_WriteString(const char *fname, const char *section, const char *name, const char *value);
void INIFILE_ReadInteger(const char *fname, const char *section, const char *name, signed long *value, signed long def);
void INIFILE_WriteInteger(const char *fname, const char *section, const char *name, signed long value);

int INIFILE_ReadSection_Ex(const char *fname, int num, const char *section, char *sep);
int INIFILE_ReadNames_Ex(const char *fname, const char *section, int num, const char *name, char *sep);
char *INIFILE_ReadSection_Content(const char *fname, const char *section);

void INIFILE_ReadString_Ex(const char *fname, const char *section, const char *name, char *value, const char *def, int len, char *sep);
void INIFILE_WriteString_Ex(const char *fname, const char *section, const char *name, const char *value, char sep);
void INIFILE_ReadInteger_Ex(const char *fname, const char *section, const char *name, signed long *value, signed long def, char *sep);
void INIFILE_WriteInteger_Ex(const char *fname, const char *section, const char *name, signed long value, char sep);

void INIFILE_Delete(const char *fname, const char *section, const char *name);
void INIFILE_DropLine(const char *fname, int nline);

#define INIFILE_EasyReadString(a,b,c,d) INIFILE_ReadString(a,b,c,d,"",sizeof(d))

#ifndef INIFILE_MAXLINE_LEN
	#define INIFILE_MAXLINE_LEN 640
#endif

#ifndef INIFILE_SEPARATOR
	#define INIFILE_SEPARATOR   '='
#endif

#endif
