/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __SYN_PARSE_H__
#define __SYN_PARSE_H__

#include "syn_base.h"
#include "stm_intf.h"
#include "stm_seg.h"
#include "syn_ops.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    SYNCTX_CMD_CFG_FFFB_SPEED = 0,
    SYNCTX_CMD_CFG_PVR_INFO,
} synctx_cmd;

typedef struct {
    td_s32 chan_id;
    td_s32 vid_std;
    syn_ops *ops;
    td_void *syn_ctx;
    td_u32 alloc_mem_size;
} syn_parse_ctx;

typedef struct {
    stm_seg_info stream_pack[2]; /* 2 :a number */
} dec_stream_packet;

typedef struct {
    td_u8 found_pic_flag;
    td_u8 is_current_seg_wait_dec;
    td_u8 convolution_packet_num;
    stm_seg_info *current_seg;
    dec_stream_packet *pack;
    dec_stream_packet *last_pack;
    td_u8 scd_wrap_around;
} stream_seg_packet;

td_s32 syn_parse_seg_man(td_s32 chan_id, const syn_cfg_info *syn_extra_data, stream_seg_packet *seg_man);
td_void *syn_parse_get_synctx(td_s32 chan_id);
syn_parse_state syn_parse_decode_stream(stm_seg_info *seg, dps_info *dps, td_s32 chan_id);
td_void syn_parse_init_ctx(td_s32 chan_id, const td_void *cfg);
td_void syn_parse_deinit_ctx(td_s32 chan_id);
td_void syn_parse_reset_ctx(td_s32 chan_id, td_void *args);
td_s32 syn_parse_open(td_void);
td_s32 syn_parse_close(td_void);
td_s32 syn_parse_get_mem_need_size(vfmw_svr_mem_need *mem);
td_void syn_parse_read_std_log(td_s32 chan_id, td_void *buf, td_s32 len, td_s32 *used);
td_s32 syn_parse_get_info(td_s32 chan_id, td_void *args);
td_s32 syn_parse_cfg_fffb_speed(td_s32 chan_id, const td_void *args);
td_s32 syn_parse_cfg_pvr_info(td_s32 chan_id, const td_void *args);
td_void syn_parse_get_ctx(syn_parse_ctx **ctx, td_s32 chan_id);
td_s32 judge_framerate_valid(td_u32 frame_rate);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SYN_PARSE_H__ */
