/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_drv.h
 * Author: AuthorNameMagicTag
 * Create: 2025-06-30
 */

#ifndef VIU_DRV_VICAP_H
#define VIU_DRV_VICAP_H

#include "drv_viu_struct.h"
#include "viu_hal.h"
#include "viu_public.h"
#include "drv_viu_vicap.h"
#include "viu_common_mem.h"
#include "hal_videv_intf.h"
#include "viu_extern.h"

#if defined(CONFIG_CHIP_3322)
#else
#include "wdr_drv.h"
#include "viu_drv_isp.h"
#include "fpn_drv.h"
#include "drv_viu.h"
#include "viu_common_mem.h"
#include "hal_videv_intf.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VIU_PIPE_RAW_QUEUE_MAX_DEPTH        8U

#define VIU_3DNR_MAX_AUTO_PARAM_NUM         16

#define is_mipi_intf(intf_mode)                           \
            (((intf_mode) == EXT_VIU_MODE_MIPI) \
            ||  ((intf_mode) == EXT_VIU_MODE_MIPI_YUV420_NORMAL) \
            ||  ((intf_mode) == EXT_VIU_MODE_MIPI_YUV420_LEGACY) \
            ||  ((intf_mode) == EXT_VIU_MODE_MIPI_YUV422) \
            ||  ((intf_mode) == EXT_VIU_MODE_LVDS))

#define is_coms_intf(intf_mode) \
    (((intf_mode) == EXT_VIU_MODE_BT656) \
            || ((intf_mode) == EXT_VIU_MODE_BT601) \
            || ((intf_mode) == EXT_VIU_MODE_DIGITAL_CAMERA) \
            || ((intf_mode) == EXT_VIU_MODE_BT1120_STANDARD) \
            || ((intf_mode) == EXT_VIU_MODE_BT1120_INTERLEAVED))

#define is_cap_report_irq(int_status) \
    ((int_status) & VIU_INDICATOR_MASK)

typedef struct {
    td_bool nr_en;
    td_bool rfr_en;
} viu_drv_pipe_attr;

typedef struct {
    td_bool chn_en;
    td_bool mirror;
    td_bool flip;
} viu_drv_chn_attr;

typedef struct {
    td_u64 y_addr;
    td_u64 u_addr;
    td_u64 v_addr;
    td_u64 vir_addr[3]; /* 3 -- y, u, v */
    td_u32 y_stride;
    td_u32 u_stride;
    td_u32 v_stride;

    td_u32 width;
    td_u32 height;

    td_u32 bit_width;
    td_u32 hor_offset;
    td_u32 ver_offset;

    ext_video_format video_format;
    ext_compress_mode compress_mode;
    ext_dynamic_range dynamic_range;

    mcp_soc_mod_id mod_id;

    td_u64 header_phy_addr[3]; /* 3 -- y, u, v */
    td_u32 header_stride[3];   /* 3 -- y, u, v */
    td_u64 header_vir_addr[3]; /* 3 -- y, u, v */

    ext_pixel_format pix_fmt;

    td_u32 field;
    td_u32 pool_id;
    td_bool send_ok;

    td_u64 pts;
    td_u32 ext_y_stride;
    td_u32 ext_u_stride;
    td_u32 ext_v_stride;
    td_u64 ext_y_addr;
    td_u64 ext_u_addr;
    td_u64 ext_v_addr;

    ext_video_supplement supplement;
} viu_drv_pic_info;

typedef struct {
    viu_drv_pipe_attr drv_pipe_attr;
    viu_drv_chn_attr drv_chn_attr[VIU_MAX_PHY_CHN_NUM];

    viu_drv_pic_info cur_pic;
    viu_drv_pic_info wdr_pic[ISP_WDR_CHN_MAX - 1];

    /* 这里可以不需要乒乓buffer,mkp层有乒乓就行了 */
    viu_drv_pic_info dest_pic[VIU_MAX_PHY_CHN_NUM][2]; /* 2 use to ping-pong */
    td_bool valid[VIU_MAX_PHY_CHN_NUM][2];  /* 2 use to ping-pong */
    td_u32 buf_id[VIU_MAX_PHY_CHN_NUM];
} viu_drv_proc_hw_info;

typedef struct {
    td_u64 pts;
    td_u64 phy_addr[VIU_MAX_SPLIT_NODE_NUM];
    td_void *be_stt_reg_array[VIU_MAX_SPLIT_NODE_NUM];
} viu_drv_be_stt_buf;

