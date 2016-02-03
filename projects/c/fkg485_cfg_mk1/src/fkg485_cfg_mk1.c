#include "../../common/fkg_cfg_cli.h"
#include "../../common/485/fkg485_i.h"

static const char *PROGNAME = "fkg485_cfg_mk1";
static char  *comm_name = NULL;
#ifdef WIN32
static HANDLE comm_handle;
#else
static int comm_handle;
#endif


BOOL fkg485_debug = FALSE;

int main(int argc, char **argv)
{
  int new_argc = 0;
  char **new_argv = NULL;

  if (argc > 1)
  {
#ifdef WIN32
    comm_name = argv[1];
#else
    char dev[30];
    memset(dev, 0, sizeof(dev));
    if ((strlen("/dev/") + strlen(argv[1])) < sizeof(dev))
    {
      sprintf(dev, "/dev/%s", argv[1]);
      comm_name = dev;
    } else
    {
      comm_name = argv[1];
    }
#endif
    new_argc = argc - 1;
    new_argv = &argv[1];
  }

  if (comm_name == NULL)
  {
    fkg_cfg_usage();
    return EXIT_FAILURE;
  }

  return fkg_cfg_main(new_argc, new_argv);
}

void fkg_cfg_usage(void)
{
  printf("Usage: %s comm_name ACTION\n", PROGNAME);
  fkg_cfg_usage_common();
}

BOOL fkg_cfg_connect(void)
{
  printf("Opening communication device '%s'\n", comm_name);

  if (!FKG485_OpenCommEx(&comm_handle, comm_name))
  {
    printf("ERROR - Failed to open comm. device '%s'\n", comm_name);
    return FALSE;
  }

  if (!FKG485_MK1_Probe(comm_handle))
  {
    printf("ERROR - FKG485 MK1 device not found!\n");
    return FALSE;
  }

  printf("Connected!\n");
  return TRUE;
}

void fkg_cfg_disconnect(void)
{
  FKG485_CloseCommEx(comm_handle);
}

BOOL fkg_cfg_reset(void)
{
  return FKG485_MK1_Command(comm_handle, "reset");
}

BOOL fkg_cfg_write(BYTE ident, const BYTE data[], DWORD length)
{
  char t[512];
  WORD i;

  if (length > 64)
    return FALSE;

	if (length > 0)
  {
		sprintf(t, "cfg%02X=", ident);
		for (i=0; i<length; i++)
			sprintf(&t[6 + 2*i], "%02X", data[i]);
	} else
	{
		sprintf(t, "cfg%02X=!!", ident);
	}

  if (!FKG485_MK1_Command(comm_handle, t))
    return FALSE;

  if (!FKG485_MK1_Response(comm_handle, t, sizeof(t)))
    return FALSE;

  return TRUE;
}

BOOL fkg_cfg_read(BYTE ident, BYTE data[], DWORD maxlength, DWORD *curlength)
{
  char c[32];
  char r[512];
  char *t;
  WORD i;

  sprintf(c, "cfg%02X", ident);

  if (!FKG485_MK1_Command(comm_handle, c))
    return FALSE;

  if (!FKG485_MK1_Response(comm_handle, r, sizeof(r)))
    return FALSE;

  if (strstr(r, "Failed") != NULL)
  {
    *curlength = 0;
    return TRUE;
  }

  sprintf(c, "%02X=", ident);

  t = strstr(r, c);
  if (t == NULL)
  {
    printf("'%s' not found in '%s'\n", c, r);
    return FALSE;
  }
  t += strlen(c);

  for (i=0; i<strlen(t); i++)
  {
    if ((t[i] == '\r') || (t[i] == '\n'))
    {
      t[i] = '\0';
      break;
    }
  }
  
  *curlength = hstob(t, data, (WORD) maxlength);
  return TRUE;
}

