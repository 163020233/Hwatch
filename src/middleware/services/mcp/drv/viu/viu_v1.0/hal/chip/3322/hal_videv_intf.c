/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hal_ip of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-05-29
 */

#include "drv_viu_struct.h"
#include "drv_vicap.h"

#include "hal_capability_intf.h"
#include "hal_videv_intf.h"

#include "hal_adapt_comm.h"
#include "hal_videv_reg_pt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

td_s32 hal_videv_init(viu_dev_id dev_id, const ext_viu_dev_attr *viu_dev_attr)
{
    viu_unused(dev_id);
    viu_unused(viu_dev_attr);

    // clk open
    // unreset
    return TD_SUCCESS;
}

td_void hal_videv_deinit(viu_dev_id dev_id)
{
    viu_unused(dev_id);

    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(0);

    videv_reg_set_port_en(vicap_reg, TD_FALSE);
    // reset
    // clk close
}

td_void hal_videv_enable(td_s32 dev_id, td_bool dev_en)
{
    viu_unused(dev_id);

    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(0);

    videv_reg_set_port_en(vicap_reg, dev_en);
}

static td_void videv_set_chn_sync(volatile vicap_regs_type *regs,
    ext_viu_input_mode input_mode, const ext_viu_sync_cfg *sync_cfg)
{
    td_u32 sync_mode = 1;
    td_u32 unify_hsync_inv = 0;
    td_u32 unify_hsync_and = 0;
    td_u32 unify_vsync_inv = 0;
    td_u32 unify_vsync_mode = 1;
    td_u32 unify_field_sel = 0;
    td_u32 unify_field_inv = 0;

    if ((input_mode == EXT_VIU_INPUT_MODE_MIPI) || (input_mode == EXT_VIU_INPUT_MODE_LVDS) ||
        (input_mode == EXT_VIU_INPUT_MODE_HISPI)) {
        sync_mode = 0;
    }
    videv_reg_set_unify_timing_cfg(regs, sync_mode);

    if (!(input_mode == EXT_VIU_INPUT_MODE_MIPI || input_mode == EXT_VIU_INPUT_MODE_LVDS ||
        input_mode == EXT_VIU_INPUT_MODE_HISPI)) {
        /* 输入hsyc低时配1，其他配0 */
        if (sync_cfg->hsync_neg == EXT_VIU_HSYNC_NEG_LOW) {
            unify_hsync_inv = 1;
        }

        /* bt656默认配1 (bt656 vsync 为有效方式跟21的配置vsync_field有区别) */
        /* vsync有效方式配1，其他配0 */
        if ((input_mode == EXT_VIU_INPUT_MODE_BT656) ||
            (sync_cfg->vsync == EXT_VIU_VSYNC_PULSE && sync_cfg->vsync_valid == EXT_VIU_VSYNC_VALID_SINGAL)) {
            unify_hsync_and = 1;
        }

        /* 输入vsync为低，配1；其他配0 */
        if ((sync_cfg->vsync_neg == EXT_VIU_VSYNC_NEG_LOW && sync_cfg->vsync_valid == EXT_VIU_VSYNC_NORM_PULSE) ||
            (sync_cfg->vsync_valid_neg == EXT_VIU_VSYNC_VALID_NEG_LOW &&
            sync_cfg->vsync_valid == EXT_VIU_VSYNC_VALID_SINGAL)) {
            unify_vsync_inv = 1;
        }

        /* bt601只支持逐行，vsync信号只取上升沿, 不需field信号。 使用自产生时序，不需恢复vsync和hsync */
        if (input_mode == EXT_VIU_INPUT_MODE_BT601) {
            unify_vsync_mode = 1;
            unify_field_sel = 3; /* 3 means not need field signal */
            videv_reg_set_chn_sync_gen_timing_cfg(regs, 1, 0);
        }
        if ((sync_cfg->vsync == EXT_VIU_VSYNC_FIELD) && (sync_cfg->vsync_neg == EXT_VIU_VSYNC_NEG_LOW)) {
            unify_field_inv = 1;
        }
    }

    videv_reg_set_chn_sync_unify_hsync(regs, unify_hsync_inv, unify_hsync_and, unify_field_inv);
    videv_reg_set_chn_sync_unify_vsync(regs, unify_vsync_inv, unify_vsync_mode, unify_field_sel);
}

