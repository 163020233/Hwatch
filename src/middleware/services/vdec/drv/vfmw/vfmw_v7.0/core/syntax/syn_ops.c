/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "syn_ops.h"
#include "dbg.h"
#include "vfmw_define.h"
#ifdef VFMW_HEVC_SUPPORT
#include "hevc.h"
#endif
#ifdef VFMW_H264_SUPPORT
#include "h264.h"
#endif
#ifdef VFMW_MVC_SUPPORT
#include "mvc.h"
#endif

#ifdef VFMW_HEVC_SUPPORT
syn_ops g_hevc_ops = {
    .vid_std = VFMW_HEVC,
    .init_syn_ctx = hevc_init_,
    .config_syn_ctx = hevc_config,
    .decode_stream = hevc_decode,
    .deinit_syn_ctx = hevc_deinit,
    .reset_syn_ctx = hevc_reset,
    .get_syn_ctx_size = hevc_get_ctx_size,
    .get_syn_max_size = hevc_get_max_size,
    .read_std_log = hevc_read_log,
    .get_parse_info = hevc_get_parse_info,
    .ctrl_syn_ctx = hevc_control,
};
#endif

#ifdef VFMW_H264_SUPPORT
syn_ops g_h264_ops = {
    .vid_std = VFMW_H264,
    .init_syn_ctx = h264_init,
    .config_syn_ctx = h264_config,
    .decode_stream = h264_decode,
    .deinit_syn_ctx = h264_deinit,
    .reset_syn_ctx = h264_reset,
    .get_syn_ctx_size = h264_get_ctx_size,
    .get_syn_max_size = h264_get_max_size,
    .read_std_log = h264_read_log,
    .get_parse_info = h264_get_parse_info,
    .ctrl_syn_ctx = h264_control,
};
#endif

#ifdef VFMW_MVC_SUPPORT
syn_ops g_mvc_ops = {
    .vid_std = VFMW_MVC,
    .init_syn_ctx = mvc_init,
    .config_syn_ctx = mvc_config,
    .decode_stream = mvc_decode,
    .deinit_syn_ctx = mvc_deinit,
    .reset_syn_ctx = mvc_reset,
    .get_syn_ctx_size = mvc_get_ctx_size,
    .get_syn_max_size = mvc_get_max_size,
    .ctrl_syn_ctx = mvc_control,
    .read_std_log = mvc_read_log,
};
#endif

static ops_entry g_ops_entry = {
    .reserve1 = 0x1234abcd,
    .reserve2 = 0x1234abcd,
};

STATIC ops_entry *syn_ops_get_entry(td_void)
{
    if (g_ops_entry.reserve1 != 0x1234abcd || g_ops_entry.reserve2 != 0x1234abcd) {
        dprint(PRN_ERROR, "%s reserve = 0x%x, 0x%x\n", __func__, g_ops_entry.reserve1, g_ops_entry.reserve2);
    }

    return &g_ops_entry;
}

td_s32 syn_ops_add(syn_ops *ops)
{
    td_s32 id;
    ops_entry *entry = TD_NULL;

    if (ops == TD_NULL) {
        dprint(PRN_ERROR, "%s ops = TD_NULL \n", __func__);
        return SYN_ERR;
    }

    entry = syn_ops_get_entry();
    if (entry->state == OPS_NULL) {
        dprint(PRN_ERROR, "%s state error \n", __func__);
        return SYN_ERR;
    }

    for (id = 0; id < SYN_OPS_NUM; id++) {
        if (entry->ops_list[id] == TD_NULL) {
            entry->ops_list[id] = ops;
            break;
        }
    }

    if (id == SYN_OPS_NUM) {
        return SYN_ERR;
    }

    return SYN_OK;
}

syn_ops *syn_ops_get(td_s32 vid_std)
{
    td_s32 id;
    syn_ops *ops = TD_NULL;
    ops_entry *entry = TD_NULL;

    entry = syn_ops_get_entry();
    if (entry->state == OPS_NULL) {
        dprint(PRN_ERROR, "%s state error \n", __func__);
        return ops;
    }

    for (id = 0; id < SYN_OPS_NUM; id++) {
        if (entry->ops_list[id] != TD_NULL &&
            entry->ops_list[id]->vid_std == vid_std) {
            ops = entry->ops_list[id];
            break;
        }
    }

    return ops;
}

STATIC td_void syn_ops_add_part1(td_void)
{
#ifdef VFMW_H264_SUPPORT
    if (syn_ops_add(&g_h264_ops) != SYN_OK) {
        dprint(PRN_ERROR, "add h264 err\n");
    }
#endif
#ifdef VFMW_HEVC_SUPPORT
    if (syn_ops_add(&g_hevc_ops) != SYN_OK) {
        dprint(PRN_ERROR, "add hevc err\n");
    }
#endif
#ifdef VFMW_MVC_SUPPORT
    if (syn_ops_add(&g_mvc_ops) != SYN_OK) {
        dprint(PRN_ERROR, "add mvc err\n");
    }
#endif
}

td_s32 syn_ops_open(td_void)
{
    ops_entry *entry = TD_NULL;

    entry = syn_ops_get_entry();
    if (entry->state == OPS_OPEN) {
        dprint(PRN_ERROR, "%s state error \n", __func__);
        return SYN_ERR;
    }

    entry->state = OPS_OPEN;
    vfmw_check_sec_func(memset_s(entry->ops_list, sizeof(entry->ops_list), 0, sizeof(entry->ops_list)));

    syn_ops_add_part1();

    return SYN_OK;
}

td_s32 syn_ops_close(td_void)
{
    ops_entry *entry = TD_NULL;

    entry = syn_ops_get_entry();
    if (entry->state == OPS_NULL) {
        dprint(PRN_ERROR, "%s state error \n", __func__);
        return SYN_ERR;
    }

    entry->state = OPS_NULL;
    vfmw_check_sec_func(memset_s(entry->ops_list, sizeof(entry->ops_list), 0, sizeof(entry->ops_list)));

    return SYN_OK;
}

td_s32 syn_ops_get_chan_mem_max_size(vfmw_svr_mem_need *mem)
{
    td_u32 size = 0;
    td_u32 tmp_size;
    td_s32 id;
    ops_entry *entry = TD_NULL;

    entry = syn_ops_get_entry();
    if (entry->state == OPS_NULL) {
        dprint(PRN_ERROR, "%s state error \n", __func__);
        return SYN_ERR;
    }

    for (id = 0; id < SYN_OPS_NUM; id++) {
        if (entry->ops_list[id] != TD_NULL) {
            tmp_size = (td_u32)entry->ops_list[id]->get_syn_max_size();
            if (tmp_size > size) {
                size = tmp_size;
            }
        }
    }

    vfmw_check_ret(mem != TD_NULL, SYN_ERR);
    mem->chan_cache_size += vfmw_align_up(size, VFMW_CTX_ALIGN);

    return SYN_OK;
}
