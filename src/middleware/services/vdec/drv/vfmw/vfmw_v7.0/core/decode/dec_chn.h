/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_CHAN_H__
#define __DEC_CHAN_H__

#include "dec_base.h"
#include "dec_hal.h"
#include "dec_post.h"
#include "dps.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEC_MAX_CHAN_NUM VFMW_CHAN_NUM
#define DEC_MAX_TASK_NUM MAX_DPS_NUM

typedef enum {
    DEC_TASK_IDLE = 0,
    DEC_TASK_DPSERR,
    DEC_TASK_START,
    DEC_TASK_SPLITOK,
    DEC_TASK_REFOK,
    DEC_TASK_MEMOK,
    DEC_TASK_CFGMSGOK,
    DEC_TASK_DECODING,
    DEC_TASK_TEMPCANCEL,
    DEC_TASK_PERMCANCEL,
    DEC_TASK_BSPINT,
    DEC_TASK_PXPINT,
    DEC_TASK_MAX
} dec_task_state;

typedef enum {
    DEC_CHAN_STATE_NULL = 0,
    DEC_CHAN_STATE_SUSPEND,
    DEC_CHAN_STATE_RUN,
    DEC_CHAN_STATE_CANCEL,
    DEC_CHAN_STATE_MAX
} dec_chan_state;

typedef enum {
    DEC_TASK_OPS_NORMAL = 0,
    DEC_TASK_OPS_REPAIR,
    DEC_TASK_OPS_DISCARD,
    DEC_TASK_OPS_MAX
} dec_task_ops;

typedef enum {
    BLOCK_BY_DPS = 0, /* wakup by SYN */
    BLOCK_BY_FSP,     /* wakup by FSP, or BSP? */
    BLOCK_BY_MSG,     /* wakup by BSP, or CancelDecode? */
    BLOCK_BY_REG,     /* wakup by BSP */
    BLOCK_BY_MAX
} dec_task_block;

typedef struct {
    vfmw_vid_std vid_std;
    vfmw_cmp vcmp_mode;
    vfmw_bit_depth_attr disp_bit_depth_mode;
    td_u8 priority;
    td_u8 low_delay_en;
    td_u8 repair_en;
    td_u8 uv_order_en;
    td_u8 repair_thr; /* if ErrRatio < RepairThr, then repair */
    td_u8 sec_flag;
    td_s8 uvmos_en;
    td_s8 out_err_thr;
    td_s8 ref_err_thr;
    td_s8 mask_mmu_irq;
    td_s32 tunl_line_num;
    td_u32 poc_threshold;
    td_u32 max_core_num;
    td_u32 need_film_grain;
    vfmw_frm_layout layout;
    vfmw_frm_alloc_type alloc_type;
    vfmw_air_dec_version air_version;
    td_s32 linear_no_gap_en;
    td_bool is_max_buf_size_en;
    td_u32 max_dec_width;
    td_u32 max_dec_height;
} dec_chan_cfg;

typedef struct {
    td_s32 is_sec;
    vfmw_adpt_type adpt_type;
} dec_create_info;

typedef struct {
    td_s64 frm_mem_fd;
    td_u64 frm_dma_buf;
    td_u32 is_disp_need;
    UADDR luma_phy_addr;
    td_u8 *luma_vir_addr;
    td_u32 image_stride;
    td_u32 y_stride;
    td_u32 uv_stride;
    td_u32 uv_offset;
    td_u32 bit_depth;

    td_u8 vdh_2d_en;
    td_u8 tile_format;
    td_u8 compress_en;
    td_u32 cmp_head_stride;
    td_u32 cmp_head_size;
    td_u32 nbit_image_stride;
    td_u32 nbit_y_stride;
    td_u32 nbit_y_offset;
    td_u32 nbit_uv_offset;
    td_u32 line_num_ofs;
    td_u32 air_y_offset;
    td_u32 air_uv_offset;

    td_u64 sec_info;
    td_u32 buf_size;
} dec_chan_disp_mem_param;

