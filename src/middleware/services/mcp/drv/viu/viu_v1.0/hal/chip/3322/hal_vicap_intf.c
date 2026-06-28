/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: hal_ip of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-05-29
 */
#include "hal_vicap_intf.h"

#include "drv_viu_struct.h"
#include "drv_vicap.h"

#include "hal_chip_define.h"
#include "hal_adapt_comm.h"

#include "hal_vicap_reg_crg.h"
#include "hal_vicap_reg_chn.h"
#include "hal_vicap_reg_global.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

typedef struct {
    td_u32 c_width;
    td_u32 c_height;
    td_bool y_en;
    td_bool c_en;
    td_u32 bit_width;
    td_bool cmp_en;
    td_u32 cmp_mode;
} hal_vicap_pic_cfg;

static void vi_soft_rst(bool rst)
{
    td_u32 *crg_rst_reg = (td_u32 *)0x52063308;
    if (rst == 1) {
        *crg_rst_reg = 0x5;
        osal_msleep(10);  // time 10

        if (*crg_rst_reg != 0xD) {
            viu_hal_err(" crg_rst_reg fail = %x\n", crg_rst_reg);
        }
    } else {
        *crg_rst_reg = 0x1;
        osal_msleep(10);  // time 10
        if (*crg_rst_reg != 0x1) {
            viu_hal_err("unreeset crg_rst_reg = %x\n", crg_rst_reg);
        }
    }
}

static td_void vicap_init_crg(viu_cap_id cap_id)
{
    viu_unused(cap_id);

    //  sdiom 50M
    //  clk_vicap_padin  输入时钟 pclk
    //  MCLK 晶振默认上电24M
    // 开时钟-》解复位-》软复位
    td_u32 *crg_mux_reg = (td_u32 *)0x5206330C;
    td_u32 *crg_clk_reg = (td_u32 *)0x52063300;
    td_u32 *crg_rst_reg = (td_u32 *)0x52063308;

    *crg_mux_reg = 0x1;
    mcp_osal_msleep(10); // time 10

    *crg_clk_reg = 0x7;
    mcp_osal_msleep(10); // time 10

    *crg_rst_reg = 0x1;
    mcp_osal_msleep(10); // time 10

    vi_soft_rst(true);
    vi_soft_rst(false);
}

static td_void vicap_deinit_crg(viu_cap_id cap_id)
{
    viu_unused(cap_id);
    td_u32 cnt = 0;
    td_u32 *crg_clk_reg = (td_u32 *)0x52063300;  // VICAP_CTRL_CFG0
    td_u32 *crg_rst_reg = (td_u32 *)0x52063308; // VICAP_CTRL_CFG2
    td_u32 *crg_mux_reg = (td_u32 *)0x5206330C; // VICAP_CTRL_CFG3

    *crg_rst_reg = 0x5;
    osal_msleep(10);  // time 10

    while (*crg_rst_reg != 0xD) {
        osal_msleep(10);  // time 10
        cnt++;
        if (cnt == 10) {  // 10 : wait 100ms
            viu_hal_err("hal vicap%d crg_rst_reg fail.\n", cap_id);
            return;
        }
    }
    *crg_rst_reg = 0x2;
    mcp_osal_msleep(10); // time 10
    *crg_clk_reg = 0x0;
    mcp_osal_msleep(10); // time 10
    *crg_mux_reg = 0x0;
    mcp_osal_msleep(10); // time 10

    return;
}

td_s32 hal_vicap_init(viu_cap_id cap_id)
{
    volatile vicap_regs_type *vicap_reg = TD_NULL;

    if (hal_comm_init_vicap_reg(cap_id) != TD_SUCCESS) {
        viu_hal_err("hal cap%d reg init fail.\n", cap_id);
        return TD_FAILURE;
    }

    vicap_init_crg(cap_id);

    vicap_reg = hal_comm_get_vicap_reg(cap_id);
    vicap_reg_set_outstanding(vicap_reg, VICAP_RD_OUTSTANDING, VICAP_WR_OUTSTANDING); /* outstanding: rd: 8, wr: 8 */
    vicap_reg_set_int_mask(vicap_reg, VIU_GLOBAL_INT_MASK);

    return TD_SUCCESS;
}

