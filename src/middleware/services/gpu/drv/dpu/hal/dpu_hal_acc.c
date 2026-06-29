/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. */

#include "dpu_hal_acc.h"

td_void dpu_reg_get_acc_info(volatile dpu_regs_type *dpu_reg, dpu_hal_acc_info *acc_info)
{
    for (td_u32 i = 0; i < ACC_YCNT32_DATA_NUM; i++) {
        acc_info->ycnt32_data[i] = dpu_reg_read(&(dpu_reg->vhdaccyhist_0.u32) + i);
    }

    acc_info->acc_y_sum = dpu_reg_read(&(dpu_reg->vhdaccysum.u32));
    acc_info->acc_ydiffd1_sum = dpu_reg_read(&(dpu_reg->vhdaccydiffd1sum.u32));
    acc_info->acc_ydiffd2_sum = dpu_reg_read(&(dpu_reg->vhdaccydiffd2sum.u32));
    acc_info->acc_sat_sum = dpu_reg_read(&(dpu_reg->vhdaccsatsum.u32));
    acc_info->acc_sat_hist_sum = dpu_reg_read(&(dpu_reg->vhdaccsathistsum.u32));
    acc_info->acc_sat_hist_cum = dpu_reg_read(&(dpu_reg->vhdaccsathistcum.u32));
}

td_void dpu_reg_set_acc_lut(volatile dpu_regs_type *dpu_reg, td_s32 acc_lut[], td_u32 num)
{
    for (td_u32 offset = 0, lut_index = 0; lut_index < num; offset++, lut_index += 3) { // 3 offset
        u_vhdacclut1 vhdacclut1;
        vhdacclut1.u32 = dpu_reg_read((&(dpu_reg->vhdacclut1.u32) + offset));
        vhdacclut1.bits.table_data0 = (td_u32)acc_lut[lut_index];
        vhdacclut1.bits.table_data1 = (td_u32)acc_lut[lut_index + 1];
        vhdacclut1.bits.table_data2 = (td_u32)acc_lut[lut_index + 2];  // 2 offset
        dpu_reg_write((&(dpu_reg->vhdacclut1.u32) + offset), vhdacclut1.u32);
    }
}

td_void dpu_reg_set_acc_en(volatile dpu_regs_type *dpu_reg, td_u32 en)
{
    u_vhdaccctrl vhdaccctrl;
    vhdaccctrl.u32 = dpu_reg_read(&(dpu_reg->vhdaccctrl.u32));
    vhdaccctrl.bits.acc_en = en;
    vhdaccctrl.bits.acc_ck_gt_en = en;
    dpu_reg_write(&(dpu_reg->vhdaccctrl.u32), vhdaccctrl.u32);
}

td_void dpu_reg_set_acc_hpos(volatile dpu_regs_type *dpu_reg, td_u32 acc_hstart, td_u32 acc_hend)
{
    u_vhdacchpos vhdacchpos;
    vhdacchpos.u32 = dpu_reg_read(&(dpu_reg->vhdacchpos.u32));
    vhdacchpos.bits.acc_hstart = acc_hstart;
    vhdacchpos.bits.acc_hend = acc_hend;
    dpu_reg_write(&(dpu_reg->vhdacchpos.u32), vhdacchpos.u32);
}

td_void dpu_reg_set_acc_vpos(volatile dpu_regs_type *dpu_reg, td_u32 acc_vstart, td_u32 acc_vend)
{
    u_vhdaccvpos vhdaccvpos;
    vhdaccvpos.u32 = dpu_reg_read(&(dpu_reg->vhdaccvpos.u32));
    vhdaccvpos.bits.acc_vstart = acc_vstart;
    vhdaccvpos.bits.acc_vend = acc_vend;
    dpu_reg_write(&(dpu_reg->vhdaccvpos.u32), vhdaccvpos.u32);
}

td_void dpu_reg_set_acc_stat_en(volatile dpu_regs_type *dpu_reg, td_u32 en)
{
    u_vhdaccstatctrl vhdaccstatctrl;
    vhdaccstatctrl.u32 = dpu_reg_read(&(dpu_reg->vhdaccstatctrl.u32));
    vhdaccstatctrl.bits.acc_stat_en = en;
    vhdaccstatctrl.bits.acc_stat_ck_gt_en = en;
    dpu_reg_write(&(dpu_reg->vhdaccstatctrl.u32), vhdaccstatctrl.u32);
}

td_void dpu_reg_set_acc_stat_hpos(volatile dpu_regs_type *dpu_reg, td_u32 acc_stat_hstart, td_u32 acc_stat_hend)
{
    u_vhdaccstathpos vhdaccstathpos;
    vhdaccstathpos.u32 = dpu_reg_read(&(dpu_reg->vhdaccstathpos.u32));
    vhdaccstathpos.bits.acc_stat_hstart = acc_stat_hstart;
    vhdaccstathpos.bits.acc_stat_hend = acc_stat_hend;
    dpu_reg_write(&(dpu_reg->vhdaccstathpos.u32), vhdaccstathpos.u32);
}

td_void dpu_reg_set_acc_stat_vpos(volatile dpu_regs_type *dpu_reg, td_u32 acc_stat_vstart, td_u32 acc_stat_vend)
{
    u_vhdaccstatvpos vhdaccstatvpos;
    vhdaccstatvpos.u32 = dpu_reg_read(&(dpu_reg->vhdaccstatvpos.u32));
    vhdaccstatvpos.bits.acc_stat_vstart = acc_stat_vstart;
    vhdaccstatvpos.bits.acc_stat_vend = acc_stat_vend;
    dpu_reg_write(&(dpu_reg->vhdaccstatvpos.u32), vhdaccstatvpos.u32);
}

td_void dpu_reg_set_acc_stat_shifthend(volatile dpu_regs_type *dpu_reg, td_u32 acc_stat_shifthend)
{
    u_vhdaccshiftd vhdaccshiftd;
    vhdaccshiftd.u32 = dpu_reg_read(&(dpu_reg->vhdaccshiftd.u32));
    vhdaccshiftd.bits.acc_stat_shifthend = acc_stat_shifthend;
    dpu_reg_write(&(dpu_reg->vhdaccshiftd.u32), vhdaccshiftd.u32);
}