typedef struct {
    td_s64 frm_mem_fd;
    td_u64 frm_dma_buf;
    UADDR luma_phy_addr;
    td_u8 *luma_vir_addr;
    td_u32 image_stride;
    td_u32 y_stride;
    td_u32 uv_stride;
    td_u32 uv_offset;
    td_u32 bit_depth;

    td_s64 ext_att_handle;
    UADDR ext_att_phy_addr;
    td_u64 ext_att_vir_addr;
    td_u32 ext_att_buf_size;
    td_u32 ext_attr_offset;

    td_s64 ext_hfbc_handle;
    td_u64 ext_hfbc_vir_addr;

    td_u8 vdh_2d_en;
    td_u8 compress_en;
    td_u32 cmp_head_stride;
    td_u32 cmp_head_size;

    td_u32 nbit_image_stride;
    td_u32 nbit_y_stride;
    td_u32 nbit_y_offset;
    td_u32 nbit_uv_offset;
    td_u32 uv_offset_dis;
    td_u32 line_num_ofs;
    td_u32 tile_cdf_ofs;
    td_u32 seg_id_ofs;
    td_u32 av1_film_tab_offset;

    UADDR pmv_phy_addr;
    td_u32 half_pmvblk_offset;

    UADDR mtdt_phy_addr;
    td_u8 *mtdt_vir_addr;
    td_u64 meta_data_dma_buf;
    td_u32 mtdt_size;
    td_s64 mtdt_fd;
    td_u64 mtdt_fd_offset;

    td_u8 vdh_mmu_en;
    td_u8 mask_mmu_irq;
    td_u8 bsp_in_ram;
    td_u8 pxp_in_ram;
    td_u8 vdh_safe_flag;
    td_u8 low_delay_en;
    td_u8 uv_order_en;
    td_u8 tunnel_en;
    td_s32 tunl_line_num;
    dec_chan_disp_mem_param disp_mem;
    vfmw_mem_desc *chan_mem;
    td_u32 prev_pic_mb0_qp;
    td_u32 switch_rounding;
    td_u32 buf_size;
    td_s32 linear_no_gap_en;
} dec_chan_mem_param;

typedef struct {
    td_void *first_slice;
    td_void *list_head[VDH_BSP_NUM]; /* head node of SliceParam after splitting */
    td_u16 list_len_pre[VDH_BSP_NUM];
    td_u16 list_len[VDH_BSP_NUM];
    td_u16 list_num;
    td_u16 fake_slice; /* if insert a fake slice with first MB address 0 */

    td_u32 dn_msg_one_size;
    UADDR dn_msg_phy_addr[VDH_BSP_NUM];
    td_u8 *dn_msg_vir_addr[VDH_BSP_NUM];
    UADDR up_msg_phy_addr[VDH_BSP_NUM];
    td_u8 *up_msg_vir_addr[VDH_BSP_NUM];
} dec_chan_slice_list;

typedef struct {
    dps_info *dps;
    dec_task_state state;
    td_u16 dev_id;
    td_u16 msg_id; /* idx of msg to which the task is config */
    td_u16 reg_id; /* idx of reg to which the task is config */
    td_u16 cfg_id; /* unique config idc */
    td_u16 seg_rlsd_flag;
    td_u16 err_ratio;
    dec_chan_slice_list slice_list;
    dec_chan_mem_param mem_param;
    dec_repair_param rep_param[2]; /* 2 :a number */
} dec_chan_task;

typedef struct {
    dec_chan_task task[DEC_MAX_TASK_NUM]; /* Task of Chan */
    td_u16 cfg_task_num;                  /* task number of waiting for configing */
    td_u16 dec_task_num;                  /* task number of decoding */
    td_u16 next_cfg_task;                 /* next task index to config register */
    td_u16 next_bsp_task;                 /* next task index to return bsp interrupt */
    td_u16 next_pxp_task;                 /* next task index to return pxp interrupt */
} dec_chan_task_pool;

