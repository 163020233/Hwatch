/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_HAL_H__
#define __DEC_HAL_H__

#include "dec_base.h"
#include "vfmw_sys.h"

#define DEC_DEV_NUM            (VDH_NUM + MDMA_NUM)
#define DEC_DEV_START_IDX_VDH  0
#define DEC_DEV_START_IDX_MDMA VDH_NUM

#define DEC_BASE_UPMSG_SLOT_LEN  (4 * 4)
#define DEC_DEV_VDH_UP_MSG_ALIGN 256

#define VDH_IN_RAM_MAX_WIDTH   1280
#define VDH_ADDR_ALIGN_16 16

#define OLD_SED_TOP_ADDR_LEN  (4 * HEVC_MAX_PIX_WIDTH)
#define OLD_PMV_TOP_ADDR_LEN  (8 * HEVC_MAX_PIX_WIDTH)
#define OLD_RCN_TOP_ADDR_LEN  (4 * HEVC_MAX_PIX_WIDTH)
#define OLD_DBLK_TOP_ADDR_LEN (16 * HEVC_MAX_PIX_WIDTH)

#define VDH_PUB_MSG_SLOT_LEN   (64 * 4 * 1)
#define VDH_CORE_MSG_SLOT_LEN  (64 * 4 * 7)
#define VDH_PIC_MSG_SLOT_LEN   (64 * 4 * 6)
#define VDH_SLICE_MSG_SLOT_LEN (64 * 4 * 4)

#define SLICE_BIN_LEN (512 * 1024)
#define CTB_HEAD_LEN  (256 * 1024)

#define FRM_UP_MSG_LEN 128

#define WORK_NORMAL  0
#define WORK_SEDONLY 3

#define arrange_ptr(mem, len, ptr) \
    do {                              \
        (ptr) = (mem);             \
        (mem) += (len);            \
    } while (0)
#define arrange_add(mem, len) \
    do {                         \
        (mem) += (len);       \
    } while (0)

#define hw_addr_shift(addr) ((addr) >> (4))

#define rd_vreg(base, reg, dat)                            \
    do {                                                   \
        (dat) = *((volatile td_u32 *)((td_u8 *)(base) + (reg))); \
    } while (0)
#define RD_VREG rd_vreg

#define wr_vreg(base, reg, dat)                            \
    do {                                                   \
        *((volatile td_u32 *)((td_u8 *)(base) + (reg))) = (dat); \
    } while (0)
#define WR_VREG wr_vreg

#define rd_msgword(vir_addr, dat)                          \
    do {                                                   \
        (dat) = *((volatile td_u32 *)((td_u8 *)(vir_addr))); \
    } while (0)
#define RD_MSGWORD rd_msgword

#define wr_msgword(vir_addr, dat)                          \
    do {                                                   \
        *((volatile td_u32 *)((td_u8 *)(vir_addr))) = (dat); \
    } while (0)
#define WR_MSGWORD wr_msgword

typedef enum {
    DEC_HAL_EXT_MEM_STATIC,
    DEC_HAL_EXT_MEM_DYNAMIC,
    DEC_HAL_EXT_MEM_MAX,
} dec_hal_ext_mem_type;

typedef struct {
    td_u32 dev_id;
    td_u64 hal_fd;
    UADDR base_phy_addr;
    td_u8 *base_vir_addr;
    td_u32 base_mem_len;

    td_bool is_dyn_ram_alloc_ok;
    UADDR dyn_ram_phy_addr;
    td_u8 *dyn_ram_vir_addr;
    td_u32 dyn_ram_mem_len;

    UADDR h264_mn_phy_addr;
    UADDR h265_mn_phy_addr;

    /*  old  */
    UADDR old_sed_top_addr;
    UADDR old_pmv_top_addr;
    UADDR old_rcn_top_addr;
    UADDR old_dblk_top_addr;

    /*  h264  */
    UADDR h264_sed_top_addr;
    UADDR h264_pmv_top_addr;
    UADDR h264_rcn_top_addr;
    UADDR h264_dblk_top_addr;

    /*  h265  */
    UADDR h265_sed_top_addr;
    UADDR h265_pmv_top_addr;
    UADDR h265_pmv_left_addr;
    UADDR h265_rcn_top_addr;
    UADDR h265_dblk_top_addr;
    UADDR h265_dblk_left_addr;
    UADDR h265_sao_top_addr;
    UADDR h265_sao_left_addr;
    UADDR h265_cmp_left_addr;

    /*  vp9  */
    UADDR vp9_sed_top_addr;
    UADDR vp9_pmv_top_addr;
    UADDR vp9_rcn_top_addr;
    UADDR vp9_dblk_top_addr;
    UADDR vp9_dblk_left_addr;
    UADDR vp9_cmp_left_addr;
    UADDR vp9_bsp_sed_top_addr;
    UADDR vp9_bsp_pmv_top_addr;

    /*  avs2  */
    UADDR avs2_sed_top_addr;
    UADDR avs2_pmv_top_addr;
    UADDR avs2_rcn_top_addr;
    UADDR avs2_dblk_top_addr;
    UADDR avs2_dblk_left_addr;
    UADDR avs2_sao_top_addr;
    UADDR avs2_bsp_sed_top_addr;
} vdh_ext_mem_info;

