/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hal_reg of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#include "hal_vicap_reg_chn.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

td_void vicap_reg_set_ch_crop_en(volatile vicap_regs_type *reg, td_u32 enable)
{
    u_ch_crop_cfg t;

    t.u32 = reg->ch_crop_cfg.u32;
    t.bits.ch_crop_n0_en = enable;
    reg->ch_crop_cfg.u32 = t.u32;
}

td_void vicap_reg_set_ch_crop_xy(volatile vicap_regs_type *reg, td_u32 x, td_u32 y)
{
    u_ch_crop0_start t;

    t.u32 = reg->ch_crop0_start.u32;
    t.bits.ch_crop_x_start = x;
    t.bits.ch_crop_y_start = y;
    reg->ch_crop0_start.u32 = t.u32;
}

td_void vicap_reg_set_ch_crop_wh(volatile vicap_regs_type *reg, td_u32 w, td_u32 h)
{
    u_ch_crop0_size t;

    t.u32 = reg->ch_crop0_size.u32;
    t.bits.ch_crop_width = w - 1;
    t.bits.ch_crop_height = h - 1;
    reg->ch_crop0_size.u32 = t.u32;
}

td_void vicap_reg_set_wch_finish_mode(volatile vicap_regs_type *reg, td_u32 mode)
{
    u_ch_ctrl ut_ch_ctrl;

    ut_ch_ctrl.u32 = reg->ch_ctrl.u32;
    ut_ch_ctrl.bits.ch_finish_mode = mode;
    reg->ch_ctrl.u32 = ut_ch_ctrl.u32;
}

td_void vicap_reg_set_wch_en(volatile vicap_regs_type *reg, td_bool enable)
{
    u_ch_ctrl t;

    t.u32 = reg->ch_ctrl.u32;
    t.bits.ch_enable = enable;
    reg->ch_ctrl.u32 = t.u32;
}

td_void vicap_reg_set_wch_y_ctrl(volatile vicap_regs_type *reg, td_u32 y_en, td_u32 bitw,
    td_u32 en_b32)
{
    u_ch_wch_y_cfg t;

    t.u32 = reg->ch_wch_y_cfg.u32;
    t.bits.wch_y_enable = y_en;
    t.bits.wch_y_bit_width = bitw;
    t.bits.wch_y_32b_align = en_b32;
    reg->ch_wch_y_cfg.u32 = t.u32;
}

td_void vicap_reg_set_wch_y_wh(volatile vicap_regs_type *reg, td_u32 w, td_u32 h)
{
    u_ch_wch_y_size t;

    t.u32 = reg->ch_wch_y_size.u32;
    t.bits.wch_y_width = w - 1;
    t.bits.wch_y_height = h - 1;
    reg->ch_wch_y_size.u32 = t.u32;
}

td_void vicap_reg_set_wch_y_buf(volatile vicap_regs_type *reg, td_u64 y_addr, td_u32 y_stride)
{
    u_ch_wch_y_stride t;

    t.u32 = reg->ch_wch_y_stride.u32;
    t.bits.wch_y_stride = y_stride;
    reg->ch_wch_y_stride.u32 = t.u32;

    reg->ch_wch_y_faddr_l.u32 = (td_u32)(y_addr);
}

td_void vicap_reg_set_wch_y_cmp(volatile vicap_regs_type *reg, td_u32 cmp_en, td_u32 cmp_mode)
{
    u_ch_wch_y_cfg t;

    t.u32 = reg->ch_wch_y_cfg.u32;
    t.bits.wch_y_cmp_en = cmp_en;
    t.bits.wch_y_cmp_mode = cmp_mode;
    reg->ch_wch_y_cfg.u32 = t.u32;
}

td_void vicap_reg_set_wch_c_ctrl(volatile vicap_regs_type *reg, td_u32 c_en, td_u32 bitw)
{
    u_ch_wch_c_cfg t;
    u_ch_uv2c_cfg ch_uv2c_cfg;

    t.u32 = reg->ch_wch_c_cfg.u32;
    t.bits.wch_c_enable = c_en;
    t.bits.wch_c_bit_width = bitw;
    reg->ch_wch_c_cfg.u32 = t.u32;

    ch_uv2c_cfg.u32 = reg->ch_uv2c_cfg.u32;
    ch_uv2c_cfg.bits.ch_uv2c_enable = c_en;
    reg->ch_uv2c_cfg.u32 = ch_uv2c_cfg.u32;
}

