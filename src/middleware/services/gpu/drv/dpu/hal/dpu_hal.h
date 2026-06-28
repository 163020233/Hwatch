/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : dpu reg common file
 */

#ifndef DRV_GRAPHICS_HAL_DPU_HAL_H
#define DRV_GRAPHICS_HAL_DPU_HAL_H

#include "td_base.h"
#include "dpu_hal_layer.h"
#include "dpu_hal_disp.h"
#include "gpu_crg_intf.h"
#include "common_def.h"
#include "dpu_hal_acc.h"
#include "drv_dpu_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define DPU_CLOCK_START_POS  2

typedef enum {
    DPU_DISP_INT_TYPE_NONE       = 0x0,
    DPU_DISP_INT_TYPE_SETTING    = 0x1,
    DPU_DISP_INT_TYPE_FINISH     = 0x2,
    DPU_DISP_INT_TYPE_RESERVE    = 0x4,
    DPU_DISP_INT_TYPE_LOWBAND    = 0x8,
    DPU_DISP_INT_TYPE_BUS_ERR    = 0x1000,
    DPU_DISP_INT_TYPE_TE_INT     = 0x100000,
    DPU_DISP_INT_TYPE_FRM_DONE   = 0x200000,
    DPU_DISP_INT_TYPE_WBC_DONE   = 0x800000,
    DPU_DISP_INT_TYPE_DCMP_CFG_ERR = 0x2000000,
    DPU_DISP_INT_TYPE_DCMP_BS_ERR  = 0x4000000,

    DPU_DISP_INT_TYPE_MAX        = 0x80000000
} dpu_disp_int_type;

typedef enum {
    DPU_DISP_VSYNC_TE_VODEO_MODE    = 0x0, /* The VFB-to-VBB switchover is controlled by the timing configuration. */
    DPU_DISP_VSYNC_TE_CMD0_MODE     = 0x1, /* The VFB-to-VBB switchover is controlled by the TE signal. */
    DPU_DISP_VSYNC_TE_CMD1_MODE     = 0x2, /* The VFB-to-VBB switchover is controlled by the FRM_RDY signal. */
    DPU_DISP_VSYNC_TE_CMD2_MODE     = 0x3, /* The VFB-to-VBB switchover is controlled by TE signal and MIPI_RDY. */
} dpu_disp_vsync_te_mode;

typedef enum {
    DPU_DISP_FORMAT_LINK_ARGB_RGB   = 0x0,
    DPU_DISP_FORMAT_LINK_YUV420     = 0x1,
    DPU_DISP_FORMAT_LINK_HDR        = 0x2,
} dpu_disp_format_link;

typedef enum {
    DPU_DISP_CBM_MIX_PRIORITY_NONE = 0x0,
    DPU_DISP_CBM_MIX_PRIORITY_0    = 0x1,
    DPU_DISP_CBM_MIX_PRIORITY_1    = 0x2,
    DPU_DISP_CBM_MIX_PRIORITY_MAX,
} dpu_disp_cbm_mix_priority;

typedef enum {
    DPU_DISP_CBM_MIX_LAYER_NONE    = 0x0,
    DPU_DISP_CBM_MIX_LAYER_G0      = 0x1,
    DPU_DISP_CBM_MIX_LAYER_G1      = 0x2,
    DPU_DISP_CBM_MIX_LAYER_MAX
} dpu_disp_cbm_mix_layer;

typedef struct {
    td_u32 vfb;
    td_u32 vbb;
    td_u32 vact;
    td_u32 hfb;
    td_u32 hbb;
    td_u32 hact;
    td_u32 vpw;
    td_u32 hpw;

    td_u32 idv;
    td_u32 ihs;
    td_u32 ivs;

    td_u32 iop;

    td_u32 pixel_clk;
    td_u32 refresh_rate;
    td_bool is_cmd_mode;
} dpu_disp_timing;

