/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __FSP_H__
#define __FSP_H__

#include "vfmw.h"
#include "vfmw_osal.h"
#include "vfmw_pdt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define FSP_OK  0
#define FSP_ERR (-1)

#ifdef VFMW_CHAN_NUM
#define FSP_CHAN_NUM VFMW_CHAN_NUM
#else
#define FSP_CHAN_NUM 1
#endif

#define FSP_FRAME_NUM VFMW_MAX_FRAME_NUM

#define FSP_PMV_NUM  17
#define FSP_USED_NUM 4

#define FSP_MEM_CFG 0
#define FSP_MEM_VMM 1

#define FSP_PHY_TYPE_BUTT    0
#define FSP_PHY_TYPE_DECODE  1
#define FSP_PHY_TYPE_DISPLAY 2

#define FSP_STORE_TYPE_FRAME 0
#define FSP_STORE_TYPE_FIELD 1
#define FSP_STORE_TYPE_BUTT  2

#define FSP_DEC_STATE_IDLE 0
#define FSP_DEC_STATE_WAIT 1
#define FSP_DEC_STATE_OVER 2
#define FSP_DEC_STATE_ERR  3

#define FSP_REF_SKIP 100
#define FSP_OUTER_PARA_SIZE VFMW_OUTER_PARA_SIZE

typedef enum {
    FSP_DISP_STATE_IDLE = 0,
    FSP_DISP_STATE_ALLOC,
    FSP_DISP_STATE_DEC,
    FSP_DISP_STATE_WAIT,
    FSP_DISP_STATE_BUTT
} fsp_fs_state;

typedef enum {
    FSP_STATE_NULL = 0,
    FSP_STATE_OPEN,
    FSP_STATE_INIT,
    FSP_STATE_EXIT,
    FSP_STATE_BUTT
} fsp_state;

typedef enum {
    FSP_MEM_TYPE_DEC = 0,
    FSP_MEM_TYPE_DISP,
    FSP_MEM_TYPE_PMV,
    FSP_MEM_TYPE_NOR,
    FSP_MEM_TYPE_SEC,
    FSP_MEM_TYPE_BUTT,
} fsp_mem_type;

typedef enum {
    FSP_LOW_PERF = 0,
    FSP_HIGH_PERF,
    FSP_PERF_BUTT,
} fsp_perf;

typedef union {
    td_u8 data;
    struct {
        td_u8 top_state : 2;
        td_u8 bot_state : 2;
        td_u8 reserved : 4;
    } bits;
} fsp_dec_state;

typedef enum {
    FS_AIR_STATE_IDLE = 0,
    FS_AIR_STATE_USE,
    FS_AIR_STATE_OVER,
    FS_AIR_STATE_MAX,
} fsp_air_state;

typedef enum {
    FS_OUT_FRM_UNBIND_READY,
    FS_OUT_FRM_BIND_READY,
    FS_OUT_FRM_MAX,
} fsp_out_frm_state;

typedef struct {
    td_s32 (*mark_image_state)(td_s32, vfmw_image *, td_s32, td_u32);
    td_s32 (*mark_field_flag)(td_s32, const vfmw_image *);
    td_s32 (*update_image_info)(td_s32, const vfmw_image *);
    td_void (*try_rels_image)(td_s32 chan_id);
} fsp_intf;

typedef struct {
    td_s64 mem_fd;
    td_u64 dma_buf;
    UADDR luma_phy_addr;
    td_u64 luma_vir_addr;
    UADDR chrom_phy_addr;
    td_u64 chrom_vir_addr;
    td_u32 image_stride;

    UADDR luma_outdisp_phy_addr;
    td_u64 luma_outdisp_vir_addr;
    UADDR chrom_outdisp_phy_addr;
    td_u64 chrom_outdisp_vir_addr;

    UADDR luma_2bit_phy_addr;
    td_u64 luma_2bit_vir_addr;
    UADDR chrom_2bit_phy_addr;
    td_u64 chrom_2bit_vir_addr;
    td_u32 image_stride_2bit;

    td_u32 head_stride;
    td_u32 head_info_size;

    UADDR top_luma_phy_addr;
    UADDR top_chrom_phy_addr;
    UADDR btm_luma_phy_addr;
    UADDR btm_chrom_phy_addr;
    UADDR line_num_phy_addr;

    td_u32 vdh_2d_en;
    td_u32 tile_format;
    td_u32 compress_en;
    td_u32 bit_depth;

    td_u32 metadata_size;
    UADDR metadata_phy_addr;
    td_u64 metadata_vir_addr;
    td_u64 metadata_dma_buf;
    td_s64 mtdt_fd;
    td_u64 mtdt_fd_offset;

    td_s64 ext_att_handle;
    td_u32 ext_attr_offset;
    UADDR ext_att_phy_addr;
    td_u64 ext_att_vir_addr;
    td_s32 ext_att_buf_size;

    td_s64 ext_hfbc_handle;
    td_u64 ext_hfbc_vir_addr;

    td_u64 sec_info;
    td_u32 av1_film_tab_offset;
    td_u32 buf_size;
} image_buf;

