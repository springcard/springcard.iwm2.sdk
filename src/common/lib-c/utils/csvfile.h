#ifndef _CSVFILE_H
#define _CSVFILE_H

typedef int (*CSVFILE_PARSE_CALLBACK) (int line, int columnc, const char **columnv);

int CSVFILE_Foreach(const char *filename, CSVFILE_PARSE_CALLBACK callback);

#ifndef CSVFILE_MAXLINE_LEN
	#define CSVFILE_MAXLINE_LEN 512
#endif

#ifndef CSVFILE_SEPARATOR
	#define CSVFILE_SEPARATOR ';'
#endif

#endif