typedef enum {
#ifdef GPU_1A22
    DPU_HAL_FMT_ARGB8888    = 0x00,
    DPU_HAL_FMT_KRGB8888    = 0x01,
    DPU_HAL_FMT_ARGB4444    = 0x02,
    DPU_HAL_FMT_ARGB1555    = 0x03,
    DPU_HAL_FMT_ARGB8565    = 0x04,
    DPU_HAL_FMT_RGB888      = 0x05,
    DPU_HAL_FMT_RGB444      = 0x06,
    DPU_HAL_FMT_RGB555      = 0x07,
    DPU_HAL_FMT_RGB565      = 0x08,
    DPU_HAL_FMT_A1          = 0x09,
    DPU_HAL_FMT_A8          = 0x0A,
    DPU_HAL_FMT_A1B         = 0x0D,
    DPU_HAL_FMT_CLUT1       = 0x10,
    DPU_HAL_FMT_CLUT2       = 0x11,
    DPU_HAL_FMT_CLUT4       = 0x12,
    DPU_HAL_FMT_CLUT8       = 0x13,
    DPU_HAL_FMT_ACLUT44     = 0x14,
    DPU_HAL_FMT_ACLUT88     = 0x15,
    DPU_HAL_FMT_CLUT1B      = 0x16,
    DPU_HAL_FMT_CLUT2B      = 0x17,
    DPU_HAL_FMT_CLUT4B      = 0x18,
    DPU_HAL_FMT_A2          = 0x19,
    DPU_HAL_FMT_A4          = 0x1A,
    DPU_HAL_FMT_A2B         = 0x1B,
    DPU_HAL_FMT_A4B         = 0x1C,
    DPU_HAL_FMT_YUV400SP    = 0x30,
    DPU_HAL_FMT_YUV422SP    = 0x31,
    DPU_HAL_FMT_YUV420SP    = 0x33,
    DPU_HAL_FMT_YUV444SP    = 0x34,
    DPU_HAL_FMT_R8          = 0x35,
    DPU_HAL_FMT_G8          = 0x36,
    DPU_HAL_FMT_B8          = 0x37,
    DPU_HAL_FMT_RGB332      = 0x38,
    DPU_HAL_FMT_ARGB2101010 = 0x39,
#else
    DPU_HAL_FMT_CLUT1       = 0x00,
    DPU_HAL_FMT_CLUT2       = 0x10,
    DPU_HAL_FMT_CLUT4       = 0x20,
    DPU_HAL_FMT_CLUT8       = 0x30,
    DPU_HAL_FMT_YUV420SP    = 0x31,
    DPU_HAL_FMT_RGB332      = 0x34,
    DPU_HAL_FMT_R8          = 0x35,
    DPU_HAL_FMT_G8          = 0x36,
    DPU_HAL_FMT_B8          = 0x37,
    DPU_HAL_FMT_ACLUT44     = 0x38,
    DPU_HAL_FMT_RGB444      = 0x40,
    DPU_HAL_FMT_RGB555      = 0x41,
    DPU_HAL_FMT_RGB565      = 0x42,
    DPU_HAL_FMT_PKGUYVY     = 0x43,
    DPU_HAL_FMT_PKGYUYV     = 0x44,
    DPU_HAL_FMT_PKGYVYU     = 0x45,
    DPU_HAL_FMT_ACLUT88     = 0x46,
    DPU_HAL_FMT_ARGB4444    = 0x48,
    DPU_HAL_FMT_ARGB1555    = 0x49,
    DPU_HAL_FMT_RGB888      = 0x50,
    DPU_HAL_FMT_YCBCR888    = 0x51,
    DPU_HAL_FMT_ARGB8565    = 0x5a,
    DPU_HAL_FMT_KRGB888     = 0x60,
    DPU_HAL_FMT_ARGB8888    = 0x68,
    DPU_HAL_FMT_AYCBCR8888  = 0x69,
    DPU_HAL_FMT_ARGB2101010 = 0x79,
    DPU_HAL_FMT_RGBA4444    = 0xc8,
    DPU_HAL_FMT_RGBA5551    = 0xc9,
    DPU_HAL_FMT_RGBA5658    = 0xda,
    DPU_HAL_FMT_RGBA8888    = 0xe8,
    DPU_HAL_FMT_YCBCRA8888  = 0xe9,
    DPU_HAL_FMT_ABGR8888    = 0xef,
#endif
    DPU_HAL_FMT_DEFAULT = DPU_HAL_FMT_RGB888
} dpu_hal_fmt;

typedef struct {
    td_u32 y_addr;
    td_u32 out_height;
    td_u32 uv_stride;
    td_u32 uv_offset;
} dpu_hal_yuv_info;

typedef struct {
    td_u32 hdr0;
    td_u32 pld_data00;
    td_u32 pld_data01;

    td_u32 hdr1;
    td_u32 pld_data10;
    td_u32 pld_data11;
} dpu_hal_partial_cmd_info;

typedef enum {
    DPU_HAL_TRANSFORM_MODE_DIRECT = 0,
    DPU_HAL_TRANSFORM_MODE_BILINEAR
} dpu_hal_transform_mode;

typedef struct {
    td_u32 mapping_crop_x;
    td_u32 mapping_crop_y;
    td_u32 mapping_crop_w;
    td_u32 mapping_crop_h;
} dpu_hal_mapping_crop;