typedef struct {
    td_u32 dec_num;
    td_u32 disp_num;
    td_u32 pmv_num;
    td_u32 nor_num;
    td_u32 sec_num;
} fsp_report;

typedef struct {
    td_u8 is_valid;
    td_u8 is_use;
    td_u8 is_map;
    td_u8 pmv_id;
    td_u32 tick;
    td_u64 dma_buf;
    td_u32 buf_size;
    UADDR phy_addr;
    td_u64 vir_addr;
    td_u64 sec_info;
} fsp_pmv;

typedef struct {
    td_u8 is_valid;
    td_u8 is_use;
    td_s32 fs_id;
    td_s64 mem_fd;
    td_u64 fd_offset;
    td_u64 dma_buf;
    UADDR phy_addr;
    td_u64 vir_addr;
    td_u32 buf_size;

    td_u64 sec_info;
} fsp_attach;

typedef struct {
    td_u8 is_valid;
    td_u8 is_use;
    td_u8 is_map;
    td_bool need_attach;

    td_s32 ref_cnt;
    td_s32 disp_cnt;
    td_s32 fs_id;
    td_u32 tick;

    td_s64 mem_fd;
    td_u64 dma_buf;
    UADDR phy_addr;
    td_u64 vir_addr;
    td_u32 buf_size;
    UADDR line_num_addr;

    /* used for overlay frm info mem */
    td_s64 info_handle;
    UADDR info_phy_addr;
    td_u64 info_vir_addr;
    td_u32 info_buf_size;
    td_u32 info_offset;

    td_s64 hfbc_handle;
    td_u64 hfbc_vir_addr;

    td_s32 head_stride;
    td_s32 head_info_size;

    td_u64 sec_info;
    td_u32 av1_film_tab_offset;
    fsp_air_state fs_air_state;
} fsp_phy;

typedef enum {
    FSP_TYPE_EXCLU = 0,
    FSP_TYPE_SHARE,
    FSP_TYPE_LINK,
    FSP_TYPE_DUMMY,
    FSP_TYPE_BUTT
} fsp_fs_type;

typedef struct {
    td_s8 logic_id;
    td_s8 link_id;
    td_s8 is_use;
    td_s8 is_ref;
    td_s8 is_in_voq;
    td_s8 is_dec_ref;
    td_s8 is_syn_drop;
    fsp_dec_state dec_state;
    td_s8 disp_state;
    td_s8 is_dummy;
    fsp_fs_type logic_type;
    td_u8 store_type;
    td_s32 err_flag;
    td_s32 ref_err;

    td_u32 is_dec_gap;
    td_u32 tr;
    td_u32 tick;

    fsp_phy *dec_fs;
    fsp_phy *disp_fs;
    fsp_pmv *pmv_fs;
    fsp_attach *nor_fs;
    fsp_attach *sec_fs;
    fsp_air_state fs_air_state;
    vfmw_image image;
} fsp_logic;

typedef struct {
    vfmw_vid_std vid_std;
    td_bool resize_ref;
    td_u32 need_dec_num;
    td_u32 need_disp_num;
    td_u32 need_pmv_num;
    td_u32 need_mtdt;
    td_u32 bit_depth;
    td_u32 dec_width;
    td_u32 dec_height;
    td_u32 disp_width;
    td_u32 disp_height;
    td_u32 disp_wofs;
    td_u32 disp_hofs;
    td_u32 frame_mbs_only;
    vfmw_splice_info splice_info;
} fsp_cfg;

typedef struct {
    td_s8 resize_succ;

    mem_need_info need;
    mem_need_info report;

    td_s32 wait_dec_size;
    td_s32 wait_pmv_size;
    td_s32 wait_disp_size;

    td_u32 apply_dec_num;
    td_u32 apply_pmv_num;
    td_u32 apply_disp_num;

    td_u32 have_dec_num;
    td_u32 have_pmv_num;
    td_u32 have_disp_num;
    td_u32 have_nor_num;

    td_u32 total_dec_num;
    td_u32 total_pmv_num;
    td_u32 total_disp_num;
} fsp_mem;

