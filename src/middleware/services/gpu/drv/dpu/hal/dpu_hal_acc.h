/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#ifndef DPU_HAL_ACC_H
#define DPU_HAL_ACC_H

#include "td_base.h"
#include "dpu_reg_define.h"

#define ACC_YCNT32_DATA_NUM 20
typedef struct {
    td_u32 acc_y_sum;
    td_u32 acc_ydiffd1_sum;
    td_u32 acc_ydiffd2_sum;
    td_u32 acc_sat_sum;
    td_u32 acc_sat_hist_sum;
    td_u32 acc_sat_hist_cum;
    td_u32 ycnt32_data[ACC_YCNT32_DATA_NUM];
} dpu_hal_acc_info;

td_void dpu_reg_get_acc_info(volatile dpu_regs_type *dpu_reg, dpu_hal_acc_info *acc_info);
td_void dpu_reg_set_acc_lut(volatile dpu_regs_type *dpu_reg, td_s32 acc_lut[], td_u32 num);
td_void dpu_reg_set_acc_en(volatile dpu_regs_type *dpu_reg, td_u32 en);
td_void dpu_reg_set_acc_hpos(volatile dpu_regs_type *dpu_reg, td_u32 acc_hstart, td_u32 acc_hend);
td_void dpu_reg_set_acc_vpos(volatile dpu_regs_type *dpu_reg, td_u32 acc_vstart, td_u32 acc_vend);
td_void dpu_reg_set_acc_stat_en(volatile dpu_regs_type *dpu_reg, td_u32 en);
td_void dpu_reg_set_acc_stat_hpos(volatile dpu_regs_type *dpu_reg, td_u32 acc_stat_hstart, td_u32 acc_stat_hend);
td_void dpu_reg_set_acc_stat_vpos(volatile dpu_regs_type *dpu_reg, td_u32 acc_stat_vstart, td_u32 acc_stat_vend);
td_void dpu_reg_set_acc_stat_shifthend(volatile dpu_regs_type *dpu_reg, td_u32 acc_stat_shifthend);

#endif