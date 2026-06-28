/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef __HAL_VICAP_REG_GLOBAL_H__
#define __HAL_VICAP_REG_GLOBAL_H__

#include "drv_mcp_dependency.h"
#include "vicap_reg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

// mac
td_void vicap_reg_set_mac_mmu_ctrl(volatile vicap_regs_type *reg, td_bool is_mmu);

td_void vicap_reg_set_outstanding(volatile vicap_regs_type *reg, td_u32 routstanding, td_u32 woutstanding);
td_void vicap_reg_low_power_enable(volatile vicap_regs_type *reg, td_bool enable);

td_void vicap_reg_set_int_mask(volatile vicap_regs_type *reg, td_u32 int_mask);

td_u32 vicap_reg_get_cap_int_status(volatile vicap_regs_type *reg);
td_void vicap_reg_set_isp_sel(volatile vicap_regs_type *reg, td_s32 cap_id, td_u32 sel);
td_void vicap_reg_set_online_sel(volatile vicap_regs_type *reg, td_u32 wdr_sel);

td_void vicap_reg_set_afifo_crop_xy(volatile vicap_regs_type *reg, td_u32 x, td_u32 y);
td_void vicap_reg_set_afifo_crop_wh(volatile vicap_regs_type *reg, td_u32 w, td_u32 h);

td_void vicap_reg_set_isp0_mode(volatile vicap_regs_type *reg, td_bool isp_bypass, td_u32 mode);
td_void vicap_reg_set_isp1_mode(volatile vicap_regs_type *reg, td_bool isp_bypass, td_u32 mode);

td_void vicap_reg_set_line_afifo_en(volatile vicap_regs_type *reg, td_bool fifo_en);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