typedef struct {
    td_u8 used; /* default is 0, set to 1 after configing, set to 0 after pxp int, set to 0 if canceling decoding and
                    releasing, hold to 1 if canceling decoding and not releaseing */
    td_u16 reg_id;
    td_u32 chan_id;
    td_u64 dma_buf;
    UADDR base_phy_addr;
    td_u8 *base_vir_addr;
    td_u32 base_mem_len;

    UADDR pub_msg_phy_addr;
    td_u8 *pub_msg_vir_addr;
    UADDR core_msg_phy_addr;
    td_u8 *core_msg_vir_addr;
    UADDR pic_msg_phy_addr;
    td_u8 *pic_msg_vir_addr;
    UADDR slice_msg_phy_addr;
    td_u8 *slice_msg_vir_addr;
    td_u32 slice_msg_all_size;
    td_u32 slice_msg_one_size;
    UADDR up_msg_phy_addr;
    td_u8 *up_msg_vir_addr;
    td_u32 slice_up_msg_all_size;
    UADDR tile_info_phy_addr;
    td_u8 *tile_info_vir_addr;
    UADDR vc1_tab_phy_addr;
    UADDR slice_bin_phy_addr;
    td_u32 slice_bin_length;
    UADDR ctb_head_phy_addr;
    td_u32 ctb_head_length;
    UADDR real_tab_phy_addr;
} vdh_msg_info;

typedef struct tag_dec_hal_ops {
    td_s32 (*write_msg)(const td_void *msg, td_void *task, const td_void *vdh);
    td_s32 (*process_slice)(td_void *pic_param, td_void *slice_head, td_u16 slice_num, td_void *slice_list);
    td_s32 (*make_slice_ref)(td_u32 chan_id, td_void *pic_param, td_void *slice_param);
    td_s32 (*get_rep_param)(td_void *repair, const td_void *task, td_void *report);
    td_s32 (*write_mdma_reg)(td_u8 *, UADDR, td_void *);
    td_s32 (*write_mdma_msg)(td_u8 *slot_vir_addr, td_void *pic_param, td_void *task_mem, td_void *mdma_msg);
    td_s32 (*fill_ext_mem)(td_void *vdh_mem, UADDR base_addr, dec_hal_ext_mem_type type);
    td_s32 (*alloc_chan_mem)(td_s32 chan_id, vfmw_mem_desc *mem, td_u32 len);
    td_s32 (*get_chan_mem_size)(td_void);
    vfmw_vid_std vid_std;
} dec_hal_ops;

td_s32 dec_hal_open_vdh(td_void *dec_dev);
td_s32 dec_hal_close_vdh(td_void *dec_dev);
td_s32 dec_hal_reset_vdh(const td_void *dec_dev);
td_s32 dec_hal_resume_vdh(const td_void *dev);
td_s32 dec_hal_suspend_vdh(const td_void *dev);
td_s32 dec_hal_init_irq_vdh(const td_void *dev);
td_s32 dec_hal_exit_irq_vdh(const td_void *dev);
td_void dec_hal_write_pub_msg(const td_void *vdh_msg, const td_void *dec_task, const td_void *dev_vdh);
td_void dec_hal_write_core_msg(const vdh_msg_info *msg, const td_void *task, const td_void *vdh);
td_s32 dec_hal_config_msg_vdh(const td_void *dec_dev, const dec_hal_ops *ops, const td_void *dec_task);
td_s32 dec_hal_config_reg_vdh(const td_void *dec_dev, const dec_hal_ops *ops, const td_void *dec_task);
td_s32 dec_hal_start_dec(const td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_cancel_dec(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_calc_mem_param(td_void *dec_dev, dec_hal_ops *ops, td_void *chan_cfg, td_void *dec_task);
td_s32 dec_hal_read_int_info(td_void *dec_dev, td_s32 reg_id);
td_u16 dec_hal_read_bsp_indicator(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_clear_bsp_int_state(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_clear_int_state(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_read_frm_msg(td_void *dec_dev, td_void *chan_arg, td_void *dec_task);
td_s32 dec_hal_read_slc_msg(td_void *dec_dev, td_u16 reg_id, td_void *list);
td_s32 dec_hal_read_dec_reg(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_get_ops(vfmw_vid_std vid_std_info, dec_hal_ops **ops);
td_s32 dec_hal_get_mem_need_size(const td_void *dev, vfmw_svr_mem_need *mem);
td_s32 mdma_hal_get_mem_need_size(const td_void *dev, vfmw_svr_mem_need *mem);
td_s32 dec_hal_get_dev(td_s32 is_sec, td_void *chan_task, td_u16 *dev_id);
td_s32 dec_hal_control(td_s32 dev_id, td_s32 cid, td_void *arg1, td_void *arg2);

#endif
