#ifndef __FKG485_DLL_I_H__
#define __FKG485_DLL_I_H__

#ifdef WIN32
#define FKG485_LIB __declspec( dllexport )
#endif

#include "../../common/fkg_i.h"
#include "../../common/485/fkg485_i.h"
#include "../inc/fkg485_dll.h"

#endif