td_void hal_vicap_deinit(viu_cap_id cap_id)
{
    volatile vicap_regs_type *vicap_reg = TD_NULL;

    vicap_reg = hal_comm_get_vicap_reg(cap_id);

    vicap_reg_set_int_mask(vicap_reg, 0x0);

    vicap_deinit_crg(cap_id);

    hal_comm_deinit_vicap_reg(cap_id);
}

td_void hal_vicap_work_mode(viu_cap_id cap_id, td_bool online)
{
    viu_unused(cap_id);
    viu_unused(online);

    return;
}

td_void hal_vicap_create(viu_cap_id cap_id, ext_isp_rect *crop_rect)
{
    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(cap_id);

    vicap_reg_ppc_chn_clk_en(cap_id, TD_TRUE);
    vicap_reg_isp_clk_en(cap_id, TD_TRUE);
    vicap_reg_isp_core_reset(cap_id, TD_FALSE);
    vicap_reg_isp_cfg_reset(cap_id, TD_FALSE);

    vicap_reg_set_afifo_crop_xy(vicap_reg, crop_rect->x, crop_rect->y);
    vicap_reg_set_afifo_crop_wh(vicap_reg, crop_rect->width, crop_rect->height);

    vicap_reg_set_line_afifo_en(vicap_reg, TD_FALSE);
}

td_void hal_vicap_destroy(viu_cap_id pipe_id)
{
    viu_unused(pipe_id);

    return;
}

td_void hal_vicap_start(viu_cap_id cap_id, ext_isp_frame_interrupt_type int_type, td_bool online)
{
    viu_unused(int_type);
    viu_unused(online);

    volatile vicap_regs_type *vicap_reg;
    td_u32 is_yuv = TD_TRUE;

    vicap_reg = hal_comm_get_vicap_reg(cap_id);

    if (cap_id == VIU_VICAP_ID_0) {
        vicap_reg_set_isp0_mode(vicap_reg, TD_TRUE, is_yuv);
    } else if (cap_id == VIU_VICAP_ID_1) {
        vicap_reg_set_isp1_mode(vicap_reg, TD_TRUE, is_yuv);
    }

    vicap_reg_set_chn_timestamp_en(vicap_reg, TD_TRUE);

    vicap_reg_set_wch_finish_mode(vicap_reg, 0x1);
    vicap_reg_clear_chn_int_status(vicap_reg, VI_CHN_CLEAR_INT_STATUS);
    vicap_reg_set_chn_int_mask(vicap_reg, VI_CHN_INT_MASK);

    vicap_reg_set_wch_en(vicap_reg, TD_TRUE);
    vicap_reg_set_wch_reg_newer(vicap_reg);
}

td_void hal_vicap_stop(viu_cap_id cap_id)
{
    volatile vicap_regs_type *vicap_reg;

    vicap_reg = hal_comm_get_vicap_reg(cap_id);

    vicap_reg_set_wch_en(vicap_reg, TD_FALSE);
    vicap_reg_set_chn_int_mask(vicap_reg, 0x0);
}

td_void hal_vicap_disable(viu_cap_id cap_id)
{
    viu_unused(cap_id);

    return;
}

td_void hal_vicap_get_online_status(viu_cap_id cap_id, td_u32 *value)
{
    viu_unused(cap_id);
    viu_unused(value);

    return;
}

td_void hal_vicap_regnew(viu_cap_id cap_id)
{
    viu_unused(cap_id);

    return;
}

td_u32 hal_vicap_get_int_status(td_void)
{
    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(0);

    return vicap_reg_get_cap_int_status(vicap_reg);
}

