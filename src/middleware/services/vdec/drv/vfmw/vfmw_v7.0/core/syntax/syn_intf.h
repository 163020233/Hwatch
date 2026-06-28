/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __SYN_INTF_H__
#define __SYN_INTF_H__

#include "syn_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum syn_cmd_id {
    SYN_CID_CREATE_CHAN,
    SYN_CID_CONFIG_CHAN,
    SYN_CID_CONFIG_EXT_INTF,
    SYN_CID_START_CHAN,
    SYN_CID_STOP_CHAN,
    SYN_CID_RESET_CHAN,
    SYN_CID_DESTROY_CHAN,
    SYN_CID_GET_INFO,
    SYN_CID_SET_TRICKMODE,
    SYN_CID_SET_CTRLINFO,
    SYN_CID_BUTT,
} syn_cmd_id;

td_s32 syn_open(td_void);
td_s32 syn_close(td_void);
td_s32 syn_suspend(const td_void *opt);
td_s32 syn_resume(const td_void *opt);
td_s32 syn_control(td_s32 chan_id, syn_cmd_id cmd_id, td_void *args);
td_s32 syn_wake_up(td_void);
td_s32 syn_get_mem_need_size(vfmw_svr_mem_need *mem);
td_s32 syn_process(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __SYN_INTF_H__ */