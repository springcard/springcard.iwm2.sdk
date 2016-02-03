#include "regfile.h"
#include "stri.h"
#include "strl.h"

#ifdef WIN32
#include <windows.h>

typedef struct
{
  const char *prefix;
  HKEY       hkey;
} HKEY_DEF_ST;

static const HKEY_DEF_ST hkey_defs[] = 
{
  { "HKLM",               HKEY_LOCAL_MACHINE },
  { "HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE },
  { "HKCU",               HKEY_CURRENT_USER  },
  { "HKEY_CURRENT_USER",  HKEY_CURRENT_USER  },
  { "",                   0                  }
};

BOOL translate_key_name(const char *kname, HKEY *k_hkey, const char **k_rname)
{
  int i;

  for (i=0; strlen(hkey_defs[i].prefix); i++)
  {
    if (!sc_strnicmp(kname, hkey_defs[i].prefix, strlen(hkey_defs[i].prefix)))
    {
      *k_hkey  = hkey_defs[i].hkey;
      *k_rname = kname + strlen(hkey_defs[i].prefix) + 1;
      return TRUE;
    }
  }

  return FALSE;
}

int REGFILE_ReadSections(const char *kname, int num, char *name, int len)
{
  HKEY        k_hkey;
  const char *k_rname;

  HKEY        hKey;
  DWORD       count, size;

  if (!translate_key_name(kname, &k_hkey, &k_rname)) return -1;

  if (RegOpenKeyExA(k_hkey, k_rname, 0, KEY_READ|KEY_EXECUTE, &hKey) != ERROR_SUCCESS) return -1;

  if (RegQueryInfoKey(hKey, NULL, NULL, NULL, &count, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
  {
    RegCloseKey(hKey);
    return -1;
  }

  if ((DWORD) num >= count) return 0;

  size = len;
  if (RegEnumKeyExA(hKey, num, name, &size, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
  {
    RegCloseKey(hKey);
    return -1;
  }


  RegCloseKey(hKey);
  return 1;
}

void REGFILE_ReadString(const char *kname, const char *name, char *value, const char *def, int len)
{
  HKEY        k_hkey;
  const char *k_rname;

  HKEY        hKey;
  DWORD       size;
	DWORD       type;

  if (!translate_key_name(kname, &k_hkey, &k_rname))
	  goto use_default;

  if (RegOpenKeyExA(k_hkey, k_rname, 0, KEY_READ|KEY_EXECUTE, &hKey) != ERROR_SUCCESS)
	  goto use_default;

  size = len;
  if (RegQueryValueExA(hKey, name, NULL, &type, (LPBYTE) value, &size) != ERROR_SUCCESS)
	  goto use_default;
		
	if ((type != REG_SZ) && (type != REG_EXPAND_SZ))
	  goto use_default;

  RegCloseKey(hKey);
  return;

use_default:
  if (def != NULL)
    strlcpy(value, def, len);
}

void REGFILE_WriteString(const char *kname, const char *name, const char *value)
{
  HKEY        k_hkey;
  const char *k_rname;

  HKEY        hKey;

	if (value == NULL) return;

  if (!translate_key_name(kname, &k_hkey, &k_rname)) return;

  if (RegOpenKeyExA(k_hkey, k_rname, 0, KEY_SET_VALUE|KEY_EXECUTE, &hKey) != ERROR_SUCCESS) return;

  RegSetValueExA(hKey, name, 0, REG_SZ, (LPBYTE) value, strlen(value) + 1);

  RegCloseKey(hKey);
}

void REGFILE_ReadDWord(const char *kname, const char *name, unsigned long *value, unsigned long def)
{
  HKEY        k_hkey;
  const char *k_rname;

  HKEY        hKey;
  DWORD       size;
	DWORD       type;

	LONG rc;

	if (value == NULL) return;

  if (!translate_key_name(kname, &k_hkey, &k_rname)) return;

  rc = RegOpenKeyExA(k_hkey, k_rname, 0, KEY_READ|KEY_EXECUTE, &hKey);
	if (rc != ERROR_SUCCESS) return;

  size = sizeof(unsigned long);
  rc = RegQueryValueExA(hKey, name, NULL, &type, (LPBYTE) value, &size);
	if ((rc != ERROR_SUCCESS) || (type != REG_DWORD))
  {
    *value = def;
  }

  RegCloseKey(hKey);
}

void REGFILE_WriteDWord(const char *kname, const char *name, unsigned long value)
{
  HKEY        k_hkey;
  const char *k_rname;

  HKEY        hKey;

  if (!translate_key_name(kname, &k_hkey, &k_rname)) return;

  if (RegOpenKeyExA(k_hkey, k_rname, 0, KEY_SET_VALUE|KEY_EXECUTE, &hKey) != ERROR_SUCCESS) return;

  RegSetValueExA(hKey, name, 0, REG_DWORD, (LPBYTE) &value, sizeof(unsigned long));

  RegCloseKey(hKey);
}

static BOOL RegDeleteKeyNT(HKEY hStartKey, const char *pKeyName)
{
  DWORD   dwRtn, dwSubKeyLength;
  char    *pSubKey = NULL;
  char    szSubKey[MAX_PATH]; // (256) this should be dynamic.
  HKEY    hKey;
  BOOL    rc = FALSE;

  /* Do not allow NULL or empty key name */
  if (pKeyName && strlen(pKeyName))
  {
    if ((dwRtn=RegOpenKeyExA(hStartKey, pKeyName, 0, KEY_ENUMERATE_SUB_KEYS | DELETE, &hKey )) == ERROR_SUCCESS)
    {
      while (dwRtn == ERROR_SUCCESS )
      {
        dwSubKeyLength = MAX_PATH;

        dwRtn=RegEnumKeyExA(
                           hKey,
                           0,       // always index zero
                           szSubKey,
                           &dwSubKeyLength,
                           NULL,
                           NULL,
                           NULL,
                           NULL
                         );

        if (dwRtn == ERROR_NO_MORE_ITEMS)
        {
          dwRtn = RegDeleteKeyA(hStartKey, pKeyName);
          if (dwRtn == ERROR_SUCCESS)
            rc = TRUE;
          break;
        } else
        if (dwRtn == ERROR_SUCCESS)
        {
          RegDeleteKeyNT(hKey, szSubKey);
        }
      }
      RegCloseKey(hKey);
    }
  }

  return rc;
}


int REGFILE_EraseSection(const char *kname)
{
  HKEY        k_hkey;
  const char *k_rname;

  if (!translate_key_name(kname, &k_hkey, &k_rname)) return -1;

  if (RegDeleteKeyNT(k_hkey, k_rname)) return 1;

  return 0;
}

//void REGFILE_WriteString(const char *kname, const char *name, const char *value);
//void REGFILE_WriteDWord(const char *kname, const char *name, DWORD value);
//void REGFILE_CreateSection(const char *kname);

#endif
