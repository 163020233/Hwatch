/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __OPS_H__
#define __OPS_H__

#include "vfmw.h"
#include "syn_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OPS_NULL 0
#define OPS_OPEN 1

#define SYN_OPS_NUM 32

typedef struct {
    td_s32 ops_id;
    td_s32 vid_std;

    td_s32 (*init_syn_ctx)(td_s32 chan_id, td_void **ctx, const syn_cfg_info *extra_data);
    td_s32 (*config_syn_ctx)(td_void *ctx, const syn_cfg_info *extra_data);
    syn_parse_state (*decode_stream)(stm_seg_info *seg, dps_info *dps, td_void *ctx);
    td_s32 (*deinit_syn_ctx)(td_void *ctx, td_void *extra_data);
    td_s32 (*reset_syn_ctx)(td_void *ctx, td_void *args);
    td_u32 (*get_syn_ctx_size)(const td_void *ctx);
    td_s32 (*get_syn_max_size)(td_void);
    td_void (*read_std_log)(const td_void *, td_void *, td_s32, td_s32 *);
    td_s32 (*get_parse_info)(const td_void *ctx, td_void *args);
    td_s32 (*ctrl_syn_ctx)(td_void *ctx, td_s32 cmd_id, const td_void *args);
} syn_ops;

typedef struct {
    td_u32 reserve1;

    td_s32 state;
    syn_ops *ops_list[SYN_OPS_NUM];

    td_u32 reserve2;
} ops_entry;

td_s32 syn_ops_open(td_void);
td_s32 syn_ops_close(td_void);
td_s32 syn_ops_add(syn_ops *ops);
syn_ops *syn_ops_get(td_s32 vid_std);
td_s32 syn_ops_get_chan_mem_max_size(vfmw_svr_mem_need *mem);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __VFMW_OPS_H__ */
