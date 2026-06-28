/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved. \n
 * Description: provide pairinfo manager header for livemic.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-03-23, Create file. \n
 */
#ifndef UX_BS_SLE_DB_LIVEMIC_H
#define UX_BS_SLE_DB_LIVEMIC_H

#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)

#include "stddef.h"
#include "stdbool.h"
#include "errcode.h"
#include "bc_api.h"
#include "key_struct.h"
#include "ux_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ux内部使用的配对信息结构，读写nv时需要转化为ux_nv_livemic_pair_info_t，该结构体在直播麦产品的key_struct.h中有定义 */
typedef struct {
    uint16_t device_id;
    bs_2_4_g_paired_device_info_t info;
    uint16_t index;
} ux_sle_db_livemic_pair_info_t;

/* 从nv中读取配对信息 */
errcode_t ux_sle_db_livemic_read_pair_info(const uint16_t nv_id, ux_sle_db_livemic_pair_info_t *pair_info);

/* 将配对信息写入nv */
errcode_t ux_sle_db_livemic_write_pair_info(const uint16_t nv_id, const ux_sle_db_livemic_pair_info_t *pair_info);

/* 删除配对信息，向指定nv槽位写入全0 */
errcode_t ux_sle_db_livemic_delete_pair_info(const uint16_t nv_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

#endif /* UX_BS_SLE_DB_LIVEMIC_H */