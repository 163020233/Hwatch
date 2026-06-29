/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2006-2024. All rights reserved.
 * Description: vdh h264
 * Author: AuthorNameMagicTag
 * Create: 2006-01-01
 */

#ifndef __HAL_VDH_H264_H__
#define __HAL_VDH_H264_H__

#include "dec_base.h"
#include "dec_reg.h"
#include "msg_h264.h"
#include "dec_post.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H264_CA_MN_ADDR_LEN           (64 * 4 * 20)
#define H264_SED_TOP_ADDR_LEN         (H264_MAX_PIX_WIDTH * 3)
#define H264_PMV_TOP_ADDR_LEN         (H264_MAX_PIX_WIDTH * 4)
#define H264_RCN_TOP_ADDR_LEN         (H264_MAX_PIX_WIDTH * 4)
#define H264_DBLK_TOP_ADDR_LEN        (H264_MAX_PIX_WIDTH * 16)

#define H264_PIC_MSG_SLOT_LEN         (64 * 4 * 2)
#define H264_SLICE_MSG_SLOT_LEN       (64 * 4 * 4)
#define H264_UP_MSG_SLOT_LEN          (4 * 4)

typedef struct {
    td_u32 slice_index;
    td_u32 too_many_slice;
    td_u32 slice_mb_range[MAX_UP_MSG_SLICE_NUM][2]; /* 2 is length */
} h264_post;

td_s32 hal_h264_init_hal(td_void *vdh_mem);
td_s32 hal_h264_write_reg(td_u8 *reg_vir_addr, td_void *h264_pic_param, td_void *h264_task_mem, UADDR dn_msg_phyaddr,
    UADDR up_msg_phy_addr);
td_s32 hal_h264_write_pic_msg(td_u8 *slot_vir_addr, td_void *h264_pic_param, td_void *h264_task_mem,
    td_void *h264_vdh_mem, td_void *h264_vdh_msg);
td_s32 hal_h264_write_slice_msg(td_u8 *slot_vir_addr, td_void *h264_pic_param, td_void *task_mem,
    dec_slice_param_h264 *h264_slice_param, UADDR next_slice_phy_addr);
td_s32 hal_h264_write_slice_lst(td_void *h264_pic_param, td_void *task_mem, td_void *h264_slice_list,
    td_u8 **slot_vir_addr, UADDR *slot_phyaddr);
td_s32 hal_h264_process_slice(td_void *h264_pic_param, td_void *h264_slice_head, td_u16 slice_num,
    td_void *slice_list);
td_s32 hal_h264_calc_mem_param(td_void *h264_pic_param, td_void *cfg, td_void *h264_task_mem);
td_s32 hal_h264_make_slice_ref(td_s32 chan_id, td_void *h264_pic_param, td_void *h264_slice_param);
td_s32 hal_h264_get_rep_param(td_void *repair, const td_void *chan_task, td_void *report);

#ifdef __cplusplus
}
#endif

#endif