static td_bool videv_mipi_intf_mode(ext_viu_intf_mode intf)
{
    td_bool ret = TD_FALSE;

    if ((intf == EXT_VIU_MODE_MIPI) ||
        (intf == EXT_VIU_MODE_MIPI_YUV420_NORMAL) ||
        (intf == EXT_VIU_MODE_MIPI_YUV420_LEGACY) ||
        (intf == EXT_VIU_MODE_MIPI_YUV422) ||
        (intf == EXT_VIU_MODE_LVDS) ||
        (intf == EXT_VIU_MODE_HISPI)) {
        ret = TD_TRUE;
    }

    return ret;
}

static td_void videv_set_chn_combine_mode(volatile vicap_regs_type *regs,
    const ext_viu_dev_attr *viu_dev_attr)
{
    td_u32 unify_data_enable = 0;
    ext_viu_combine_mode combine_mode;

    if (viu_dev_attr->intf_mode == EXT_VIU_MODE_BT656 || viu_dev_attr->intf_mode == EXT_VIU_MODE_BT601) {
        combine_mode = EXT_VIU_COMBINE_SEPARATE;
    } else if (viu_dev_attr->intf_mode == EXT_VIU_MODE_DIGITAL_CAMERA) {
        if (viu_dev_attr->input_data_type == EXT_VIU_DATA_TYPE_YUV) {
            combine_mode = EXT_VIU_COMBINE_SEPARATE;
        } else {
            combine_mode = EXT_VIU_COMBINE_COMPOSITE;
        }
    } else if ((viu_dev_attr->intf_mode == EXT_VIU_MODE_BT1120_STANDARD) ||
               (viu_dev_attr->intf_mode == EXT_VIU_MODE_BT1120_INTERLEAVED) ||
               ((videv_mipi_intf_mode(viu_dev_attr->intf_mode) == TD_TRUE) &&
               (viu_dev_attr->input_data_type == EXT_VIU_DATA_TYPE_YUV))) {
        combine_mode = EXT_VIU_COMBINE_SEPARATE;
    } else {
        combine_mode = EXT_VIU_COMBINE_COMPOSITE;
    }

    if (combine_mode == EXT_VIU_COMBINE_SEPARATE) {
        unify_data_enable = 1;
    }

    videv_reg_set_chn_combine_mode(regs, unify_data_enable);
}

static td_u32 videv_calc_mask(td_u32 mask, td_bool reverse)
{
    td_u32 in_mask = mask;
    td_u32 out_mask = 0;
    td_u32 bit = 0;

    if (reverse == TD_FALSE) {
        out_mask = in_mask;
    } else {
        do {
            if ((in_mask & 0x80000000U) == 0x80000000U) { /* 0x80000000 [31]bit */
                out_mask |= ((td_u32)0x1U << bit);
            } else {
                out_mask <<= 1;
            }

            in_mask <<= 1;
            bit++;
        } while (in_mask != 0);
    }

    return out_mask;
}