typedef struct {
    td_u32 cur_be_stt_index;
    viu_drv_be_stt_buf be_buf[2]; /* 2 used to ping-pong */
    td_u32 total_len;
    td_u32 reg_size;
} viu_drv_be_stt_buf_info;

typedef struct {
    td_u64 phy_addr;
    td_u64 vir_addr;
    td_u32 stride;
    td_u32 width; /* 图像宽度 */
    td_u32 height; /* 图像高度 */
    td_u32 bit_width;
    td_bool dis_buf_ok;
} viu_drv_dsf_pic_info;

typedef struct {
    td_bool after3_dnr;
    td_bool dis_enable;
    td_bool last_dis_buf_ok;
    td_u32 pool_id; /* pool id */
    td_u32 buf_size; /* buf大小 */
    td_u32 width; /* 图像宽度 */
    td_u32 height; /* 图像高度 */

#if defined(CONFIG_CHIP_3322)
#else
    viu_dis_dsf_hw_cfg dsf_hw_cfg;
    viu_drv_dsf_pic_info drv_dsf_pic_info;
#endif
} viu_drv_dis_info;

#if defined(CONFIG_CHIP_3322)
#else
typedef struct {
    ext_isp_operation_mode opt_mode; /* adaptive NR */
    ext_viu_pipe_nrx_param_v2 nrx_manual; /* NRX V2 param for manual */
    td_u32 param_num;
    td_u32 iso[VIU_3DNR_MAX_AUTO_PARAM_NUM];
    ext_viu_pipe_nrx_param_v2 nrx_auto[VIU_3DNR_MAX_AUTO_PARAM_NUM];
} viu_nrx_cfg_v2;

typedef struct {
    ext_viu_nr_version nr_ver; /* 当前运行版本号 */
    viu_nrx_cfg_v2 nrx_cfg;
} viu_nrx_cfg;
#endif

typedef struct {
    ext_viu_pipe_attr viu_pipe_attr;
    ext_viu_chn_attr viu_chn_attr[VIU_MAX_PHY_CHN_NUM];
    ext_viu_pipe_frame_type frame_type;
    ext_viu_dump_attr dump_attr;

    td_bool pipe_post_crop_update;
    td_bool pipe_attr_update;
    td_bool viu_chn_attr_update[VIU_MAX_PHY_CHN_NUM];
    td_bool dump_attr_update;
    td_bool frame_type_update;

#if defined(CONFIG_CHIP_3322)
#else
    viu_gdc_ldc_attr chn_ldc_attr[VIU_MAX_PHY_CHN_NUM];
    viu_gdc_spread_attr spread_attr[VIU_MAX_PHY_CHN_NUM];
    ext_viu_ext_chn_attr ext_chn_attr[VIU_MAX_EXT_CHN_NUM];

    td_bool ldc_attr_update[VIU_MAX_PHY_CHN_NUM];
    td_bool spread_attr_update[VIU_MAX_PHY_CHN_NUM];
    td_bool viu_ext_chn_attr_update[VIU_MAX_EXT_CHN_NUM];

    viu_nrx_cfg nrx_cfg;
    viu_nrx_cfg nrx_ai_cfg;
    td_bool nr_param_update;
#endif
} viu_pipe_drv_attr;

