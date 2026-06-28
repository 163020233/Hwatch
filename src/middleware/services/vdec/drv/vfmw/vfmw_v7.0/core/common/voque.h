/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VOQUE_H__
#define __VOQUE_H__

#include "vfmw.h"
#include "vfmw_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VOQ_OK  0
#define VOQ_ERR (-1)

#ifdef VFMW_CHAN_NUM
#define VOQ_CHAN_NUM VFMW_CHAN_NUM
#else
#define VOQ_CHAN_NUM 1
#endif

#define VOQ_IMAGE_NUM (VFMW_FSP_MAX_FRAME_NUM + 1)

typedef enum {
    VOQ_STATE_NULL = 0,
    VOQ_STATE_OPEN,
    VOQ_STATE_INIT,
    VOQ_STATE_EXIT,
    VOQ_STATE_BUTT
} voq_state;

typedef enum {
    VOQ_EOS_STATE_NULL = 0,
    VOQ_EOS_STATE_WAIT,
    VOQ_EOS_STATE_OUT,
    VOQ_EOS_STATE_BUTT
} voq_eos_state;

typedef enum {
    VOQ_EOS_TYPE_NULL = 0,
    VOQ_EOS_TYPE_SUCC,
    VOQ_EOS_TYPE_FAIL,
    VOQ_EOS_TYPE_IMG_ID,
    VOQ_EOS_TYPE_BUTT
} voq_eos_type;

typedef struct {
    td_u32 old_aspect_ratio;
    td_u32 old_scan_type;
    td_u32 old_seq_cnt;
    td_u32 old_disp_size;
    td_u32 old_disp_center;
    td_u32 last_image_id_plus2;
    td_u32 seq_img_cnt;
    td_u32 seq_cnt;
    td_s64 prev_pts;
} voq_side;

typedef struct {
    td_s32 last_img_id_plus2;
    voq_eos_state state;
    voq_eos_type type;
} voq_eos;

typedef struct {
    td_bool frm_fence_enable;
} voq_cfg_info;

typedef struct {
    td_s32 id;
    td_s32 un_id;
    td_s32 state;
    td_void *handle;
    os_sema sema;
    voq_eos eos;

    td_u16 tail;
    td_u16 head;
    td_u16 history;
    td_u32 magic_word;
    voq_side side_info;
    vfmw_image *img[VOQ_IMAGE_NUM];

    td_u32 view_num;
    td_u32 frm_cnt;
    td_u32 rcv_try;
    td_u32 rcv_ok;
    td_u32 rls_try;
    td_u32 rls_ok;
    td_u32 frm_total;
    td_u32 last_time_ms;
    td_u32 frm_rate;
    td_u32 new_frm_cnt;
    td_s32 rep_id;
    td_u32 rpt_eos_cnt;
    td_s32 speed;
    voq_cfg_info cfg;
} voq_chan;

typedef struct {
    td_u32 reserve1;

    os_sema sema;
    os_lock lock;
    td_u32 ref_cnt;

    voq_state state;
    td_u32 ctx_size;
    voq_chan *chan[VOQ_CHAN_NUM];

    td_u32 reserve2;
} voq_entry;

typedef struct {
    td_s32 (*voq_last_frm)(td_s32);
    td_s32 (*voq_insert)(td_s32, vfmw_image *);
    td_s32 (*voq_delete)(td_s32, vfmw_image *);
} voq_ext_intf;

typedef struct {
    td_u32 recv_frm_num;
    td_u32 dec_frm_rate;
} voq_info;

td_s32 voq_open(td_void);
td_s32 voq_close(td_void);
td_s32 voq_init(td_void);
td_s32 voq_exit(td_void);
td_s32 voq_create(td_s32 chan_id);
td_s32 voq_reset(td_s32 chan_id);
td_s32 voq_destroy(td_s32 chan_id);
td_s32 voq_insert(td_s32 chan_id, vfmw_image *image_info);
td_s32 voq_delete(td_s32 chan_id, const vfmw_image *image_info);
td_s32 voq_is_img_in(td_s32 chan_id, const vfmw_image *image_info);
td_s32 voq_report(td_s32 chan_id);
td_s32 voq_receive(td_s32 chan_id, vfmw_image *orig_img);
td_s32 voq_release(td_s32 chan_id, vfmw_image_rls *img);
td_s32 voq_check_img(td_s32 chan_id, vfmw_image *src_img);
td_void voq_last_frm(td_s32 chan_id);
td_s32 voq_print(td_s32 chan_id);
td_s32 voq_get_mem_need_size(vfmw_svr_mem_need *mem);
td_s32 voq_set_ext_intf(const voq_ext_intf *intf);
td_s32 voq_get_info(td_s32 chan_id, td_void *args);
td_s32 voq_get_queue_img_num(td_s32 chan_id, td_u32 *read_num, td_u32 *new_num);
td_s32 voq_flush(td_s32 chan_id);
td_s32 voq_check_eos(td_s32 chan_id, td_s32 *flag);
td_void voq_set_speed(td_s32 chan_id, td_s32 speed);
td_s32 voq_config(td_s32 chan_id, const td_void *args);
voq_entry *voq_get_entry(td_void);
voq_chan *voq_get_chan(td_s32 chan_id);
td_void voq_sema_down(td_void);
td_void voq_sema_up(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __VOQUE_H__ */