static td_u32 videv_calc_offset(ext_viu_intf_mode intf_mode, td_u32 mask_id, td_u32 mask, td_bool reverse)
{
    viu_unused(mask_id);

    td_u32 j = 0;
    td_u32 temp;
    td_u32 offset;

    temp = mask;
    while (temp != 0) {
        temp = (temp >> 1);
        j++;
    }

    if (reverse == TD_TRUE) {
        offset = 20 - j; /* 20 valid bit */
    } else {
        offset = 32 - j; /* 32bit */
    }

    /*
     * 10bit: {0xffc00000, 0x003ff000} 与 {0x003ff000, 0xffc00000} -> offset{0, 10} 与 {10, 0}
     * 8bit:  {0xff000000, 0x00ff0000} 与 {0x00ff0000, 0xff000000} -> offset{0, 8} 与 {8, 0}
     * mipi_yuv： 10bit, y[19:10]  c[9:0]; 8bit, y[19:12]  c[9:2]; need offset 10.
     */
    if ((intf_mode == EXT_VIU_MODE_MIPI_YUV420_NORMAL) ||
        (intf_mode == EXT_VIU_MODE_MIPI_YUV420_LEGACY) ||
        (intf_mode == EXT_VIU_MODE_MIPI_YUV422)) {
        if (offset == 8) {  /* 8 is 32 - 24. when mipi_yuv 8bit */
            offset = 10;    /* mipi_yuv need offset 10.  [19:12] [9:2] */
        }
    }

    return offset;
}

td_void hal_videv_process_loop(viu_dev_id dev_id, const ext_viu_dev_attr *viu_dev_attr)
{
    viu_unused(dev_id);
    viu_unused(viu_dev_attr);

    return;
}
td_void hal_videv_process(viu_dev_id dev_id, const ext_viu_dev_attr *viu_dev_attr)
{
    viu_unused(dev_id);

    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(0);
    td_u32 comp_num = 1;
    td_u32 mask, data_offset, i;
    td_u32 pt_mode = VI_SYNC_MODE_EXTERNAL;
    td_bool internal_sync = (pt_mode == VI_SYNC_MODE_INTERNAL) ? TD_TRUE : TD_FALSE;
    td_bool bt656_en = (pt_mode == VI_SYNC_MODE_INTERNAL) ? TD_TRUE : TD_FALSE;
    td_u32 yc_seq = 0;
    td_u32 uv_seq = 0;

    if (viu_dev_attr->intf_mode == EXT_VIU_MODE_DIGITAL_CAMERA) {
        comp_num = 0x1;
    } else if (viu_dev_attr->input_data_type == EXT_VIU_DATA_TYPE_YUV) {
        comp_num = 0x2;
    }
    videv_reg_set_port_comp_mode(vicap_reg, comp_num - 1);

    /* 根据是否反向计算新的mask */
    mask = videv_calc_mask(viu_dev_attr->component_mask[0], viu_dev_attr->data_reverse);
    /* 根据新mask计算offset */
    data_offset = videv_calc_offset(viu_dev_attr->intf_mode, 0x1, mask, viu_dev_attr->data_reverse);
    /* 配置 offset */
    for (i = 0; i < comp_num; i++) {
        videv_reg_set_data_offset(vicap_reg, i, data_offset, viu_dev_attr->data_reverse);
        videv_reg_set_data_mask(vicap_reg, i, viu_dev_attr->component_mask[i]);
    }

    /* 配置内同步模式 */
    videv_reg_set_port_input_mode(vicap_reg, internal_sync);
    videv_reg_set_bt656_en(vicap_reg, bt656_en);
    videv_reg_set_port_bt656_mode(vicap_reg, 0x3);

    /* 归一化时序配置 */
    videv_set_chn_sync(vicap_reg, EXT_VIU_INPUT_MODE_DIGITAL_CAMERA, &viu_dev_attr->syn_cfg);

    /* 归一化数据配置 */
    videv_set_chn_combine_mode(vicap_reg, viu_dev_attr);

    /* 设置vi dev的输入数据的顺序,其他时序默认为UYVY */
    if ((viu_dev_attr->data_seq == EXT_VIU_DATA_SEQ_YUYV) || (viu_dev_attr->data_seq == EXT_VIU_DATA_SEQ_YVYU)) {
        yc_seq = 1;
    }
    if ((viu_dev_attr->data_seq == EXT_VIU_DATA_SEQ_VYUY) || (viu_dev_attr->data_seq == EXT_VIU_DATA_SEQ_YVYU) ||
        (viu_dev_attr->data_seq == EXT_VIU_DATA_SEQ_VUVU)) {
        uv_seq = 1;
    }
    videv_reg_set_chn_yuv_input_seq(vicap_reg, yc_seq, uv_seq);

    if (viu_dev_attr->input_data_type == EXT_VIU_DATA_TYPE_RGB) {
        videv_reg_set_port_yuv444_en(vicap_reg, TD_FALSE); /* close YUV444 */
    } else if (viu_dev_attr->input_data_type == EXT_VIU_DATA_TYPE_YUV) {
        videv_reg_set_port_yuv444_en(vicap_reg, TD_TRUE);  /* open YUV444 */
    }
}