typedef struct {
    td_s32 pipe_id;
    td_s32 master_pipe;
    td_s32 bind_viu_dev;

    td_s32 pipe_lost_frm_key;

    td_bool gdc_config_update[VIU_MAX_PHY_CHN_NUM]; /* 用于替换g_viu_gdc_opt_ctx[viu_pipe_id].config[viu_chn_id] */
    td_bool set_chn_attr[VIU_MAX_CHN_NUM];

    viu_crop_calc chn_crop_calc_info[VIU_MAX_CHN_NUM];
    ext_isp_size cur_size;

    viu_pipe_drv_attr pipe_drv_attr_setting;
    viu_pipe_drv_attr pipe_drv_attr_working;
    td_bool first_frame;
    ext_isp_frame_interrupt_attr frame_int_attr; /* 延时上报帧起始中断 */

#if defined(CONFIG_CHIP_3322)
#else
    viu_dev_wdr_attr wdr_ctx;

    td_u32 *iso;
    ext_viu_pipe_nrx_param_v2 *nrx_param;
    ext_viu_pipe_nrx_param_v2 nrx_auto_param_v2_out;
    ext_viu_pipe_nrx_param_v2 ai_nrx_auto_param_v2_out;

    /* cover */
    viu_drv_cover_info chn_coverex[VIU_MAX_CHN_NUM];
    viu_drv_osd_info chn_osd[VIU_MAX_CHN_NUM];
    viu_drv_proc_hw_info drv_proc_hw_info;

    /* ISP  */
    viu_mem_info be_stt_mem;
    viu_drv_be_stt_buf_info be_stt_buf_info;
    isp_cfg_rec isp_be_buf;
    td_u32 isp_be_reg_size;
    isp_be_sync_para be_sync_param;

    td_s32 be_buf_cnt;

    /* FPN */
    td_void *frame_vir_addr;
    ext_viu_fpn_attr ext_viu_fpn_attr;
    td_u32 sensor_bit_width;

    /* DRC */
    viu_mem_info drc_buf_info;

    /* 3DNR */
    td_bool img_debug;
    td_bool update_nr;
    viproc_3dnr_regs_type st3_dnr_regs;

    /* 6 dof dis */
    viu_drv_dis_info dis_info[VIU_MAX_PHY_CHN_NUM];
    td_u32 isp_iso;
    td_bool ai_set;
#endif
} viu_pipe_drv_ctx;

typedef enum {
    VIU_PT_INPUT_MIPI = 0,
    VIU_PT_INPUT_COMS = 1,
    VIU_PT_INPUT_BUTT
} viu_pt_input;

extern viu_dev_drv_ctx g_ast_viu_drv_dev_ctx[VIU_MAX_DEV_NUM];
extern viu_pipe_drv_ctx *g_past_pipe_drv_ctx[MCP_VICAP_INSTANCE_MAX];

/* vicap */
td_u32 viu_drv_get_chn_min_width(td_s32 pipe_id, td_s32 chn_id);
td_u32 viu_drv_get_chn_min_height(td_s32 pipe_id, td_s32 chn_id);
td_s32 viu_drv_check_frame_rate(const ext_isp_frame_rate_ctrl *frame_rate);

td_u32 viu_drv_get_yuv_stride_by_bit_width(td_u32 width, td_u32 bit_width, ext_compress_mode compress_mode);

td_u32 viu_drv_trans_pixel(ext_pixel_format pix_fmt);

td_u32 viu_drv_trans_bit_width(ext_pixel_format pix_fmt, ext_dynamic_range dynamic_range);

td_u32 viu_drv_get_sensor_bit_width(td_s32 viu_dev_id);

td_void viu_drv_set_online_sel(td_u32 proc_id, td_u32 wdr_chn_num, td_s32 viu_pipe_id);
td_s32 viu_drv_get_mast_pipe_by_dev(td_s32 viu_pipe_id, td_s32 viu_dev_id);

td_u32 viu_drv_get_pipe_max_size(td_s32 pipe_id);

ext_isp_data_bitwidth viu_drv_trans_dynamic_range2_bit_width_en(ext_pixel_format pix_fmt,
    ext_dynamic_range dynamic_range);

#define viu_drv_get_dev_ctx(viu_dev_id) (&g_ast_viu_drv_dev_ctx[viu_dev_id])

td_bool viu_drv_check_func_support(td_s32 dev_id, td_s32 pipe_id, td_s32 chn_id, td_u32 cmd);

td_s32 viu_comm_check_video_frame_valid(td_s32 pipe_id, const ext_isp_video_frame *video_frame, td_bool is_wdr);

td_s32 viu_drv_set_auto_pattern_info(td_u32 timing_en, td_u32 data_en, td_u32 frame_rate);

td_s32 viu_drv_set_split_num(td_u32 num1, td_u32 num2);
td_s32 viu_drv_proc_split_info(td_void *seqfile);

td_void viu_drv_set_default_work_mode(td_void);

#if defined(CONFIG_CHIP_3322)
#else
viproc_be_regs_type *viu_drv_get_isp_be_reg(td_s32 viu_pipe_id);
viproc_regs_type *viu_drv_get_proc_reg(td_s32 viu_pipe_id);

td_s32 viu_drv_get_pipe_ai_nrxv2_param(td_s32 viu_pipe_id, ext_viu_pipe_nrx_param *nr_x_param);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
