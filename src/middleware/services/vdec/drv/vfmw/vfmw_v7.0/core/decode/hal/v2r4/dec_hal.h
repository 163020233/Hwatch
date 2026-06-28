/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2006-2024. All rights reserved.
 * Description: dec hal
 * Author: AuthorNameMagicTag
 * Create: 2006-01-01
 */

#ifndef __DEC_HAL_H__
#define __DEC_HAL_H__

#include "dec_base.h"
#include "vfmw_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEC_DEV_NUM                (VDH_NUM + MDMA_NUM)
#define DEC_DEV_START_IDX_VDH      0
#define DEC_DEV_START_IDX_MDMA     VDH_NUM

#define DEC_DEV_VDH_UP_MSG_ALIGN   (256)

#define MDMA_MAX_MBG_NUM           (300)
#define MDMA_PIC_MSG_SLOT_LEN      (12 * 4)
#define MDMA_MBG_MSG_SLOT_LEN      (2 * 4 * MDMA_MAX_MBG_NUM)

#define ARRANGE_PTR(mem, len, ptr) {(ptr) = (mem); (mem) += (len);}
#define ARRANGE_ADD(mem, len)      {(mem) += (len);}

#define RD_VREG(base, reg, dat) \
    do { \
        dat = *((volatile td_u32*)((td_u8*)(base) + (reg))); \
    } while (0)

#define WR_VREG(base, reg, dat) \
    do { \
        *((volatile td_u32*)((td_u8*)(base) + (reg))) = dat; \
    } while (0)

#define RD_MSGWORD(vir_addr, dat) \
    do { \
        dat = *((volatile td_u32*)((td_u8*)(vir_addr))); \
    } while (0)

#define WR_MSGWORD(vir_addr, dat) \
    do { \
        *((volatile td_u32*)((td_u8*)(vir_addr))) = dat; \
    } while (0)


typedef struct {
    UADDR   base_phy_addr;
    td_u8  *base_vir_addr;
    UADDR   mn_phy_addr;
    td_u8  *mn_vir_addr;
    UADDR   h264_mn_phy_addr;
    td_u8  *h264_mn_vir_addr;
    UADDR   sed_top_phy_addr;
    UADDR   pmv_top_phy_addr;
    UADDR   pmv_left_phy_addr;
    UADDR   rcn_top_phy_addr;
    UADDR   dblk_top_phy_addr;
    UADDR   dblk_left_phy_addr;
    UADDR   sao_top_phy_addr;    // if not need, do not alloc memory
    UADDR   sao_left_phy_addr;
} vdh_ext_mem_info;

typedef struct {
    /* default is 0, set to 1 after configing, set to 0 after ilp int,
       set to 0 if canceling decoding and releasing, hold to 1 if canceling decoding and not releaseing */
    td_u8    used;
    td_u16   reg_id;
    td_s32   chan_id;
    UADDR    base_phy_addr;
    td_u8   *base_vir_addr;
    UADDR    pic_msg_phy_addr;
    td_u8   *pic_msg_vir_addr;
    UADDR   *slice_msg_phy_addr;
    td_u8  **slice_msg_vir_addr;
    UADDR    up_msg_phy_addr;
    td_u8   *up_msg_vir_addr;
    UADDR    tile_info_phy_addr;
    td_u8   *tile_info_vir_addr;
} vdh_msg_info;

