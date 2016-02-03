/*
 *  common/lib-c/utils/cfgfile, config file utility
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

#ifndef _REGFILE_H
#define _REGFILE_H

int REGFILE_ReadSections(const char *kname, int num, char *name, int len);
void REGFILE_ReadString(const char *kname, const char *name, char *value, const char *def, int len);
void REGFILE_ReadDWord(const char *kname, const char *name, unsigned long *value, unsigned long def);
void REGFILE_WriteString(const char *kname, const char *name, const char *value);
void REGFILE_WriteDWord(const char *kname, const char *name, unsigned long value);
void REGFILE_CreateSection(const char *kname);
int REGFILE_EraseSection(const char *kname);

#endif