static td_u64 videv_get_ppc_clock_by_clk_sel(td_u32 ppc_clk_sel)
{
    td_u64 viu_clk;

    switch (ppc_clk_sel) {
        case 0x0:
            viu_clk = 100; /* 100 M */
            break;
        case 0x1:
            viu_clk = 148; /* 148 M */
            break;
        case 0x2:
            viu_clk = 148; /* 148 M */
            break;
        case 0x3:
            viu_clk = 198; /* 198 M */
            break;
        case 0x4:
            viu_clk = 264; /* 264 M */
            break;
        case 0x5:
            viu_clk = 297; /* 297 M */
            break;
        case 0x6:
            viu_clk = 380; /* 380 M */
            break;
        case 0x7:
            viu_clk = 478; /* 478 M */
            break;
        case 0x8:
            viu_clk = 200; /* 200 M */
            break;
        default:
            viu_clk = 475; /* 475 M */
            break;
    }

    return viu_clk * 1000000; // 1000000 -> M
}

#define MIN_PT_H_BANK 0x60U
#define MIN_PT_V_BANK 0x40U
td_void hal_videv_set_timing(viu_dev_id dev_id, ext_viu_dev_timing_attr *timing_attr, ext_isp_size *size)
{
    viu_unused(dev_id);
    td_u64 max_frm_rate;
    td_u64 viu_clk;
    td_u32 hba, vba;
    td_u32 frm_rate;
    hal_capability_vicap *cap_vicap = hal_cap_get_vicap();
    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(0);

    if (timing_attr->enable == TD_FALSE) {
        videv_reg_set_dev_auto_timing_en(vicap_reg, TD_FALSE);
        videv_reg_set_dev_auto_data_en(vicap_reg, TD_FALSE);
    } else {
        viu_clk = videv_get_ppc_clock_by_clk_sel(cap_vicap->clk_sel);
        max_frm_rate = viu_clk / ((size->width + MIN_PT_H_BANK * 2) * (size->height + MIN_PT_V_BANK * 2)); // 2 bank
        frm_rate = timing_attr->frm_rate;

        vba = MIN_PT_V_BANK;
        if (frm_rate > max_frm_rate) {
            hba = MIN_PT_H_BANK;
            timing_attr->frm_rate = max_frm_rate;
            viu_hal_err("dev%d max frame rate: %u, but set frame rate is %u when clock is %lluHZ.\n",
                dev_id, max_frm_rate, frm_rate, viu_clk);
        } else {
            hba = ((viu_clk / (frm_rate * (size->height + 2 * vba))) - size->width) / 2; // 2 vba
        }

        videv_reg_set_dev_timing_hfb(vicap_reg, hba);
        videv_reg_set_dev_timing_hact(vicap_reg, size->width);
        videv_reg_set_dev_timing_hbb(vicap_reg, hba);
        videv_reg_set_dev_timing_vfb(vicap_reg, vba);
        videv_reg_set_dev_timing_vact(vicap_reg, size->height);
        videv_reg_set_dev_timing_vbb(vicap_reg, vba);
        videv_reg_set_dev_auto_timing_en(vicap_reg, TD_TRUE);
        videv_reg_set_dev_auto_data_en(vicap_reg, TD_TRUE);
    }
}

td_void hal_videv_get_status(td_s32 viu_dev_id, hal_videv_status *status)
{
    viu_unused(viu_dev_id);
    viu_unused(status);

    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */