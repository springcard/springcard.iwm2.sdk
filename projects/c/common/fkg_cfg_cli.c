#include "fkg_cfg_cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MK2_PROTOCOL    
extern int addr;
#endif

BOOL do_read(BYTE ident, BOOL show)
{
  BYTE  data[256];
  DWORD length;
  DWORD c;

  if (!fkg_cfg_read(ident, data, sizeof(data), &length))
  {
    printf("%02X : ", ident);
    printf("(read error)\n");
    return FALSE;
  }

  if (length || show)
  {
    printf("%02X : ", ident);
    if (length)
      for (c=0; c<length; c++) printf("%02X", data[c]);
    else
      printf("(empty)\n");
    printf("\n");
  }

  return TRUE;
}

BOOL do_write(BYTE ident, BYTE data[], DWORD length)
{
  if (!fkg_cfg_write(ident, data, length) && length)
  {
    printf("%02X : ", ident);
    printf("(write error)\n");
    return FALSE;
  }

  return TRUE;
}

BOOL do_dump(void)
{
  BYTE ident;

  for (ident=1; ident<255; ident++)
    if (!do_read(ident, FALSE))
      return FALSE;


  return TRUE;
}

BOOL get_value(const char *buffer, BYTE data[], DWORD *length)
{
  DWORD l = hstob(buffer, data, 256);

  if ((2 * l) == strlen(buffer))
  {
    *length = l;
    return TRUE;
  }

  return FALSE;
}

BOOL get_from_file(const char *filename, BYTE ident, BYTE data[], DWORD *length)
{
  char buffer[512+1];
  char ident_s[4+1];

  sprintf(ident_s, "%02X", ident);

  INIFILE_ReadString(filename, "raw", ident_s, buffer, "", sizeof(buffer));
  if (strlen(buffer))
    return get_value(buffer, data, length);

  CFGFILE_ReadString(filename, ident_s, buffer, "", sizeof(buffer));
  if (strlen(buffer))
    return get_value(buffer, data, length);

  sprintf(ident_s, "%02x", ident);

  INIFILE_ReadString(filename, "raw", ident_s, buffer, "", sizeof(buffer));
  if (strlen(buffer))
    return get_value(buffer, data, length);

  CFGFILE_ReadString(filename, ident_s, buffer, "", sizeof(buffer));
  if (strlen(buffer))
    return get_value(buffer, data, length);

  return FALSE;
}

int do_exit(int reason)
{
  return reason;
}

/*
 * main
 * ----
 */
int fkg_cfg_main(int argc, char **argv)
{
  BYTE  ident;
  int i;
  char *p;
	BOOL cmd_correct = FALSE;
  BOOL verbose = FALSE;
  BOOL success = TRUE;
  
#ifdef MK2_PROTOCOL      
	BOOL got_adr=FALSE;
#endif
  for (i=1; i<argc; i++)
  {
    if (!sc_stricmp("-v", argv[i]))
		{
			verbose = TRUE;
    }
#ifdef MK2_PROTOCOL    
    else
    if (!sc_stricmp("-a", argv[i]))
    {
      /* Got MK2 address */
      /* --------------- */
      while (++i<argc)
      {
        if (argv[i][0] == '-')
          break;
        addr = atoi(argv[i]);
				got_adr=TRUE;
      }
    }
#endif
  }

#ifdef MK2_PROTOCOL      
  if (!got_adr)
	{
    success = FALSE;
    goto done;
  }
#endif
  
  if (!fkg_cfg_connect())
    return do_exit(EXIT_FAILURE);

  for (i=1; i<argc; i++)
  {
		if (!sc_stricmp("-v", argv[i]) || !sc_stricmp("-f", argv[i]))
    {
      continue;
    } else
    if (!sc_stricmp("-a", argv[i]))
		{
			i++;
			continue;
		} else			
		if (!sc_stricmp("-d", argv[i]))
    {	      
      /* Dump all registers */
      /* ------------------ */
			printf("Retrieving values, please wait...\n");
      cmd_correct = TRUE;
      if (!do_dump())
        success = FALSE;
      goto done;

    } else
    if (!sc_stricmp("-s", argv[i]) || !sc_stricmp("-w", argv[i]))
    {
      /* Write a register */
      /* ---------------- */
      while (++i<argc)
      {
        BYTE  t[256];
        DWORD l = 0;

        if (argv[i][0] == '-')
          break;

        strtok(argv[i], "=");
        ident = htob(argv[i]);
        p = strtok(NULL, "");

        if ((p != NULL) && strlen(p))
        {
          l = hstob(p, t, sizeof(t));
          cmd_correct = TRUE;
        }
        
        printf("Write(%02x)\n", ident);
        if (!do_write(ident, t, l))
        {
          success = FALSE;
          goto done;
        }
#ifndef MK2_PROTOCOL
        if (!do_read(ident, TRUE))
        {
          success = FALSE;
          goto done;
        }
#endif
      }

    } else
    if (!sc_stricmp("-g", argv[i]) || !sc_stricmp("-r", argv[i]))
    {
      /* Read a register */
      /* --------------- */

      if (verbose)
        printf("Read(%s)\n", argv[i]);

      while (++i<argc)
      {
        if (argv[i][0] == '-')
          break;

        ident = htob(argv[i]);
        cmd_correct = TRUE;
        if (!do_read(ident, TRUE))
        {
          success = FALSE;
          goto done;
        }
      }
      
    } else
    if (!sc_stricmp("-sf", argv[i]))
    {
      /* Write from a file */
      /* ----------------- */

      char *filename;

      if (++i >= argc)
      {
        printf("A filename must be specified\n");
        success = FALSE;
        goto done;
      }

      filename = argv[i];

      printf("Writing configuration from %s\n", filename);

      for (ident=0; ident<255; ident++)
      {
        BYTE  t[256];
        DWORD l = 0;

        if (get_from_file(filename, ident, t, &l))
        {
          printf("Write %02X...\n", ident);
          cmd_correct = TRUE;
          if (!do_write(ident, t, l))
          {
            success = FALSE;
            goto done;
          }
        }
      }

    } else
    if (!sc_stricmp("-e", argv[i]))
    {
      /* Erase... */
      /* -------- */

      printf("Erasing the configuration...\n");
      cmd_correct = TRUE;
      for (ident=0; ident<255; ident++)
      {
        printf("Erase %02X...\n", ident);

        if (!do_write(ident, NULL, 0))
        {
          success = FALSE;
          goto done;
        }
      }

    } else
    if (!sc_stricmp("-z", argv[i]))
    {
      /* Reset... */
      /* -------- */

      printf("Resetting the device...\n");
      cmd_correct = TRUE;
      success = fkg_cfg_reset();

    } else
    {
      printf("Unsupported command '%s'\n", argv[i]);
    }
  }


done:
  fkg_cfg_disconnect();
  
	if (!cmd_correct)
    fkg_cfg_usage();

  if (cmd_correct && success)
    return do_exit(EXIT_SUCCESS);

  return do_exit(EXIT_FAILURE);
}

void fkg_cfg_usage_common(void)
{
	printf("Valid ACTIONs are:\n");
  printf("\t-d            dump configuration registers\n");
	printf("\t-r XX         show value of register XX\n");
	printf("\t-s XX=YYYY... write register XX with value YYYY...\n");
  printf("\t-e            erase all configuration registers\n");
  printf("\t-sf FILE      write register(s) from config FILE\n");
  printf("\t-z            reset the device\n");
}
