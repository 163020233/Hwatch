/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: head file of crg
 * Author: AuthorNameMagicTag
 * Create: 2025-04-23
 */

#ifndef HAL_VIPROC_INTF_H
#define HAL_VIPROC_INTF_H

#include "drv_mcp_dependency.h"
#include "drv_vb_ext.h"
#include "drv_isp_comm_struct.h"
#include "viu_base_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* #ifdef __cplusplus */

#ifndef CONFIG_CHIP_3322
typedef struct {
    td_u32 enable;
    td_u64 head_y_addr;
    td_u64 head_c_addr;
    td_u64 y_addr;
    td_u64 c_addr;
    td_u32 y_head_stride;
    td_u32 c_head_stride;
    td_u32 y_stride;
    td_u32 c_stride;
    td_u32 width;
    td_u32 height;
    td_u32 dcmp_mode;
    td_u32 dcmp_en;
    td_u32 l_dcmp_en;
    td_u32 bit_width;
    td_u32 pix_fmt;
    td_u32 ver_offset;
    td_u32 hor_offset;
} viu_hal_cur_pic_config;

typedef struct {
    td_u32 x;
    td_u32 y;
    td_u32 width;
    td_u32 height;
    td_u32 crop_en;
} viu_hal_crop;

typedef enum {
    VIU_HAL_PT_SELECT_PT = 0,
    VIU_HAL_PT_SELECT_SRC = 1,
    VIU_HAL_PT_SELECT_BUTT
} viu_hal_pt_select;

typedef struct {
    td_s32 fpd_en;
    td_u64 phyddr;
    td_u64 stride;
    td_u32 bit_width;
    td_bool dsf_sel;
    td_bool isp_dsf_en;
    td_s32 isp_dsf_bit_tru;
    td_u32 isp_dsf_cropinx_start;
    td_u32 isp_dsf_cropinx_step;
    td_u32 isp_dsf_cropoutx_start;
    td_u32 isp_dsf_cropoutx_step;
    td_u32 isp_dsf_cropiny_start;
    td_u32 isp_dsf_cropiny_step;
    td_u32 isp_dsf_cropouty_start;
    td_u32 isp_dsf_cropouty_step;
    td_u32 isp_dsf_dsen;
    td_u32 isp_dsf_factor;
    td_u32 isp_dsf_dswidth;
    td_u32 isp_dsf_dsheight;
    td_bool isp_dsf_lpfen;
    td_s32 isp_dsf_update_mode;
    td_s32 isp_dsf_cpi_fix_timing_stat;
    td_s32 isp_dsf_newer;
    td_u32 isp_dsf_width;
    td_u32 isp_dsf_height;
} viu_hal_dsf_hw_cfg;

typedef struct {
    td_u64 head_y_addr;
    td_u64 head_c_addr;
    td_u64 y_addr;
    td_u64 c_addr;
    td_u32 y_head_stride;
    td_u32 c_head_stride;
    td_u32 y_stride;
    td_u32 c_stride;
    td_u32 width;
    td_u32 height;
    td_bool chn_en;
    td_u32 dither_mode;
    td_u32 dither_en;
    td_u32 pix_fmt;
    td_u32 tile_en;
    td_u32 bit_width;
    td_u32 hor_offset;
    td_u32 uv_invert;
    td_bool flip;
    td_bool mirror;
    td_u32 cmp_mode;
    td_u32 cmp_en;
    td_u32 ext_y_stride;
    td_u32 ext_u_stride;
    td_u32 ext_v_stride;
    td_u64 ext_y_addr;
    td_u64 ext_u_addr;
    td_u64 ext_v_addr;
} viu_hal_chn_pic_config;

typedef struct {
    viu_hal_cur_pic_config cur_pic_config;
    ext_vb_cmp_para_output y_cmp_cfg;
    ext_vb_cmp_para_output c_cmp_cfg;

    viu_hal_crop crop;
    viu_hal_chn_pic_config chn_pic_config;
    ext_vb_cmp_para_output y_output;
    ext_vb_cmp_para_output c_output;
    viu_hal_dsf_hw_cfg dsf_hw_config;

    viu_hal_cur_pic_config wdr_pic_config;
    ext_vb_cmp_para_output wdr_y_output;
    ext_vb_cmp_para_output wdr_c_output;

    /* isp */
    viproc_3dnr_regs_type *st3_dnr_regs;

    td_bool online_int_mode;
    td_bool isp_be_en;
    td_bool isp_be_bypass;
    td_bool fifo_en;
    td_u32 img_pro_mode;

    td_bool isp_be_addr_en;
    td_u64 be_addr;
    td_u64 node_addr;

    td_u64 drc_para_addr;
    td_u64 drc_para_out_addr;
    td_u64 ldci_para_addr;
    td_u64 ldci_para_out_addr;
    td_u64 isp_stt_addr;
    td_u64 isp_stt_addr_pre;

    td_u32 in_pix_fmt;
    td_u32 out_pix_fmt;

    td_bool mcds_vcds_en;
} hal_viproc_cfg;
#endif

td_void hal_viproc_prepare(viu_proc_id pipe_id);
td_void hal_viproc_unprepare(viu_proc_id pipe_id);

td_s32 hal_viproc_init(viu_proc_id proc_id);
td_void hal_viproc_deinit(viu_proc_id proc_id);

td_void hal_viproc_start(viu_proc_id pipe_id, td_bool vicap_online);
td_void hal_viproc_stop(viu_proc_id pipe_id, td_bool vicap_online);

td_void hal_viproc_work_mode(viu_proc_id proc_id, td_bool online);

#ifndef CONFIG_CHIP_3322
td_s32 hal_viproc_init_wdr(viu_proc_id proc_id);

td_void hal_viproc_enable(viu_proc_id proc_id, td_bool enable);

td_void hal_viproc_process_node(viproc_regs_type *node, hal_viproc_cfg *viproc_cfg, isp_cfg_rec *be_cfg_buf);
td_void hal_viproc_process_isp_node(viproc_regs_type *node, isp_cfg_rec *be_cfg, hal_viproc_cfg *viproc_cfg);

td_s32 hal_viproc_hw_start(viu_proc_id proc_id, td_u64 next_phy_addr, td_bool bstep);
td_void hal_viproc_set_next(viproc_regs_type *node, td_u64 phy_addr);
td_void hal_viproc_set_node_id(viproc_regs_type *node, td_u32 node_id);
td_u32 hal_viproc_get_int_status(viu_proc_id proc_id);
td_void hal_viproc_clear_int_statuc(viu_proc_id proc_id, td_u32 proc_int);
td_void hal_viproc_manual_reg_new(viu_proc_id proc_id);
td_void hal_viproc_reg_new(viu_proc_id proc_id);

td_void hal_viproc_set_wrap(viu_proc_id proc_id, td_bool enable, td_u32 cache_line);
td_void hal_viproc_get_node_id(viu_proc_id proc_id, td_u32 *node_id);
td_void hal_viproc_set_wdr(viu_proc_id proc_id, viproc_regs_type *node, viu_hal_cur_pic_config *wdr_cfg,
    ext_vb_cmp_para_output *wdr_cmp);
td_void hal_viproc_set_wdr_cmp_mode(viu_proc_id proc_id, viproc_regs_type *node, td_u32 bayer_mode);
td_u32 hal_viproc_get_hw_cycle_cnt(viu_proc_id proc_id);
void viu_hal_limit_check(const viproc_regs_type *node);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif