#ifndef __FKG_CFG_CLI_H__
#define __FKG_CFG_CLI_H__

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib-c/utils/types.h"
#include "lib-c/utils/stri.h"
#include "lib-c/utils/strl.h"
#include "lib-c/utils/inifile.h"
#include "lib-c/utils/cfgfile.h"
#include "lib-c/utils/binconvert.h"
#include "lib-c/network/network.h"
#include "products/iwm2/iwm2_proto_mk2.h"

BOOL fkg_cfg_connect(void);
void fkg_cfg_disconnect(void);

BOOL fkg_cfg_reset(void);
BOOL fkg_cfg_write(BYTE ident, const BYTE data[], DWORD length);
BOOL fkg_cfg_read(BYTE ident, BYTE data[], DWORD maxlength, DWORD *curlength);

void fkg_cfg_usage(void);
void fkg_cfg_usage_common(void);
int fkg_cfg_main(int argc, char **argv);

#endif

