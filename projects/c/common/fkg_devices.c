#include "fkg_i.h"

typedef struct _FKG_DEVICE_CTX_LIST_ST
{
  FKG_DEVICE_CTX_ST *device_ctx;
  struct _FKG_DEVICE_CTX_LIST_ST *next;
} FKG_DEVICE_CTX_LIST_ST;

FKG_DEVICE_CTX_LIST_ST *fkg_device_list = NULL;

FKG_DEVICE_CTX_ST *FKG_DeviceFirst(void)
{
  if (fkg_device_list == NULL)
    return NULL;
  return fkg_device_list->device_ctx;
}

FKG_DEVICE_CTX_ST *FKG_DeviceNext(FKG_DEVICE_CTX_ST *current)
{
  FKG_DEVICE_CTX_LIST_ST *p = fkg_device_list;

  while (p != NULL)
  {
    if (p->device_ctx == current)
    {
      if (p->next == NULL)
        return NULL;
      p = p->next;
      return p->device_ctx;
    }
    p = p->next;
  }

  return NULL;
}

FKG_DEVICE_CTX_ST *FKG_FindDevice(const char *device_name)
{
  FKG_DEVICE_CTX_LIST_ST *p = fkg_device_list;

  while (p != NULL)
  {
    if (!strcmp(p->device_ctx->Name, device_name))
      return p->device_ctx;
    p = p->next;
  }

  return NULL;
}

DWORD FKG_GetDeviceCount(void)
{
  DWORD r = 0;
  FKG_DEVICE_CTX_LIST_ST *p = fkg_device_list;

  while (p != NULL)
  {
    r++;
    p = p->next;
  }

  return r;
}

BOOL FKG_RemoveDevice(FKG_DEVICE_CTX_ST *device_ctx)
{
  FKG_DEVICE_CTX_LIST_ST *p = fkg_device_list;
  FKG_DEVICE_CTX_LIST_ST *p_prev = NULL;

  while (p != NULL)
  {
    if (p->device_ctx == device_ctx)
    {
      if (p_prev == NULL)
      {
        fkg_device_list = p->next;
      } else
      {
        p_prev->next = p->next;
      }
      free(p);
      return TRUE;
    }
    p_prev = p;
    p = p->next;
  }

  return FALSE;
}

BOOL FKG_InsertDevice(FKG_DEVICE_CTX_ST *device_ctx)
{
  FKG_DEVICE_CTX_LIST_ST *p_new, *p = fkg_device_list;

  if ((device_ctx == NULL) || (device_ctx->Name == NULL))
    return FALSE;

  if (FKG_FindDevice(device_ctx->Name) != NULL)
    return FALSE;

  p_new = calloc(1, sizeof(FKG_DEVICE_CTX_LIST_ST));
  if (p_new == NULL)
    return FALSE;

  p_new->device_ctx = device_ctx;

  if (p == NULL)
  {
    fkg_device_list = p_new;
    return TRUE;
  }

  while (p->next != NULL)
  {
    p = p->next;
  }

  p->next = p_new;
  return TRUE;
}
