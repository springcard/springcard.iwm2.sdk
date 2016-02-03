#ifndef __BINCONVERT_H__
#define __BINCONVERT_H__

char *btoh(char *s, BYTE b);
char *wtoh(char *s, WORD w);
char *dwtoh(char *s, DWORD dw);

BOOL ishexq(const char q);
BOOL ishexb(const char s[2]);
BYTE dtob2(const char s[2]);
WORD dtow4(const char s[4]);
void btod2(char s[2], BYTE b);

BYTE  htob(const char s[2]);
WORD  htow(const char s[4]);
DWORD htodw(const char s[8]);

WORD  hstob(const char *str, BYTE *data, WORD size);

BYTE  bcdtob(BYTE b);
WORD  bcdtow(WORD w);
DWORD bcdtodw(DWORD dw);

BYTE to_bcd(BYTE v);
BYTE from_bcd(BYTE v);

BOOL  isbcdb(BYTE v);

#endif