#define DPU_HAL_MATRIX_ARRAY_LENGTH 3
typedef struct {
    td_float matrix[DPU_HAL_MATRIX_ARRAY_LENGTH][DPU_HAL_MATRIX_ARRAY_LENGTH];
} dpu_hal_matrix;

typedef struct {
    disp_src2_mapping_ctrl mapping_ctrl;
    disp_t2r0_ctrl t2r0_ctrl;
    dpu_hal_matrix pos_m;
    dpu_hal_matrix inv_m;
    dpu_hal_mapping_crop crop;
} dpu_hal_affine_cfg;

typedef struct {
    td_u32 iop;
    td_u32 wbc_prio;
    td_u32 burst_type;
    td_u32 des_addr;
    td_u32 des_width;
    td_u32 des_height;
    td_u32 des_stride;
    td_u32 wbc_bind_mode;
    disp_wbc_dhd_ctrl d_wbc_dhd_ctrl;
    disp_wbc_dhd_cmd_ctrl d_wbc_dhd_cmd_ctrl;
} dpu_hal_wbc_info;

typedef enum {
    DPU_HAL_CSC_BIND_TO_V0 = 0,
    DPU_HAL_CSC_BIND_TO_DHD
} dpu_hal_csc_bind_mode;

td_s32 dpu_hal_init(const dpu_clk_scene_info *scene_info);
td_void dpu_hal_deinit(const dpu_clk_scene_info *scene_info);

/* ---------------- DISP ---------------- */
td_void hal_disp_init(void);
td_void hal_disp_deinit(void);

td_void dpu_hal_disp_set_timing(dpu_disp_timing *timing);

td_u32 dpu_hal_disp_get_ulps_mipi_state(void);
td_u32 dpu_hal_disp_get_int_state(void);
td_void dpu_hal_disp_clear_int_state(td_u32 int_type);
td_void dpu_hal_disp_set_int_enable(td_u32 int_type, td_bool enable);
td_void dpu_hal_disp_set_int_thd(td_u32 int_type, td_u32 thd);
td_u32  dpu_hal_disp_get_int_mipi_state(void);
td_void dpu_hal_disp_clear_int_mipi_state(void);
td_void dpu_hal_disp_set_dhd0_te_mode(td_u8 vsync_te_mode);
td_void dpu_hal_disp_set_int_mipi_enable(td_bool enable);
td_void dpu_hal_disp_set_dhd_te_timeout_en(td_bool enable);
td_void dpu_hal_disp_set_dhd_te_timeout_thd(td_u32 thd);
td_void dpu_hal_disp_set_start_pos(td_u32 start_pos);
td_void dpu_hal_disp_set_chnl_regup(void);
td_void dpu_hal_disp_set_chnl_enable(td_bool enable);
td_void dpu_hal_disp_set_cbm_priority(dpu_disp_cbm_mix_layer layer, dpu_disp_cbm_mix_priority prio);
td_u32 dpu_hal_disp_get_cbm_priority(void);
td_void dpu_hal_disp_set_partial_cfg_vld(td_bool is_valid);
td_void dpu_hal_disp_set_partial_cmd_info(dpu_hal_partial_cmd_info *cmd_info);
td_void dpu_hal_disp_set_frame_ready_mode(td_u32 frm_rdy_mode);
td_void dpu_hal_disp_set_te_isr_dly_lines(td_u32 delay_lines);
td_void dpu_hal_disp_set_frame_ready(void);
td_void dpu_hal_disp_set_cbar(td_bool enable);
td_bool dpu_hal_disp_get_vblank(void);
td_void dpu_hal_disp_get_chksum(td_u32 *chksum_y, td_u32 *chksum_u, td_u32 *chksum_v);
td_void dpu_hal_disp_set_mir_en(td_bool enable);
td_void dpu_hal_disp_set_mapping_en(td_bool mapping_en);
td_void dpu_hal_disp_set_affine_info(dpu_hal_affine_cfg *hal_cfg, td_u32 lyaer_id);
td_void dpu_hal_disp_set_wbc_info(dpu_hal_wbc_info *hal_wbc);
td_void dpu_hal_layer_set_cmp(td_u32 layer_id, td_bool ice_en, disp_cmp_ctrl *cmp_ctrl);

