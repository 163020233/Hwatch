/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef STM_INTF_H
#define STM_INTF_H

#include "vfmw.h"
#include "stm_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    STM_CMD_CREATE_CHAN,
    STM_CMD_START_CHAN,
    STM_CMD_CONFIG_CHAN,
    STM_CMD_CFG_EXT_INTF,
    STM_CMD_STOP_CHAN,
    STM_CMD_RESET_CHAN,
    STM_CMD_DESTROY_CHAN,
    STM_CMD_SEEK_PTS,
    STM_CMD_GET_STATE,
    STM_CMD_PUSH_STREAM,
    STM_CMD_PULL_STREAM,
    STM_CMD_FLUSH_CHAN,
    STM_CMD_GET_SEG_NUM,
    STM_CMD_GET_INFO,
    STM_CMD_SET_ES_INVALID,
    STM_CMD_MAX,
} stm_cmd;

td_s32 stm_open(td_void);
td_s32 stm_close(td_void);
td_s32 stm_suspend(const td_void *opt);
td_s32 stm_resume(const td_void *opt);
td_s32 stm_wake_up(td_void *opt);
td_s32 stm_recvive_seg(td_s32 chan_id, stm_seg_info *seg);
td_s32 stm_release_seg(td_s32 chan_id, td_s32 seg_id);
td_s32 stm_set_ref_cnt(td_s32 chan_id, td_s32 seg_id, td_s32 ref_cnt);
td_s32 stm_control(td_s32 chan_id, td_s32 cmd, td_void *arg);
td_s32 stm_get_mem_need_size(vfmw_svr_mem_need *mem);
td_s32 stm_process(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* STM_INTF_H */