typedef struct {
    td_u8 sec_flag;
    td_u8 android_test;
    td_u32 need_film_grain;
    td_u32 extra_fs_num;
    td_s8 uvmos_en;
    td_s32 out_err_thr;
    fsp_perf perf;
    vfmw_cmp vcmp_mode;
    vfmw_bit_depth_attr disp_bit_depth_mode;
    vfmw_frm_alloc_type alloc_type;
    vfmw_frm_layout frm_layout;
    vfmw_air_dec_version air_version;
    vfmw_outer_frm_mode outer_frm_mode;
    td_bool is_max_buf_size_en;
    td_u32 max_dec_width;
    td_u32 max_dec_height;
} fsp_attr;

typedef struct {
    td_u32 width;
    td_u32 height;
    vfmw_vid_std std;
    td_u32 buf_size;
    td_u32 buf_num;
    td_u32 bit_depth;
} fsp_pvr_info;

typedef struct {
    td_s32 id;
    td_u32 unid;
    td_s32 state;
    td_u32 tick;
    td_void *handle;
    td_u32 wait_flush;
    td_u32 wait_unbind;
    os_sema sema;
    fsp_attr attr;

    fsp_cfg cfg;
    fsp_mem fs_mem;
    fsp_intf ext_intf;

    fsp_pvr_info pvr_info;

    fsp_logic logic_fs[FSP_FRAME_NUM];
    fsp_phy dec_fs[FSP_FRAME_NUM];
    fsp_phy disp_fs[FSP_FRAME_NUM];
    fsp_pmv pmv_fs[FSP_PMV_NUM];
    fsp_attach nor_fs[FSP_FRAME_NUM];
    fsp_attach sec_fs[FSP_FRAME_NUM];

    fsp_out_frm_state outer_frm_status;
} fsp_chan;

typedef struct {
    td_u32 reserve1;

    os_sema sema;
    os_lock lock[FSP_CHAN_NUM];
    os_lock img_format_lock;
    td_u32 ref_cnt;

    fsp_state state;
    td_u32 ctx_size;
    fsp_chan *chan[FSP_CHAN_NUM];

    /* for test */
    td_u32 no_dec_fs;
    td_u32 no_dsp_fs;
    td_u32 no_pmv_fs;
    td_u32 no_att_fs;

    td_u32 reserve2;
} fsp_entry;

typedef struct {
    td_u32 total_frm_num;
    td_u32 wait_disp_frm_num;
    td_u32 pvr_buf_num;
    td_bool has_dec_fs_free;
    td_bool has_pmv_fs_free;
    td_bool has_attach_fs_free;
} fsp_info;

td_s32 fsp_open(td_void);
td_s32 fsp_close(td_void);
td_s32 fsp_initial(td_void);
td_s32 fsp_exit(td_void);
td_s32 fsp_create(td_s32 chan_id);
td_s32 fsp_get_attr(td_s32 chan_id, fsp_attr *attr);
td_s32 fsp_set_attr(td_s32 chan_id, const fsp_attr *attr);
td_s32 fsp_reset(td_s32 chan_id);
td_s32 fsp_get_pic_wh(td_s32 chan_id, td_s32 *height, td_s32 *width, td_u32 *tick);
td_s32 fsp_config(td_s32 chan_id, const td_void *attr);
td_s32 fsp_empty(td_s32 chan_id);
td_s32 fsp_destroy(td_s32 chan_id);
td_s32 fsp_clear_logic(td_s32 chan_id, td_s32 logic_id, td_s32 hold_disp);
td_s32 fsp_clear_idle(td_s32 chan_id);
td_s32 fsp_set_ext_intf(td_s32 chan_id, const td_void *args);
td_s32 fsp_get_total_num(td_s32 chan_id);
td_s32 fsp_get_post_num(td_s32 chan_id, td_s32 width, td_s32 height);

td_s32 fsp_refresh(td_s32 chan_id);
td_s32 fsp_bind_req_fs(td_s32 chan_id, const vfmw_mem_report *mem);