/* ---------------- LAYER ---------------- */
td_void dpu_hal_layer_set_ck_gt_en(td_u32 layer_id, td_bool enable);
td_void dpu_hal_layer_set_palpha_en(td_u32 layer_id, td_bool enable);
td_void dpu_hal_layer_set_fdr_chksum_en(td_u32 layer_id, td_bool chksum_en);
td_void dpu_hal_layer_get_fdr_chksum(td_u32 layer_id, td_u32 *chksum_ar, td_u32 *chksum_gb);
td_void dpu_hal_layer_set_global_alpha(td_u32 layer_id, td_u8 global_alpha);
td_void dpu_hal_layer_set_disp_rect(td_u32 layer_id, const ext_rect *disp_rect);
td_void dpu_hal_layer_set_crop_rect(td_u32 layer_id, const ext_rect *crop_rect);
td_void dpu_hal_layer_set_alpha0_alpha1(td_u32 layer_id, td_u8 alpha0, td_u8 alpha1);
td_void dpu_hal_layer_set_fmt(td_u32 layer_id, dpu_hal_fmt fmt);
td_void dpu_hal_layer_set_width(td_u32 layer_id, td_u32 width);
td_void dpu_hal_layer_set_height(td_u32 layer_id, td_u32 height);
td_void dpu_hal_layer_set_stride(td_u32 layer_id, td_u32 stride);
td_void dpu_hal_layer_set_addr(td_u32 layer_id, td_u32 addr);
td_void dpu_hal_layer_set_dcmp(td_u32 layer_id, layer_gfx_dcmp_ctrl *dcmp_ctrl, layer_gfx_htc_online_dcmp0 *dcmp0,
                               layer_gfx_htc_online_dcmp1 *dcmp1);
td_void dpu_hal_layer_set_enable(td_u32 layer_id, td_bool show);
td_void dpu_hal_layer_set_testpattern(td_u32 layer_id, td_bool on);
td_void dpu_hal_layer_set_mute_en(td_u32 layer_id, td_bool mute_en);
td_void dpu_hal_layer_set_mute_color(td_u32 layer_id, td_u32 mute_y, td_u32 mute_u, td_u32 mute_v);
td_void dpu_hal_layer_set_colorkey(td_u32 layer_id, td_bool key_en, td_u32 key_value);
td_void dpu_hal_layer_set_premulti_en(td_u32 layer_id, td_bool premulti_en, td_bool is_mapping);
td_void dpu_hal_layer_set_reg_up(td_u32 layer_id);

td_void dpu_hal_layer_set_yuv_base_info(td_u32 layer_id, dpu_hal_yuv_info *yuv_info);
td_void dpu_hal_layer_set_flip_en(td_u32 layer_id, td_bool enable);

td_void dpu_hal_get_acc_info(dpu_hal_acc_info *acc_info);
td_void dpu_hal_set_acc_lut(td_s32 acc_lut[], td_u32 num);
td_void dpu_hal_set_acc_en(td_u32 acc_en);
td_void dpu_hal_set_acc_stat_en(td_u32 acc_stat_en);
td_void dpu_hal_set_acc_ctrl(td_u32 acc_hstart, td_u32 acc_hend, td_u32 acc_vstart, td_u32 acc_vend);
td_void dpu_hal_set_acc_stat_ctrl(td_u32 acc_stat_hstart, td_u32 acc_stat_hend, td_u32 acc_stat_vstart,
    td_u32 acc_stat_vend);

#ifdef GPU_1A22
td_void dpu_hal_set_rotation_cfg(td_u32 layer_id, drv_gfx_rotation_mode mode);
td_void dpu_hal_disp_set_csc_sat(td_float sat);
td_void dpu_hal_set_dhd_csc_en(td_u32 csc_en);
#else
static inline td_void dpu_hal_set_rotation_cfg(td_u32 layer_id, drv_gfx_rotation_mode mode)
{
    uapi_unused(layer_id);
    uapi_unused(mode);
}
static inline td_void dpu_hal_disp_set_csc_sat(td_float sat) {uapi_unused(sat);};
static inline td_void dpu_hal_set_dhd_csc_en(td_u32 csc_en) {uapi_unused(csc_en);};
#endif

#if (defined(GPU_1A22) || defined(GPU_3322))
td_void dpu_hal_set_dhd0_ctrl_uf_offline_en(td_void);
td_void dpu_hal_disp_set_csc_bind(drv_dpu_color_type color_type);
td_void dpu_hal_set_dhd0_des_chksum_en(td_u32 enable);
td_u32 dpu_hal_disp_get_dhd0_intf_pcnt_cnt(void);
#else
static inline td_void dpu_hal_disp_set_csc_bind(drv_dpu_color_type color_type)
{
    uapi_unused(color_type);
}

static inline td_void dpu_hal_set_dhd0_des_chksum_en(td_u32 enable)
{
    uapi_unused(enable);
}

static inline td_u32 dpu_hal_disp_get_dhd0_intf_pcnt_cnt(void)
{
    return 0;
}

#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* DRV_GRAPHICS_HAL_DPU_HAL_H */
