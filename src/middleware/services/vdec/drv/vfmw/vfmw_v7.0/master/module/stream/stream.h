/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __STREAM_H__
#define __STREAM_H__

#include "vfmw.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define STREAM_EOS 2

typedef enum {
    STREAM_STATUS_WRITE = 0,
    STREAM_STATUS_FREE,
    STREAM_STATUS_READ,
} stream_buf_status;

typedef enum {
    STREAM_BUF_ALLOC_OUTER = 0,
    STREAM_BUF_ALLOC_INNER,
} stream_alloc_mode;

typedef struct {
    vfmw_stream_buf buf;
    stream_buf_status status;
    struct os_list_head node;
    td_bool is_used;
} stream_block;

typedef struct {
    td_bool is_used;
    td_s32 chan_id;
    vfmw_adpt_type adpt_type;
    stream_alloc_mode mode;
    td_u64 mem_dma_buf;
    td_s64 mem_fd;
    UADDR start_phy_addr;
    td_u64 start_kern_vir_addr;
    td_u64 start_usr_vir_addr;
    td_u32 length;
    td_u32 write_offst;
    td_u32 read_offst;
    td_u32 try_read_cnt;
    td_u32 read_ok_cnt;
    td_u32 try_release_cnt;
    td_u32 release_ok_cnt;
    td_u32 try_dequeue_cnt;
    td_u32 dequeue_ok_cnt;
    td_u32 try_queue_cnt;
    td_u32 queue_ok_cnt;
    td_u32 used_len;
    td_u32 free_len;
    td_u32 freeze_size;
    td_u32 raw_num;
    td_u32 valid_pts_flag;
    td_u32 que_eos_cnt;
    td_bool is_sec;
    td_bool eos_flag;
    td_handle vdec_handle;
    td_handle ssm_handle;
    td_bool en_idr_pts;
    td_s64 idr_pts;
    stream_block **block;
    td_void *blk_mem[0x10];
    struct os_list_head list;
    td_u64 evt_map;
    td_u32 max_raw_num;
    td_u32 blk_next_idx;
    vfmw_vid_std vid_std;
    td_bool frm_mode;
    os_file *file;
    os_sema sema;
} stream_chan;

stream_chan *stream_get_chan(td_s32 chan_id);

td_void stream_open1(td_void);
td_void stream_close(td_void);
td_void stream_init(const vfmw_init_param *param);
td_void stream_exit(td_void);
td_s32  stream_create_chan(td_s32 chan_id, const vfmw_chan_option *option);
td_void stream_destroy_chan(td_s32 chan_id);
td_s32  stream_alloc_buffer(td_s32 chan_id, td_u32 *size);
td_s32  stream_free_buffer(td_s32 chan_id);
td_s32  stream_bind_usr_addr(td_s32 chan_id, td_u64 usr_addr);
td_s32  stream_queue_buffer(td_s32 chan_id, vfmw_stream_buf *raw_buf);
td_s32  stream_dequeue_buffer(td_s32 chan_id, vfmw_stream_buf *raw_buf);
td_s32  stream_read_buffer(td_s32 chan_id, vfmw_stream_buf *buffer);
td_s32  stream_release_buffer(td_s32 chan_id, vfmw_stream_buf *buffer);
td_s32  stream_send_eos(td_s32 chan_id);
td_s32  stream_reset(td_s32 chan_id);
td_s32  stream_get_es_info(td_s32 chan_id, vfmw_stream_buf *raw_buf);
td_s32  stream_attach_es_buf(td_s32 chan_id, vfmw_stream_buf *raw_buf);
td_s32  stream_set_idr_pts(td_s32 chan_id, td_s64 idr_pts);
td_s32  stream_get_chan_status(td_s32 chan_id, vfmw_chan_info *status);
td_s32  stream_set_evt_map(td_s32 chan_id, td_u64 evt_map);
td_s32  stream_config(td_s32 chan_id, vfmw_chan_cfg *cfg);
td_void stream_lock_chan(td_s32 chan_id);
td_void stream_unlock_chan(td_s32 chan_id);
td_s32  stream_set_callback(vfmw_adpt_type type, fun_vfmw_event_report fp_event_report);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
