/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PLAT HCC INIT
 * Author: CompanyName
 */

#ifndef PLAT_HCC_INIT_H
#define PLAT_HCC_INIT_H

#include "errcode.h"

uint8_t get_hcc_id(void);
errcode_t hcc_init_etc(void);
void hcc_deinit_etc(void);

#endif
