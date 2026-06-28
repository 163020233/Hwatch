/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef __HAL_VICAP_REG_CHN_H__
#define __HAL_VICAP_REG_CHN_H__

#include "drv_mcp_dependency.h"
#include "vicap_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

td_void vicap_reg_set_ch_crop_en(volatile vicap_regs_type *reg, td_u32 enable);
td_void vicap_reg_set_ch_crop_xy(volatile vicap_regs_type *reg, td_u32 x, td_u32 y);
td_void vicap_reg_set_ch_crop_wh(volatile vicap_regs_type *reg, td_u32 w, td_u32 h);

td_void vicap_reg_set_wch_y_ctrl(volatile vicap_regs_type *reg, td_u32 y_en, td_u32 bitw,
    td_u32 en_b32);
td_void vicap_reg_set_wch_y_wh(volatile vicap_regs_type *reg, td_u32 w, td_u32 h);
td_void vicap_reg_set_wch_y_buf(volatile vicap_regs_type *reg, td_u64 y_addr, td_u32 y_stride);
td_void vicap_reg_set_wch_c_ctrl(volatile vicap_regs_type *reg, td_u32 c_en, td_u32 bitw);
td_void vicap_reg_set_wch_c_wh(volatile vicap_regs_type *reg, td_u32 w, td_u32 h);
td_void vicap_reg_set_wch_c_buf(volatile vicap_regs_type *reg, td_u64 c_addr, td_u32 c_stride);
td_void vicap_reg_set_wch_en(volatile vicap_regs_type *reg, td_bool enable);

td_void vicap_reg_set_chn_skip_cfg_y(volatile vicap_regs_type *reg, td_u32 skip, td_u32 w, td_u32 h);
td_void vicap_reg_set_chn_skip_cfg_c(volatile vicap_regs_type *reg, td_u32 skip, td_u32 w, td_u32 h);

td_void vicap_reg_set_wch_reg_newer(volatile vicap_regs_type *reg);
td_void vicap_reg_set_wch_finish_mode(volatile vicap_regs_type *reg, td_u32 mode);

td_void vicap_reg_set_chn_timestamp_en(volatile vicap_regs_type *reg, td_bool timestamp_en);
td_u64 vicap_reg_get_timestamp(td_void);
td_u32 vicap_reg_get_chn_int_status(volatile vicap_regs_type *reg);
td_void vicap_reg_clear_chn_int_status(volatile vicap_regs_type *reg, td_u32 int_status);
td_void vicap_reg_set_chn_int_mask(volatile vicap_regs_type *reg,  td_u32 int_status);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
