#include "fkg_i.h"

void FKG_OnTag(int number, const BYTE tag[], int length)
{
  int i;

  printf("%04d **** Tag ", number);
  for (i=0; i<length; i++)
    printf("%02X", tag[i]);
  printf(" read\n");
}

void FKG_OnTagInserted(int number, const BYTE tag[], int length)
{
  int i;

  printf("%04d **** Tag ", number);
  for (i=0; i<length; i++)
    printf("%02X", tag[i]);
  printf(" inserted\n");
}

void FKG_OnTagRemoved(int number)
{
  printf("%04d **** Tag removed", number);
}

void FKG_OnPin(int number, const BYTE pin[], int length)
{
  static const char pin_matrix[] = "0123456789*#....";
  int i;

  printf("04d **** PIN ");
  for (i=0; i<length; i++)
  {
    if (pin[i] < (sizeof(pin_matrix)-1))
      printf("%c", pin_matrix[pin[i]]);
    else
      printf("?");
  }
}

void FKG_OnAlertBegin(int number, BYTE alerts)
{
  printf("%04d **** Alert %02X", number, alerts);
}

void FKG_OnAlertEnd(int number)
{
  printf("%04d **** Alert end", number);
}


/*
BOOL FKG_OnConnect(FKG_THRD_CTX_ST *thrd_ctx)
{
  UTL_Trace(NULL, VRB_TRACE, "P%02d:RDR%02d:connecte", thrd_ctx->por_idx, thrd_ctx->rdr_idx);
  EVT_Record(EVT_SRC_ACS, thrd_ctx->por_idx, EVT_JESS_CONNECT_OK, &thrd_ctx->rdr_idx, 1);
  return TRUE;
}

BOOL FKG_OnDisconnect(FKG_THRD_CTX_ST *thrd_ctx)
{
  UTL_Trace(NULL, VRB_TRACE, "P%02d:RDR%02d:deconnecte", thrd_ctx->por_idx, thrd_ctx->rdr_idx);
  EVT_Record(EVT_SRC_ACS, thrd_ctx->por_idx, EVT_JESS_CONNECT_KO, &thrd_ctx->rdr_idx, 1);
  return TRUE;
}
*/