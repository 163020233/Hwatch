/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __PRODUCT_H__
#define __PRODUCT_H__

#include "vfmw_osal.h"
#include "dbg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    ATTR_GET_BEGIN = 0,
    ATTR_GET_CAP,
    ATTR_GET_OUT_FMT,
    ATTR_GET_SEG_BUF_SIZE,
    ATTR_GET_LICENSE,
    ATTR_GET_VMIN,
    ATTR_GET_END,

    ATTR_SET_BEGIN = 100,
    ATTR_SET_CLK,
    ATTR_ENABLE_VDH,
    ATTR_DISABLE_VDH,
    ATTR_RESET_VDH,
    ATTR_SET_VMIN,
    ATTR_COMP_VMIN,
    ATTR_SET_PERF,
    ATTR_SET_END,
} attr_id;

typedef enum {
    ATTACH_NONE,
    ATTACH_DEC,
    ATTACH_DISP,
    ATTACH_BUTT,
} mem_attach;

typedef struct {
    td_u32 mem_format; /* 0:linear */
    td_u32 bit_depth;
    td_u32 image_width;
    td_u32 image_height;
    td_u32 image_stride;
    td_u32 image_stride_2bit;
    td_u32 image_disp;
    td_u32 vid_std;

    UADDR luma_phy_addr;
    UADDR chrom_phy_addr;
    UADDR luma_2bit_phy_addr;
    UADDR chrom_2bit_phy_addr;

    td_u8 *luma_vir_addr;
    td_u8 *chrom_vir_addr;
    td_u8 *luma_2bit_vir_addr;
    td_u8 *chrom_2bit_vir_addr;
} pdt_yuv;

typedef struct {
    td_u32 is_disp_need;
    td_s32 compress_en;
    td_u8 bit_depth;
    td_u32 disp_num;
    td_u32 disp_size;
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 disp_stride;
    td_u32 head_stride;
    td_u32 head_size;
} mem_disp_info;

typedef struct {
    td_u8 bit_depth;
    td_u32 dec_num;
    td_u32 dec_size;
    td_u32 dec_width;
    td_u32 dec_height;
    td_u32 dec_stride;
    td_s32 compress_en;
    td_u32 head_stride;
    td_u32 head_size;
    td_u32 line_num_offset;
    td_u32 tile_cdf_offset;
    td_u32 segid_offset;
    td_u64 av1_film_tab_offset;
} mem_dec_info;

typedef struct {
    td_u32 pmv_num;
    td_u32 pmv_size;
    td_u32 line_num_size;

    mem_attach attach;
    td_u32 nor_num;
    td_u32 nor_size;
    td_u32 sec_size;
    td_u32 linear_en;

    mem_dec_info dec_info;
    mem_disp_info disp_info;
    td_u32 extra_fs_num;
    td_u32 extra_pmv_num;
} mem_need_info;

typedef struct {
    td_s32 chan_id;
    vfmw_frm_alloc_type alloc_type;
    vfmw_cmp cmp_mode;
    vfmw_bit_depth_attr disp_bit_depth_mode;
    vfmw_frm_layout frm_layout;
    td_u32 extra_fs_num;
    td_bool add_extra;
    vfmw_vid_std vid_std;
    td_u32 bit_depth;
    td_u32 need_mtdt;

    td_bool is_max_buf_size_en;
    td_u32 max_dec_width;
    td_u32 max_dec_height;

    td_u32 dec_width;
    td_u32 dec_height;
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 need_dec_num;
    td_u32 need_pmv_num;
    td_u32 need_disp_num;

    td_u32 slot_width;
    td_u32 slot_height;
    td_u32 disp_slot_width;
    td_u32 disp_slot_height;
    td_u32 need_film_grain;
    td_u32 frame_mbs_only;
} mem_req_info;

typedef struct {
    td_u32 valid;
    td_u32 av1_en;
    td_u32 avs3_en;
    td_u32 core_num;
} pdt_license;

typedef struct {
    td_u32 valid;
    td_u32 vdh_vmin;
    td_u32 mdc_vmin;
} pdt_vmin;

typedef struct {
    td_s32 chan_id;
    td_u32 img_wid;
    td_u32 img_hig;
    td_u32 frm_rate;
} pdt_perf;

typedef enum {
    VFMW_ID_SCD = 0,
    VFMW_ID_VDH,
    VFMW_ID_MAX,
} vfmw_veto_id;

typedef struct {
    td_u32 veto_mask;
    td_u32 count[VFMW_ID_MAX];
} vfmw_veto_info;

typedef struct {
    chip_info pdt_chip;
    board_info pdt_board;
    td_bool smmu_support;
    td_bool tee_support;
    td_u32 vdh_max_width;
    td_u32 vdh_max_frate;
    td_u32 vdh_clk_max;
    td_u32 vdh_clk_reg;
    td_u32 vdh_clk_dyn;
    td_u32 mdc_clk_max;
    td_bool vdh_lowpower;
    td_u8 opt_vp9_av1;
    td_u8 opt_real8_real9;
    td_u32 h264_ext_slc_num;
    vfmw_veto_info pdt_veto;
    td_bool linear_no_gap_support;
    os_lock spin_lock;
    os_lock apb_spin_lock;
    td_u32 apb_veto_mask;
} vfmw_product_info;

td_s32 pdt_init(td_void);
td_s32 pdt_exit(td_void);
td_s32 pdt_suspend(const td_void *args);
td_s32 pdt_resume(const td_void *args);
td_s32 pdt_get_attr(attr_id attr_id, td_void *args);
td_s32 pdt_set_attr(attr_id attr_id, const td_void *args);
td_s32 pdt_get_mem_req(mem_req_info *req, mem_need_info *need);
td_bool pdt_need_attach_mem(vfmw_vid_std vid_std, mem_need_info *need);
td_s32 pdt_get_dps_num(vfmw_vid_std std, td_s32 *num);
vfmw_product_info* pdt_get_product_info(td_void);
td_void pdt_set_product_info(const vfmw_product_info *info);
td_u32 pdt_get_y8b_data(pdt_yuv *yuv, td_u32 h, td_u32 w);
td_u32 pdt_get_c8b_data(const pdt_yuv *yuv, td_u32 h, td_u32 w);
td_u32 pdt_get_ynb_data(pdt_yuv *yuv, td_u32 h, td_u32 w);
td_u32 pdt_get_cnb_data(const pdt_yuv *yuv, td_u32 h, td_u32 w);
td_u32 pdt_gety_data(pdt_yuv *yuv, td_u32 h, td_u32 w);
td_u32 pdt_getu_data(const pdt_yuv *yuv, td_u32 h, td_u32 w);
td_u32 pdt_getv_data(pdt_yuv *yuv, td_u32 h, td_u32 w);
td_s32 pdt_yuv_2d(const vfmw_image *img, td_u8 *dst);
td_void air_dec_get_cap(vfmw_air_cap *air_cap);
td_void pdt_init_mmu_reg_addr(td_u8 *vir_addr);
td_void pdt_init_product_info(td_void);
td_void pdt_deinit_product_info(td_void);
td_s32 pdt_add_sleep_veto(vfmw_veto_id id);
td_s32 pdt_remove_sleep_veto(vfmw_veto_id id);
td_s32 pdt_check_power_is_off(td_bool *is_power_off);
td_s32 pdt_add_sleep_apb_veto(vfmw_veto_id id);
td_s32 pdt_remove_sleep_apb_veto(vfmw_veto_id id);
td_s32 pdt_check_apb_is_off(td_bool *is_apb_off);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
