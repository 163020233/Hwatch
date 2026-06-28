/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VDH_H264_H__
#define __VDH_H264_H__

#include "dec_base.h"
#include "dec_reg.h"
#include "msg_h264.h"
#include "dec_post.h"
#include "dec_hal.h"

#define H264_CA_MN_ADDR_LEN    (64 * 4 * 20)
#define H264_SED_TOP_ADDR_LEN  (H264_MAX_PIX_WIDTH * 4)
#define H264_PMV_TOP_ADDR_LEN  (H264_MAX_PIX_WIDTH * 8)
#define H264_RCN_TOP_ADDR_LEN  (H264_MAX_PIX_WIDTH * 4)
#define H264_DBLK_TOP_ADDR_LEN (H264_MAX_PIX_WIDTH * 16)

#define H264_SED_TOP_ADDR_LEN_INRAM   (VDH_IN_RAM_MAX_WIDTH * 2)
#define H264_PMV_TOP_ADDR_LEN_INRAM   (VDH_IN_RAM_MAX_WIDTH * 4)
#define H264_RCN_TOP_ADDR_LEN_INRAM   (VDH_IN_RAM_MAX_WIDTH * 2)
#define H264_DBLK_TOP_ADDR_LEN_INRAM  (VDH_IN_RAM_MAX_WIDTH * 8)

#define H264_SED_TOP_INTER_OFFSET  0
#define H264_PMV_TOP_INTER_OFFSET  (H264_SED_TOP_INTER_OFFSET + H264_SED_TOP_ADDR_LEN_INRAM)
#define H264_RCN_TOP_INTER_OFFSET  (H264_PMV_TOP_INTER_OFFSET + H264_PMV_TOP_ADDR_LEN_INRAM)
#define H264_DBLK_TOP_INTER_OFFSET (H264_RCN_TOP_INTER_OFFSET + H264_RCN_TOP_ADDR_LEN_INRAM)

#define H264_PUB_MSG_SLOT_LEN   (64 * 4 * 1)
#define H264_CORE_MSG_SLOT_LEN  (64 * 4 * 1)
#define H264_PIC_MSG_SLOT_LEN   (64 * 4 * 2)
#define H264_SLICE_MSG_SLOT_LEN (64 * 4 * 4)
#define H264_UP_MSG_SLOT_LEN    (4 * 4)
#define H264_MV_SIZE_PER_MB     (16 * 4)

typedef struct {
    td_u32 slice_index;
    td_u32 too_many_slice;
    td_u32 slice_mb_range[MAX_UP_MSG_SLICE_NUM][2]; /* 2 is length */
} h264_post;

td_s32 hal_h264_fill_ext_mem(td_void *vdh_mem, UADDR base_addr, dec_hal_ext_mem_type type);
td_s32 hal_h264_write_msg(const td_void *msg, td_void *task, const td_void *vdh);
td_s32 hal_h264_process_slice(td_void *pic_param, td_void *slice_head, td_u16 slice_num, td_void *slice_list);
td_s32 hal_h264_get_rep_param(td_void *repair, const td_void *task, td_void *report);
td_s32 hal_h264_make_slice_ref(td_u32 chan_id, td_void *pic_param, td_void *slice_param);

#endif
