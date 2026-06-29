/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_database.h
 * Description:  provide ux bs database header file.
 */
#ifndef UX_BS_SLE_DB_H
#define UX_BS_SLE_DB_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ux_bs_sle_db_save_single_pdl_info(void *param);
void ux_bs_sle_db_save_pdl_order(void *param);

void ux_bs_combo_db_save_single_pdl_info(void *param);
void ux_bs_combo_db_save_pdl_order(void *param);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_BS_SLE_DB_H