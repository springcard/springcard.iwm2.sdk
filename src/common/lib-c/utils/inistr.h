#ifndef _INISTR_H
#define _INISTR_H

int INISTR_ReadSection(const char *lines, int num, char *section);
int INISTR_ReadNames(const char *lines, const char *section, int num, char *name);
void INISTR_ReadString(const char *lines, const char *section, const char *name, char *value, const char *def, int len);
void INISTR_ReadInteger(const char *lines, const char *section, const char *name, signed long *value, signed long def);

#define INISTR_EasyReadString(a,b,c,d) INISTR_ReadString(a,b,c,d,"",sizeof(d))

#ifndef INISTR_MAXLINE_LEN
	#define INISTR_MAXLINE_LEN 256
#endif

#ifndef INISTR_SEPARATOR
	#define INISTR_SEPARATOR   '='
#endif

#endif
