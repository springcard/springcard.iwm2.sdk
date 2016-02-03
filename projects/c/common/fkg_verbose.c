#include "fkg_i.h"

#include <stdarg.h>
#include <time.h>

static BYTE trace_level = TRACE_OFF;
static char *trace_file = NULL;
static char last_trace_head = '\0';

void FKG_SetVerbose(BYTE level, const char *filename)
{
  if (trace_file != NULL)
  {
    free(trace_file);
    trace_file = NULL;
  }

  if (filename != NULL)
	{
		FKG_Trace(trace_level, "Trace file set to %s", filename);
		trace_file = sc_strdup(filename);
		FKG_Trace(trace_level, "Trace file set to %s", trace_file);
	}

	trace_level = level;    
	FKG_Trace(trace_level, "Trace level set to %02X", level);
}

void FKG_Trace(BYTE level, const char *fmt, ...)
{
  va_list arg_ptr;
  char    line[MAX_PATH];
  char   *p;
  FILE   *file_out;
  clock_t c;

  if (level > trace_level) return; /* No need to trace this */

  if (trace_file == NULL)
	{
		/* Output to console */
		file_out = stdout;
	} else
	{
		/* Output to file */
    file_out = fopen(trace_file, "at+");
    if (file_out == NULL) return;
  }

  /* Prepare the line */
  if (fmt != NULL)
  {
    va_start(arg_ptr, fmt);		
    vsprintf(line, fmt, arg_ptr);
    va_end(arg_ptr);   
	}
  
  if (fmt == NULL)
  {
    fprintf(file_out, "\n");
    last_trace_head = '\0';
  } else
  {
    p = line;
    switch (*p)
    {
      case '+':
      case '-':
        if (last_trace_head != *p)
        {
          fprintf(file_out, "\n");
          last_trace_head = *p;
        }
        break;
  
      case '>':
      case ':':
      case '.':
        break;
  
      case '_':
        p++;
        break;
  
      default:
        c  = clock();
        c *= 1000;
        c /= CLOCKS_PER_SEC;
        fprintf(file_out, "\n%03ld.%03ld\t", c / 1000, c % 1000);
        last_trace_head = '\0';
        break;
    }
    fprintf(file_out, "%s", p);
  }

  if ((file_out != stdout) && (file_out != stderr))
  	fclose(file_out);
  else
    fflush(file_out);
}

