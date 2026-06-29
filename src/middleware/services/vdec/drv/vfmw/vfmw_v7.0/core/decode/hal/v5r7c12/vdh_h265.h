/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VDH_H265_H__
#define __VDH_H265_H__

#include "dec_base.h"
#include "dec_reg.h"
#include "msg_h265.h"
#include "dec_hal.h"

#define HEVC_CA_MN_ADDR_LEN     1024
#define HEVC_SED_TOP_ADDR_LEN   (2 * HEVC_MAX_PIX_WIDTH)
#define HEVC_PMV_TOP_ADDR_LEN   (4 * HEVC_MAX_PIX_WIDTH)        // ctb16, ctb32 is 3
#define HEVC_PMV_LEFT_ADDR_LEN  (3 * HEVC_MAX_PIX_HEIGHT)
#define HEVC_RCN_TOP_ADDR_LEN   (2 * HEVC_MAX_PIX_WIDTH)
#define HEVC_DBLK_TOP_ADDR_LEN  (10 * HEVC_MAX_PIX_WIDTH + 256)
#define HEVC_DBLK_LEFT_ADDR_LEN (12 * HEVC_MAX_PIX_HEIGHT)
#define HEVC_SAO_TOP_ADDR_LEN   (4 * HEVC_MAX_PIX_WIDTH + 256)
#define HEVC_SAO_LEFT_ADDR_LEN  (22 * HEVC_MAX_PIX_HEIGHT)
#define HEVC_CMP_LEFT_ADDR_LEN  (80 * 3 * HEVC_MAX_PIX_HEIGHT >> 1)

#define HEVC_SED_TOP_ADDR_LEN_INRAM   (VDH_IN_RAM_MAX_WIDTH * 2)
#define HEVC_PMV_TOP_ADDR_LEN_INRAM   (VDH_IN_RAM_MAX_WIDTH * 3)
#define HEVC_RCN_TOP_ADDR_LEN_INRAM   (VDH_IN_RAM_MAX_WIDTH * 2)
#define HEVC_DBLK_TOP_ADDR_LEN_INRAM  (VDH_IN_RAM_MAX_WIDTH * 9 + 256)
#define HEVC_SAO_TOP_ADDR_LEN_INRAM   (VDH_IN_RAM_MAX_WIDTH * 3 + 256)

#define HEVC_SED_TOP_INTER_OFFSET  0
#define HEVC_PMV_TOP_INTER_OFFSET  (HEVC_SED_TOP_INTER_OFFSET + HEVC_SED_TOP_ADDR_LEN_INRAM)
#define HEVC_RCN_TOP_INTER_OFFSET  (HEVC_PMV_TOP_INTER_OFFSET + HEVC_PMV_TOP_ADDR_LEN_INRAM)
#define HEVC_DBLK_TOP_INTER_OFFSET (HEVC_RCN_TOP_INTER_OFFSET + HEVC_RCN_TOP_ADDR_LEN_INRAM)
#define HEVC_SAO_TOP_INTER_OFFSET  (HEVC_DBLK_TOP_INTER_OFFSET + HEVC_DBLK_TOP_ADDR_LEN_INRAM)

#define HEVC_TILE_INFO_LEN vfmw_align_up((HEVC_MAX_CTB_NUM_PER_LINE * 2 / 4 +              \
                                             HEVC_MAX_TILE_COLUMNS + HEVC_MAX_TILE_ROWS) * \
                                             4,                                            \
    16)

#define HEVC_PUB_MSG_SLOT_LEN   (64 * 4 * 1)
#define HEVC_CORE_MSG_SLOT_LEN  (64 * 4 * 7)
#define HEVC_PIC_MSG_SLOT_LEN   (64 * 4 * 6)
#define HEVC_SLICE_MSG_SLOT_LEN (64 * 4 * 4)

#define HEVC_TILE_IDX_SHIFT 0
#define HEVC_MAX_TILE_NUM (HEVC_LOGIC_TILE_ROWS_LIMIT * HEVC_LOGIC_TILE_COLUMNS_LIMIT)
#define HEVC_MAX_MB_NUM   (HEVC_MAX_PIX_WIDTH / HEVC_MB_SIZE * \
                         HEVC_MAX_PIX_HEIGHT / HEVC_MB_SIZE)

typedef struct {
    td_u32 tile_num;
    td_u8 is_decode[HEVC_MAX_MB_NUM];
    td_u32 ts_to_rs_map[HEVC_MAX_MB_NUM];
    td_u32 tile_group[HEVC_MAX_TILE_NUM][2]; /* 2 is length */
} h265_tile;

td_s32 hal_h265_fill_ext_mem(td_void *vdh_mem, UADDR base_addr, dec_hal_ext_mem_type type);
td_s32 hal_h265_write_msg(const td_void *msg, td_void *task, const td_void *vdh);
td_s32 hal_h265_process_slice(td_void *pic_param, td_void *slice_head, td_u16 slice_num, td_void *slice_list);
td_s32 hal_h265_get_rep_param(td_void *repair, const td_void *task, td_void *report);
td_s32 hal_h265_make_slice_ref(td_u32 chan_id, td_void *pic_param, td_void *slice_param);

#endif