fsp_entry *fsp_get_entry(td_void);
td_s32 fsp_set_image(td_s32 chan_id, td_s32 logic_id, const image_buf *img_buf);
vfmw_image *fsp_get_image(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_new_logic(td_s32 chan_id, fsp_fs_type type);
td_s32 fsp_link_logic(td_s32 chan_id, td_s32 link_id);
td_s32 fsp_null_logic(td_s32 chan_id);
fsp_logic *fsp_get_logic(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_new_phy(td_s32 chan_id, td_s32 logic_id, td_s32 share_disp_id);
UADDR fsp_get_phy(td_s32 chan_id, td_s32 logic_id);
UADDR fsp_get_disp_addr(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_new_pmv(td_s32 chan_id, td_s32 size);
UADDR fsp_get_pmv(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_free_pmv(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_set_ref(td_s32 chan_id, td_s32 logic_id, td_s32 is_ref);
td_s32 fsp_get_ref(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_set_ref_cnt(td_s32 chan_id, td_s32 logic_id, td_s32 delta);
td_s32 fsp_set_dec_ref(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_clear_dec_ref(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_get_dec_ref(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_set_wait(td_s32 chan_id, td_s32 logic_id, td_s32 structure);
td_s32 fsp_clear_dec(td_s32 chan_id, td_s32 logic_id);
td_u32 fsp_get_dec(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_set_disp(td_s32 chan_id, td_s32 logic_id, td_s32 disp_state);
td_s32 fsp_get_disp(td_s32 chan_id, td_s32 logic_id);
fsp_phy *fsp_get_dec_fs(td_s32 chan_id, td_s32 logic_id);
fsp_phy *fsp_get_disp_fs(td_s32, td_s32);
td_s32 fsp_set_store_type(td_s32 chan_id, td_s32 logic_id, td_s32 type);
td_s32 fsp_get_store_type(td_s32 chan_id, td_s32 logic_id);
td_s32 fsp_update_image_param(td_s32 chan_id, td_s32 logic_id, const vfmw_image *src_img);
td_s32 fsp_update_image_ext(td_s32 chan_id, td_s32 logic_id, const vfmw_image *src_img);
td_s32 fsp_check_img_dec_state(const vfmw_image *img, td_s32 state);
td_s32 fsp_clear_useless_dec_fs(td_s32 chan_id);
td_s32 fsp_check_fs_ok(td_s32 chan_id);
td_s32 fsp_get_mem_need_size(vfmw_svr_mem_need *mem);
td_bool fsp_is_logic_fs_free(const fsp_logic *logic_fs, td_bool is_disp_need);
td_bool fsp_is_dec_fs_free(const fsp_chan *chan, const fsp_phy *dec_fs);
td_bool fsp_is_disp_fs_free(const fsp_chan *chan, const fsp_phy *disp_fs);
td_bool fsp_is_pmv_free(const fsp_pmv *pmv_fs, td_u32 pmv_size);
td_bool fsp_has_free_phy(td_s32 chan_id);
td_bool fsp_is_attach_free(const fsp_attach *attach_fs);
td_s32 fsp_get_info(td_s32 chan_id, const td_void *args);
td_s32 fsp_set_fld_flag(td_s32 chan_id, td_s32 logic_id, td_s32 fld_flag);
td_s32 fsp_set_dec_state(td_s32 chan_id, td_s32 logic_id, td_s32 pic_struct, td_s32 err_ratio);
td_void fsp_img_format_lock(td_ulong *lock_flag);
td_void fsp_img_format_unlock(td_ulong *lock_flag);
td_s32 fsp_bind_extern_fs(td_s32 chan_id, const vfmw_ext_buf *ext_buf);
td_s32 fsp_unbind_extern_fs(td_s32 chan_id, const vfmw_ext_buf *ext_buf);
td_s32 fsp_set_frame_buf_mode(td_s32 chan_id, vfmw_frm_alloc_type type);
vfmw_frm_alloc_type fsp_get_frame_buf_mode(td_s32 chan_id);
td_void fsp_clear_unused_pmv(fsp_chan *chan);
td_s32  fsp_clear_unused_logic(td_s32 chan_id);
td_s32  fsp_set_wait_flush(td_s32 chan_id, td_u32 wait_flush_flag);
td_void fsp_rels_all_dec(fsp_chan *chan);
td_void fsp_clear_all_attach(fsp_chan *chan);
td_void fsp_flush(td_s32 chan_id);
fsp_chan *fsp_get_chan(td_s32 chan_id);
td_s32 fsp_bind_attach_fs(td_s32 chan_id, const vfmw_mem_attach *mem);
td_void fsp_set_air_state(td_s32 chan_id, td_s32 logic_id, fsp_air_state air_status);
td_s32 fsp_set_out_frm_state(td_s32 chan_id, const fsp_out_frm_state state);
td_bool fsp_is_dec_over(const fsp_logic *logic_fs);
fsp_chan *fsp_get_chan(td_s32 chan_id);
td_void fsp_lock(td_s32 chan_id, td_ulong *lock_flag);
td_void fsp_unlock(td_s32 chan_id, td_ulong *lock_flag);
td_void fsp_sema_down(td_void);
td_void fsp_sema_up(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __FSP_H__ */
