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
#include "ssf_persistence.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ux_bs_sle_grant_pdl_data_print(const bs_sle_nv_obj_data_t *data);
void ux_bs_sle_db_save_single_pdl_info(void *param);
void ux_bs_sle_db_save_pdl_order(void *param);
/**
 * 读取所有配对设备信息
 * @param pdl_info_list 输出参数，用于存储读取的所有设备信息
 * @param pdl_num 输出参数，用于存储读取的设备数量
 * @param max_pdl_num 最大pdl数量
 * @return 返回错误码
 */
errcode_t ux_bs_sle_db_read_all_pdl_info(bs_sle_nv_single_pdl_data_t *pdl_info_list,
    uint8_t *pdl_num, uint8_t max_pdl_num);

/**
 * 读取配对设备的连接顺序
 * @param param 输出参数，用于存储读取的连接顺序信息
 * @return 返回错误码
 */
errcode_t ux_bs_sle_db_read_pdl_order(void *param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_BS_SLE_DB_H