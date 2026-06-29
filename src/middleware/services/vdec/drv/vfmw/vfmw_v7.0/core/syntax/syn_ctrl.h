/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __SYN_CTRL_H__
#define __SYN_CTRL_H__

#include "syn_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_s32 is_need_seg;
    td_s32 is_need_dps;
} syn_parse_result;

typedef struct {
    os_sema sema;
    td_s32 chan_id;
    td_s32 unid;
    td_s32 vid_std; /* video compressing standard */
    td_s32 priority;
    td_s32 lowdly;
    syn_chan_state state;
    vfmw_adpt_type adpt_type;
    syn_parse_result parse_result;
    stm_seg_info curr_seg;
    dps_info *curr_dps;
    td_u32 creat_dps_num;
    td_u32 be_taken_dps_num;
    td_u32 give_back_dps_num;
    td_u32 one_dps_cost_time[SYN_DEBUG_CNT];
    td_u32 create_dps_time;
    td_u32 one_dps_cost_max_time;
    td_u32 dps_consume_seg_num[SYN_DEBUG_CNT];
    td_u32 dps_consume_max_seg_num;
    td_u32 last_consume_seg_pos;
    td_u32 get_dps_try_cnt;
    td_u32 get_dps_succ_cnt;
    td_u32 get_dps_fail_cnt;
    td_u32 be_taken_dps_fail_cnt;
    td_u32 get_seg_num;
    td_u32 get_seg_try_cnt;
    td_u32 get_seg_succ_cnt;
    td_u32 get_seg_fail_cnt;
    td_u32 parse_cost_time[SYN_DEBUG_CNT];
    td_u8 seg_type[SYN_DEBUG_CNT];
    td_u32 parse_cost_max_time;
    td_u32 block_time;
    td_s32 (*read_seg)(td_s32 ChanId, stm_seg_info *pSeg);
} syn_chan;

typedef enum {
    THREAD_STA_EXIT,
    THREAD_STA_START,
    THREAD_STA_STOP,
    THREAD_STA_BUTT,
} syn_thread_status;

typedef enum {
    THREAD_CMD_CREATE,
    THREAD_CMD_DELETE,
    THREAD_CMD_START,
    THREAD_CMD_STOP,
    THREAD_CMD_BUTT,
} syn_thread_cmd;

typedef enum {
    THREAD_POS_WAITING,
    THREAD_POS_WORKING,
    THREAD_POS_SLEEPING,
    THREAD_POS_WAIT_DESTROY,
    THREAD_POS_DESTROY_DONE,
    THREAD_POS_BUTT
} syn_thread_work_pos;

typedef struct {
    td_void *thread;
    os_event event;
    syn_thread_status state;
    syn_thread_cmd cmd;
    syn_thread_work_pos work_pos;
    td_u32 run_cnt;
    td_s32 unid;
    td_u32 last_run_time;
    td_u32 scheduling_interval[SYN_DEBUG_CNT];
    td_u32 thread_cost_time[SYN_DEBUG_CNT];
    td_u32 thread_max_cost_time;
    td_u32 sleep_time;
    td_u32 wait_time;
} syn_thread;

typedef struct {
    os_sema sema;
    td_u32 is_block_cmd;
    td_u32 is_chan_state_changed;
    td_u32 sleep_time;
    td_u32 wait_time;
    syn_thread thread_ctx;

    td_u32 created_chan_num;
    td_u32 started_chan_num;
    td_s32 curr_high_prior_chan;
    td_s32 ctx_size;
    td_s32 chan_pri_tab_index;
    td_s32 chan_pri_tab[VFMW_CHAN_NUM][2]; /* chanId and Prio 2 :a number */
    syn_chan chan_ctx[VFMW_CHAN_NUM];
    td_u32 syn_no_recv_seg;
    td_u32 syn_spec_invalid;
    td_s32 bypass_chan;
} syn_ctrl_ctx;

td_s32 syn_ctrl_open(td_void);
td_s32 syn_ctrl_close(td_void);
td_s32 syn_ctrl_suspend(td_void);
td_s32 syn_ctrl_resume(td_void);

td_s32 syn_ctrl_create_chan(td_s32 chan_id);
td_s32 syn_ctrl_config_chan(td_s32 chan_id, const td_void *cfg);
td_s32 syn_ctrl_cfg_ext_intf(td_s32 chan_id, const td_void *cfg);
td_s32 syn_ctrl_get_info(td_s32 chan_id, td_void *args);
td_s32 syn_ctrl_cfg_fffb_speed(td_s32 chan_id, const td_void *args);
td_s32 syn_ctrl_cfg_pvr_info(td_s32 chan_id, const td_void *args);
td_s32 syn_ctrl_start_chan(td_s32 chan_id);
td_s32 syn_ctrl_stop_chan(td_s32 chan_id);
td_s32 syn_ctrl_destroy_chan(td_s32 chan_id);
td_s32 syn_ctrl_reset_chan(td_s32 chan_id, td_void *args);
td_s32 syn_ctrl_wake_up(td_void);
td_s32 syn_ctrl_clean_thread(td_void);

syn_ctrl_ctx *syn_ctrl_get_ctx(td_void);

td_void syn_ctrl_sema_down(td_void);
td_void syn_ctrl_sema_up(td_void);
td_s32 syn_ctrl_get_mem_need_size(vfmw_svr_mem_need *mem);
td_void syn_ctrl_read_std_log(td_s32 chan_id, td_void *buf, td_s32 len, td_s32 *used);

td_s32 syn_ctrl_run_thread(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SYN_CTRL_H__ */