td_void vicap_reg_set_wch_c_wh(volatile vicap_regs_type *reg, td_u32 w, td_u32 h)
{
    u_ch_wch_c_size t;

    t.u32 = reg->ch_wch_c_size.u32;
    t.bits.wch_c_width = w - 1;
    t.bits.wch_c_height = h - 1;
    reg->ch_wch_c_size.u32 = t.u32;
}

td_void vicap_reg_set_wch_c_buf(volatile vicap_regs_type *reg, td_u64 c_addr, td_u32 c_stride)
{
    u_ch_wch_c_stride t;

    t.u32 = reg->ch_wch_c_stride.u32;
    t.bits.wch_c_stride = c_stride;
    reg->ch_wch_c_stride.u32 = t.u32;

    reg->ch_wch_c_faddr_l.u32 = (td_u32)(c_addr);
}

td_void vicap_reg_set_chn_skip_cfg_y(volatile vicap_regs_type *reg, td_u32 skip, td_u32 w, td_u32 h)
{
    u_ch_skip_y_win skip_y_win;

    reg->ch_skip_y_cfg.u32 = skip;

    skip_y_win.bits.ch_skipy_width = w - 1;
    skip_y_win.bits.ch_skipy_height = h - 1;
    reg->ch_skip_y_win.u32 = skip_y_win.u32;
}

td_void vicap_reg_set_chn_skip_cfg_c(volatile vicap_regs_type *reg, td_u32 skip, td_u32 w, td_u32 h)
{
    u_ch_skip_c_win skip_c_win;

    reg->ch_skip_c_cfg.u32 = skip;

    skip_c_win.bits.ch_skipc_width = w - 1;
    skip_c_win.bits.ch_skipc_height = h - 1;
    reg->ch_skip_c_win.u32 = skip_c_win.u32;
}

td_void vicap_reg_set_wch_reg_newer(volatile vicap_regs_type *reg)
{
    u_ch_reg_newer wch_reg_newer;

    wch_reg_newer.u32 = reg->ch_reg_newer.u32;
    wch_reg_newer.bits.ch_reg_newer = 0x1;
    reg->ch_reg_newer.u32 = wch_reg_newer.u32;
}

td_void vicap_reg_set_chn_timestamp_en(volatile vicap_regs_type *reg, td_bool timestamp_en)
{
    u_ch_timestamp_cfg ch_timestamp_cfg;

    ch_timestamp_cfg.u32 = reg->ch_timestamp_cfg.u32;
    ch_timestamp_cfg.bits.ch_timestamp_pos   = 0;     /* 低延时模式延迟多少行记录时间戳 */
    ch_timestamp_cfg.bits.ch_timestamp_level = 0x40;  /* 0x40：时间戳电平持续周期 */
    ch_timestamp_cfg.bits.ch_timestamp_mode  = 1;     /* 0：低延时模式， 1：帧起始模式 */
    ch_timestamp_cfg.bits.ch_timestamp_en    = timestamp_en;
    reg->ch_timestamp_cfg.u32 = ch_timestamp_cfg.u32;
}

td_u64 vicap_reg_get_timestamp(td_void)
{
    td_u32* VICAP_SYS_TICK_VALUE_0 = (td_u32*)0x52063320;
    td_u32* VICAP_SYS_TICK_VALUE_1 = (td_u32*)0x52063324;
    td_u32* VICAP_SYS_TICK_VALUE_2 = (td_u32*)0x52063328;
    td_u32* VICAP_SYS_TICK_VALUE_3 = (td_u32*)0x5206332c;

    td_u32 time_reg0 = ((*VICAP_SYS_TICK_VALUE_0) & 0xFFFF);
    td_u32 time_reg1 = ((*VICAP_SYS_TICK_VALUE_1) & 0xFFFF);
    td_u32 time_reg2 = ((*VICAP_SYS_TICK_VALUE_2) & 0xFFFF);
    td_u32 time_reg3 = ((*VICAP_SYS_TICK_VALUE_3) & 0xFFFF);

    td_u64 timestamp = ((td_u64)time_reg3 << 48) |
                      ((td_u64)time_reg2 << 32) |
                      ((td_u64)time_reg1 << 16) |
                      ((td_u64)time_reg0);

    return timestamp;
}

td_u32 vicap_reg_get_chn_int_status(volatile vicap_regs_type *reg)
{
    return reg->ch_int.u32;
}

td_void vicap_reg_clear_chn_int_status(volatile vicap_regs_type *reg, td_u32 int_status)
{
    reg->ch_int.u32 = int_status;
}

td_void vicap_reg_set_chn_int_mask(volatile vicap_regs_type *reg,  td_u32 int_mask)
{
    reg->ch_int_mask.u32 = int_mask;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