typedef struct {
    td_s32 chan_id;
    td_u32 unid;
    td_s32 is_sec;
    vfmw_adpt_type adpt_type;
    dec_chan_state state;
    dec_chan_cfg chan_cfg;
    dec_hal_ops *dec_ops; /* function pointer */
    td_u32 run_cnt;
    td_u32 dps_try_cnt;
    td_u32 dps_succ_cnt;
    td_u32 dps_fail_cnt;
    td_u32 mem_try_cnt;
    td_u32 mem_succ_cnt;
    td_u32 mem_fail_cnt;
    td_u32 rcv_dps_num;   /* total number of dps that have received */
    td_u32 dec_dps_num;   /* total number of dps that have processed */
    td_u32 err_frame_num; /* total number of frames that have decoded error */
    td_u32 block_time;
    vfmw_mem_desc chn_hal_mem[2]; /* 2 :a number */
    td_u32 prev_pic_mb0_qp;
    td_u32 switch_rounding;
    dec_chan_task_pool chan_task_pool; /* pool of channel task */
    td_u32 wait_dev;
} dec_chan;

typedef struct {
    td_s32 sec;
    td_s32 no_sec;
} dec_chan_type_num;

typedef struct {
    dec_chan *chan[DEC_MAX_CHAN_NUM]; /* handle of channel */
    td_u16 next_chan;
    td_u32 chan_num_no_sec; /* current number of no sec channel */
    td_u32 chan_num_sec;    /* current number of sec channel */
    td_u32 started_chan_num;
    td_s32 curr_high_prior_chan;
    td_s32 chan_pri_tab_index;
    td_s32 chan_pri_tab[DEC_MAX_CHAN_NUM][2]; /* chanId and Prio 2 :a number */
    os_lock spin_lock;
    os_lock chan_lock[DEC_MAX_CHAN_NUM];
} dec_chan_pool;

dec_chan *dec_chan_get_chan(td_s32 chan_id);
td_void dec_chan_get_chan_num(dec_chan_type_num *num);
td_void dec_chan_init(td_void);
td_void dec_chan_deinit(td_void);
td_s32 dec_chan_create(td_s32 chan_id, const td_void *args);
td_s32 dec_chan_destroy(td_s32 chan_id);
td_s32 dec_chan_config(td_s32 chan_id, const td_void *chan_cfg);
td_s32 dec_chan_start(td_s32 chan_id);
td_s32 dec_chan_stop(td_s32 chan_id);
td_s32 dec_chan_reset(td_s32 chan_id);
td_s32 dec_chan_init_all(td_void);
td_s32 dec_chan_reset_all(td_void);
td_s32 dec_chan_start_all(td_void);
td_s32 dec_chan_stop_all(td_void);
td_s32 dec_chan_destroy_all(td_void);
td_void dec_chan_routine(td_void);
td_void dec_chan_upd_scheduling(dec_chan *chan);
td_void dec_chan_lock_chan(td_s32 chan_id, td_ulong *lock_flag);
td_void dec_chan_unlock_chan(td_s32 chan_id, td_ulong *lock_flag);
td_s32 dec_chan_check_chan_valid(const dec_chan *chan);
td_s32 dec_chan_get_bsp_task(dec_chan *chan, dec_chan_task **task); /* get next task to return bsp */
td_s32 dec_chan_get_pxp_task(dec_chan *chan, dec_chan_task **task); /* get next task to return pxp */
td_s32 dec_chan_update_pxp_task(dec_chan *chan);
td_s32 dec_chan_release_task_seg(const dec_chan *chan, dec_chan_task *task); /* release Seg of task */
td_s32 dec_chan_release_task_dps(const dec_chan *chan, dec_chan_task *task); /* release DPS of task */
td_s32 dec_chan_make_dec_report(dec_chan *, dec_chan_task *);
td_s32 dec_chan_mark_display_state(dec_chan *chan, dec_chan_task *task, td_u16 state);
td_s32 dec_chan_get_info(td_s32 chan_id, const td_void *args);
td_void dec_chan_decrease_dec_task_num(dec_chan *chan);
td_void dec_chan_increase_dec_dps_num(dec_chan *chan);
td_u32 dec_chan_scdeduling(dec_chan **chan, dec_chan_task **task);
td_void dec_chan_cancel_task(dec_chan *chan, dec_chan_task *task, td_u32 perm_flag); /* cancel decoding of task */
td_s32 dec_chan_clear_ref_info(const dec_chan *chan, const dec_chan_task *task);
td_void dec_chan_adjust_ref_info(dec_chan *chan, const dec_chan_task *curr_task);
td_void dec_chan_try_open_dev(td_void);
td_void dec_chan_try_close_dev(td_void);
td_void dec_chan_try_refresh_bin(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
