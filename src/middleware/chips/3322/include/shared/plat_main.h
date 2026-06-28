/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: plat_main header file.
 * Author: CompanyName
 * Create: 2025-01
 */

#ifndef __PLAT_MAIN_H__
#define __PLAT_MAIN_H__

#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t plat_init_etc(void);
void plat_exit_etc(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
