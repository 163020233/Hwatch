/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: wdr_drv.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef WDR_DRV_H
#define WDR_DRV_H

#include "viu_hal.h"
#include "drv_viu_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_u32 inv_time;
    td_u64 last_time;
} viu_wdr_time_info;

typedef struct {
    viu_wdr_time_info wch_int_time;
    viu_wdr_time_info combine_int_time;
    td_u32 wch_cc_err_cnt;
    td_u32 combine_cc_err_cnt;
    td_u32 wch_int_cnt;
    td_u32 combine_int_cnt;
} viu_wdr_dbg_info;

typedef struct {
    ext_isp_wdr_mode wdr_mode;
    td_u32 pool_id; /* pool id */
    td_u32 buf_size; /* buf大小 */
    td_u32 width; /* 图像宽度 */
    td_u32 height; /* 图像高度 */
    viu_wdr_dbg_info dbg_info;
} viu_dev_wdr_attr;

/****************************************************************************
 * FUNCTION DEFINITION                                                      *
 ****************************************************************************/
td_s32 viu_drv_check_wdr_attr(td_s32 viu_dev_id, const ext_viu_wdr_attr *wdr_attr, const ext_isp_size *size);
td_void viu_drv_get_dev_size(td_s32 viu_dev_id, ext_isp_size *size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