typedef struct tag_dec_hal_ops {
    td_s32 (*write_vdh_reg)(td_u8 *reg_vir_addr, td_void *pic_param, td_void *task_mem,
        UADDR dn_msg_phy_addr, UADDR up_msg_phy_addr);
    td_s32 (*write_vdh_pic_msg)(td_u8 *slot_vir_addr, td_void *pic_param, td_void *task_mem,
        td_void *vdh_mem, td_void *vdh_msg);
    td_s32 (*write_vdh_slice_lst)(td_void *pic_param, td_void *task_mem, td_void *slice_list,
        td_u8 **slot_vir_addr, UADDR *slot_phy_addr);
    td_s32 (*process_slice)(td_void *pic_param, td_void *slice_head, td_u16 slice_num, td_void *slice_list);
    td_s32 (*calc_mem_param)(td_void *pic_param, td_void *Cfg, td_void *task_mem);
    td_s32 (*make_slice_ref)(td_s32 chan_id, td_void *pic_param, td_void *slice_param);
    td_s32 (*get_rep_param)(td_void *repair, const td_void *task, td_void *report);
    td_s32 (*write_mdma_reg)(td_u8 *, UADDR, td_void *);
    td_s32 (*write_mdma_msg)(td_u8 *slot_vir_addr, td_void *pic_param, td_void *task_mem, td_void *mdma_msg);
    td_s32 (*alloc_chan_mem)(td_s32 chan_id, vfmw_mem_desc *mem, td_u32 len);
    vfmw_vid_std vid_std;
} dec_hal_ops;


td_s32 dec_hal_open_vdh(td_void *dec_dev);
td_s32 dec_hal_close_vdh(td_void *dec_dev);
td_s32 dec_hal_reset_vdh(const td_void *dec_dev);
td_s32 dec_hal_resume_vdh(const td_void *dec_dev);
td_s32 dec_hal_suspend_vdh(const td_void *dec_dev);
td_s32 dec_hal_init_irq_vdh(const td_void *dec_dev);
td_s32 dec_hal_exit_irq_vdh(const td_void *dec_dev);
td_s32 dec_hal_config_msg_vdh(const td_void *dec_dev, const dec_hal_ops *ops, const td_void *dec_task);
td_s32 dec_hal_config_reg_vdh(const td_void *dec_dev, const dec_hal_ops *ops, const td_void *dec_task);
td_s32 dec_hal_start_dec(const td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_cancel_dec(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_calc_mem_param(td_void *dec_dev, const dec_hal_ops *ops, td_void *chan_cfg, td_void *chan_task);

td_s32 dec_hal_read_int_info(td_void *dec_dev, td_s32 reg_id);
td_u16 dec_hal_read_olp_indicator(td_void *dec_dev, td_u16 reg_id);
td_u16 dec_hal_read_ilp_indicator(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_disable_int_state_dec(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_enable_int_state_dec(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_query_int_state_dec(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_clear_olp_int_state(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_clear_int_state(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_read_frm_msg(td_void *dec_dev, td_void *chan, td_void *task);
td_s32 dec_hal_read_slc_msg(td_void *dec_dev, td_u16 reg_id, td_void *slice_list);
td_s32 dec_hal_read_dec_reg(td_void *dec_dev, td_u16 reg_id);
td_s32 dec_hal_check_indicator(td_u16 idc, td_u16 cfg_id);

td_s32 dec_hal_open_mdma(td_void *dec_dev);
td_s32 dec_hal_close_mdma(td_void *dec_dev);
td_s32 dechal_reset_mdma(td_void *dec_dev);
td_s32 dec_hal_init_irq_mdma(td_void *dec_dev);
td_s32 dec_hal_exit_irq_mdma(td_void *dec_dev);
td_s32 dec_hal_config_msg_mdma(td_void *dec_dev, dec_hal_ops *ops, td_void *dec_task);
td_s32 dec_hal_config_reg_mdma(td_void *dec_dev, dec_hal_ops *ops, td_void *dec_task);
td_s32 dec_hal_start_rep(td_void *dec_dev, td_u16 reg_id);

td_s32 dec_hal_get_ops(vfmw_vid_std vid_std, dec_hal_ops **ops);
td_u32 dec_hal_get_init_mem_size_vdh(td_void *dec_dev);
td_s32 dec_hal_get_mem_need_size(const td_void *dec_dev, vfmw_svr_mem_need *mem);
td_s32 mdma_hal_get_init_mem_size(td_void *dec_dev);
td_s32 dec_hal_get_dev(td_s32 is_sec, td_void *chan_task, td_u16 *dev_id);
td_s32 dec_hal_control(td_u16 dev_id, td_s32 cid, const td_void *arg1, const td_void *arg2);

#ifdef __cplusplus
}
#endif

#endif