td_bool hal_vicap_get_int_report(viu_cap_id cap_id, td_u32 cap_int_status)
{
    td_bool ret = TD_FALSE;

    switch (cap_id) {
        case 0: /* pipe0 */
            if ((cap_int_status & VIU_GLOBAL_INT_WCH0) != 0) {
                ret = TD_TRUE;
            }
            break;

        case 1: /* pipe1 */
            if ((cap_int_status & VIU_GLOBAL_INT_WCH1) != 0) {
                ret = TD_TRUE;
            }
            break;

        case 2: /* pipe2 */
            if ((cap_int_status & VIU_GLOBAL_INT_WCH2) != 0) {
                ret = TD_TRUE;
            }
            break;

        case 3: /* pipe3 */
            if ((cap_int_status & VIU_GLOBAL_INT_WCH3) != 0) {
                ret = TD_TRUE;
            }
            break;

        default:
            ret = TD_FALSE;
            break;
    }

    return ret;
}

td_u32 hal_vicap_get_ch_int_status(viu_cap_id cap_id)
{
    viu_unused(cap_id);

    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(0);

    return vicap_reg_get_chn_int_status(vicap_reg);
}

td_void hal_vicap_clear_ch_int(viu_cap_id cap_id, td_u32 wch_int)
{
    viu_unused(cap_id);

    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(0);

    vicap_reg_clear_chn_int_status(vicap_reg, wch_int);
}

td_void hal_vicap_set_isp_sel(viu_cap_id cap_id, td_u32 isp_sel)
{
    volatile vicap_regs_type *vicap_reg = hal_comm_get_vicap_reg(0);

    vicap_reg_set_isp_sel(vicap_reg, cap_id, isp_sel);
}

td_u64 hal_vicap_reg_get_timestamp(td_void)
{
    return vicap_reg_get_timestamp();
}

#define VI_LPDDR4_ALIGN 32
static td_bool vicap_chech_frame_align32_en(const ext_isp_video_frame *video_frame)
{
    td_phys_addr_t y_addr = video_frame->phy_addr[0];
    td_phys_addr_t c_addr = video_frame->phy_addr[1];
    td_u32 y_stride = video_frame->stride[0];
    td_u32 c_stride = video_frame->stride[1];
    ext_pixel_format pix_fmt = video_frame->pixel_format;

    if ((y_addr % VI_LPDDR4_ALIGN != 0) || (y_stride % VI_LPDDR4_ALIGN != 0)) {
        return TD_FALSE;
    }

    if ((pix_fmt == EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_422 || pix_fmt == EXT_ISP_PIXEL_FORMAT_YVU_SEMIPLANAR_420 ||
        pix_fmt == EXT_ISP_PIXEL_FORMAT_YUV_SEMIPLANAR_422 || pix_fmt == EXT_ISP_PIXEL_FORMAT_YUV_SEMIPLANAR_420) &&
        ((c_addr % VI_LPDDR4_ALIGN != 0) || (c_stride % VI_LPDDR4_ALIGN != 0))) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

static td_u32 vicap_convert_pixel_val(ext_pixel_format pix_fmt)
{
    td_u32 pixel = 0;

    switch (pix_fmt) {
        case EXT_PIXEL_FORMAT_RGB_BAYER_8BPP:
        case EXT_PIXEL_FORMAT_RGB_BAYER_10BPP:
        case EXT_PIXEL_FORMAT_RGB_BAYER_12BPP:
        case EXT_PIXEL_FORMAT_RGB_BAYER_14BPP:
        case EXT_PIXEL_FORMAT_RGB_BAYER_16BPP:
            pixel = 2; /* 2 -- to set cur_format, 0_SP420, 1_SP422, 2_RAW, 5_Planar444 */
            break;

        case EXT_PIXEL_FORMAT_YUV_400:
        case EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        case EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_420:
            pixel = 0;
            break;

        case EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422:
        case EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_422:

            pixel = 1;
            break;

        case EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_444:
            pixel = 5; /* 2 -- to set cur_format, 0_SP420, 1_SP422, 2_RAW, 5_Planar444 */
            break;

        case EXT_PIXEL_FORMAT_RGB_888:
        case EXT_PIXEL_FORMAT_RGB_10BPP:
        case EXT_PIXEL_FORMAT_RGB_12BPP:
            pixel = 0x6; /* pkg rgb */
            break;
        default:
            viu_debug_trace("not support this pixel format :%d!\n", pix_fmt);
            break;
    }

    return pixel;
}

static td_u32 vicap_convert_bitw_val(ext_pixel_format pix_fmt, ext_dynamic_range dynamic_range)
{
    td_u32 bit_width = 0;

    switch (pix_fmt) {
        case EXT_PIXEL_FORMAT_RGB_BAYER_8BPP:
            bit_width = 8; /* 8--8BPP, 10--10BPP, 12--12BPP, 14--14BPP, 16--16BPP */
            break;

        case EXT_PIXEL_FORMAT_RGB_BAYER_10BPP:
            bit_width = 10; /* 8--8BPP, 10--10BPP, 12--12BPP, 14--14BPP, 16--16BPP */
            break;

        case EXT_PIXEL_FORMAT_RGB_BAYER_12BPP:
            bit_width = 12; /* 8--8BPP, 10--10BPP, 12--12BPP, 14--14BPP, 16--16BPP */
            break;

        case EXT_PIXEL_FORMAT_RGB_BAYER_14BPP:
            bit_width = 14; /* 8--8BPP, 10--10BPP, 12--12BPP, 14--14BPP, 16--16BPP */
            break;

        case EXT_PIXEL_FORMAT_RGB_BAYER_16BPP:
            bit_width = 16; /* 8--8BPP, 10--10BPP, 12--12BPP, 14--14BPP, 16--16BPP */
            break;

        case EXT_PIXEL_FORMAT_YUV_400:
        case EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        case EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422:
        case EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_420:
        case EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_422:
            bit_width = (td_u32)((dynamic_range == EXT_DYNAMIC_RANGE_SDR8) ? 8 : 10); /* YUV 8--SDR8, 10--SDR10 */
            break;

        default:
            viu_debug_trace("not support this pixel format :%d !\n", pix_fmt);
            break;
    }

    return bit_width;
}

static td_void vicap_process_pic_cfg(const ext_isp_video_frame *frame, hal_vicap_pic_cfg *pic_cfg)
{
    if (vicap_convert_pixel_val(frame->pixel_format) == 2) { /* 2 -- RAW */
        pic_cfg->y_en = TD_TRUE;
        pic_cfg->c_en = TD_FALSE;
    } else {
        pic_cfg->y_en = TD_TRUE;
        pic_cfg->c_en = TD_TRUE;
    }

    if (frame->pixel_format == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422 ||
        frame->pixel_format == EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_422) {
        pic_cfg->c_width = frame->width;
        pic_cfg->c_height = frame->height;
    } else if (frame->pixel_format == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 ||
        frame->pixel_format == EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_420) {
        pic_cfg->c_width = frame->width;
        pic_cfg->c_height = frame->height >> 1;
    } else if (frame->pixel_format == EXT_PIXEL_FORMAT_YUV_400) {
        pic_cfg->c_en = TD_FALSE;
        pic_cfg->c_width = 0;
        pic_cfg->c_height = 0;
    } else {
        pic_cfg->c_width = 0;
        pic_cfg->c_height = 0;
    }

    pic_cfg->cmp_en = (frame->compress_mode != EXT_COMPRESS_MODE_NONE) ? TD_TRUE : TD_FALSE;
    pic_cfg->bit_width = vicap_convert_bitw_val(frame->pixel_format, frame->dynamic_range);

    pic_cfg->cmp_mode = (td_u32)((frame->compress_mode == EXT_COMPRESS_MODE_LINE) ? 1 : 0);
}

static td_void vicap_process_pic(viu_cap_id cap_id, volatile vicap_regs_type *vicap_reg, td_u32 skip_en,
    ext_isp_video_frame *frame)
{
    viu_unused(cap_id);

    td_u32 y_skip, c_skip = 0;
    hal_vicap_pic_cfg pic_cfg = {0};
    td_bool align_32b_en = vicap_chech_frame_align32_en(frame);

    vicap_process_pic_cfg(frame, &pic_cfg);

    vicap_reg_set_wch_y_ctrl(vicap_reg, pic_cfg.y_en, pic_cfg.bit_width, align_32b_en);
    vicap_reg_set_wch_y_wh(vicap_reg, frame->width, frame->height);
    vicap_reg_set_wch_y_buf(vicap_reg, frame->phy_addr[0], frame->stride[0]);

    vicap_reg_set_wch_c_ctrl(vicap_reg, pic_cfg.c_en, pic_cfg.bit_width);
    vicap_reg_set_wch_c_wh(vicap_reg, pic_cfg.c_width, pic_cfg.c_height);
    vicap_reg_set_wch_c_buf(vicap_reg, frame->phy_addr[1], frame->stride[1]);

    y_skip = 0xFFFFFFFFU;
    if (frame->pixel_format == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420 ||
        frame->pixel_format == EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_420) {
        if (skip_en == TD_TRUE) {
            c_skip = 0xAA00AA00U;
        } else {
            c_skip = 0xAAAAAAAAU;
        }
    } else if (frame->pixel_format == EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422 ||
        frame->pixel_format == EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_422) {
        c_skip = 0xAAAAAAAAU;
    } else if (frame->pixel_format == EXT_PIXEL_FORMAT_YUV_400) {
        c_skip = 0x00000000;
    }

    vicap_reg_set_chn_skip_cfg_y(vicap_reg, y_skip, frame->width, frame->height);
    vicap_reg_set_chn_skip_cfg_c(vicap_reg, c_skip, pic_cfg.c_width, pic_cfg.c_height);
}

td_void hal_vicap_process(viu_cap_id cap_id, td_u32 skip_en, td_u32 buf_ok, ext_isp_crop_info *crop_info,
    ext_isp_video_frame_info *frame)
{
    viu_unused(buf_ok);

    volatile vicap_regs_type *vicap_reg;

    vicap_reg = hal_comm_get_vicap_reg(cap_id);

    /* 更新当前crop */
    vicap_reg_set_ch_crop_en(vicap_reg, crop_info->enable);
    vicap_reg_set_ch_crop_xy(vicap_reg, crop_info->rect.x, crop_info->rect.y);
    vicap_reg_set_ch_crop_wh(vicap_reg, crop_info->rect.width, crop_info->rect.height);

    /* 启动采集下一帧 */
    vicap_process_pic(cap_id, vicap_reg, skip_en, &frame->v_frame);

    vicap_reg_set_wch_reg_newer(vicap_reg);

    return;
}

td_void hal_vicap_wdr_process(viu_cap_id cap_id, td_u32 skip_en, td_u32 buf_ok, ext_isp_video_frame_info *frame,
    td_u32 wdr_idx)
{
    viu_unused(cap_id);
    viu_unused(skip_en);
    viu_unused(buf_ok);
    viu_unused(frame);
    viu_unused(wdr_idx);

    return;
}

td_void hal_vicap_wdr_init(viu_cap_id cap_id)
{
    viu_unused(cap_id);

    return;
}

td_void hal_vicap_set_wrap(viu_cap_id cap_id, td_bool enable, td_u32 cache_line)
{
    viu_unused(cap_id);
    viu_unused(enable);
    viu_unused(cache_line);

    return;
}

td_void hal_vicap_get_status(viu_cap_id cap_id, hal_vicap_status *status)
{
    viu_unused(cap_id);
    viu_unused(status);

    return;
}
td_void hal_vicap_set_isp_mode(viu_cap_id cap_id, td_bool isp_bypass, td_bool isp_in_yuv)
{
    viu_unused(cap_id);
    viu_unused(isp_bypass);
    viu_unused(isp_in_yuv);

    return;
}

td_void hal_vicap_set_isp_id_sel(viu_cap_id cap_id, td_u32 isp_id_sel)
{
    viu_unused(cap_id);
    viu_unused(isp_id_sel);

    return;
}
td_void hal_vicap_set_wch_cmp_mode(viu_cap_id cap_id, td_u32 bayer_mode)
{
    viu_unused(cap_id);
    viu_unused(bayer_mode);

    return;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
