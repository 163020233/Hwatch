/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * File          ux_bs_bt_db.h
 * Description: ux bt nv write event.
 */
#ifndef UX_BS_BT_DB_H
#define UX_BS_BT_DB_H
#include <stdint.h>
#include "bt_service.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void ux_bs_bt_db_save_single_pdl_info(void *param); /* 保存单条设备配对信息 bt_nv_mobile_single_pdl_data_s */
void ux_bs_bt_db_save_multi_pdl_info(void *param); /* 保存多条设备配对信息 bt_nv_mobile_multi_pdl_data_s */
void ux_bs_bt_db_save_remainder_multi_pdl_info(void *param); /* 保存多条设备配对信息 bt_nv_mobile_multi_pdl_data_s */
void ux_bs_bt_db_save_pdl_order(void *param); /* 保存设备回连顺序 bt_nv_mobile_conn_order_s */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

