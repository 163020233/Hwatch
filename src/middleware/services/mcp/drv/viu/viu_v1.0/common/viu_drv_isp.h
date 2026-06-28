/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2016-2021. All rights reserved.
 * Description: viu_drv_isp.h
 * Author: AuthorNameMagicTag
 * Create: 2016-11-17
 */

#ifndef VIU_DRV_ISP_H
#define VIU_DRV_ISP_H

#include "viproc_reg.h"
#include "drv_isp_ext.h"
#include "hal_viproc_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_u64 viu_drv_get_isp_reg_size(isp_reg_type type);

td_void viu_drv_enable_isp_algorithm(td_s32 viu_pipe_id, td_u32 block, viproc_regs_type *node,
    hal_viproc_cfg *viproc_cfg);

td_void viu_drv_set_be_lut2_stt_width(td_s32 viu_pipe_id, td_u32 block, viproc_regs_type *node);

td_void viu_drv_update_isp_sync_param(td_s32 viu_pipe_id, td_u32 block);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif