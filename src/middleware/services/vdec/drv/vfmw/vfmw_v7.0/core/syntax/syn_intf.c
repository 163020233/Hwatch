/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "syn_intf.h"
#include "syn_ctrl.h"
#include "vdec_chr.h"

typedef td_s32 (*fn_syn_cmd_handler)(td_s32 chan_id, td_void *args);
typedef struct {
    syn_cmd_id cmd_id;
    fn_syn_cmd_handler handler;
} syn_cmd_node;

typedef struct soc_syn_intf_ctx {
    td_s32 is_open;
    syn_chan_state chan_sta[VFMW_CHAN_NUM];
} syn_intf_ctx;

static syn_intf_ctx g_st_syn_intf_ctx = {
    .is_open = TD_FALSE,
};

#define syn_get_ctx_(ptr) do {      \
    (ptr) = &g_st_syn_intf_ctx;     \
} while (0)

STATIC td_s32 syn_create_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret = SYN_OK;
    syn_intf_ctx *ctx = TD_NULL;

    vfmw_unused(args);
    syn_get_ctx_(ctx);

    if (ctx->chan_sta[chan_id] != SYN_CHAN_NONE || ctx->is_open != TD_TRUE) {
        ret = SYN_ERR;
        return ret;
    }

    syn_ctrl_create_chan(chan_id);

    ctx->chan_sta[chan_id] = SYN_CHAN_CREATE_DONE;

    return ret;
}

STATIC td_s32 syn_cfg_ext_intf(td_s32 chan_id, td_void *args)
{
    syn_intf_ctx *ctx = TD_NULL;

    syn_get_ctx_(ctx);

    if (ctx->is_open != TD_TRUE) {
        dprint(PRN_ERROR, "syn not open, chan %d state %d\n", chan_id, ctx->chan_sta[chan_id]);
        return SYN_ERR;
    }

    return syn_ctrl_cfg_ext_intf(chan_id, args);
}

STATIC td_s32 syn_get_info(td_s32 chan_id, td_void *args)
{
    syn_intf_ctx *ctx = TD_NULL;

    syn_get_ctx_(ctx);
    vfmw_check_ret(ctx->chan_sta[chan_id] != SYN_CHAN_NONE, SYN_ERR);
    vfmw_check_ret(ctx->is_open == TD_TRUE, SYN_ERR);

    return syn_ctrl_get_info(chan_id, args);
}

STATIC td_s32 syn_cfg_fffb_speed(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    syn_intf_ctx *ctx = TD_NULL;

    syn_get_ctx_(ctx);
    vfmw_check_ret(ctx->chan_sta[chan_id] != SYN_CHAN_NONE, SYN_ERR);
    vfmw_check_ret(ctx->is_open == TD_TRUE, SYN_ERR);

    ret = syn_ctrl_cfg_fffb_speed(chan_id, args);

    return ret;
}

STATIC td_s32 syn_cfg_pvr_info(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    syn_intf_ctx *ctx = TD_NULL;

    syn_get_ctx_(ctx);
    vfmw_check_ret(ctx->chan_sta[chan_id] != SYN_CHAN_NONE, SYN_ERR);
    vfmw_check_ret(ctx->is_open == TD_TRUE, SYN_ERR);

    ret = syn_ctrl_cfg_pvr_info(chan_id, args);

    return ret;
}

STATIC td_s32 syn_config_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret = SYN_OK;
    syn_intf_ctx *ctx = TD_NULL;

    syn_get_ctx_(ctx);

    if (ctx->chan_sta[chan_id] == SYN_CHAN_NONE) {
        dprint(PRN_ERROR, "syn_config_chan %d has not been created\n", chan_id);
        ret = SYN_ERR;
        return ret;
    }

    if (args == TD_NULL) {
        syn_cfg_info st_cfg = { 0 };
        st_cfg.vid_std = VFMW_HEVC;
        st_cfg.priority = 1;
        st_cfg.dec_mode = VFMW_DEC_MODE_NORMAL;
        st_cfg.output_order = VFMW_DEC_ORDER;

        dprint(PRN_ERROR, "syn_config_chan args = TD_NULL, syn select H265 default\n");
        syn_ctrl_config_chan(chan_id, (td_void *)&st_cfg);
    } else {
        syn_cfg_info *cfg = (syn_cfg_info *)args;

        if (cfg->vid_std <= VFMW_STD_START || cfg->vid_std >= VFMW_STD_MAX) {
            dprint(PRN_ERROR, "syn_config_chan vid_std = %d is ERR!\n", cfg->vid_std);
            ret = SYN_ERR;
            return ret;
        }
        syn_ctrl_config_chan(chan_id, args);
    }

    if (ctx->chan_sta[chan_id] < SYN_CHAN_CONFIG_DONE) {
        ctx->chan_sta[chan_id] = SYN_CHAN_CONFIG_DONE; /* set state to config only at first config */
    }

    return ret;
}

STATIC td_s32 syn_start_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret = SYN_OK;
    syn_intf_ctx *ctx = TD_NULL;

    vfmw_unused(args);
    syn_get_ctx_(ctx);

    if (ctx->chan_sta[chan_id] != SYN_CHAN_CONFIG_DONE &&
        ctx->chan_sta[chan_id] != SYN_CHAN_RESET_DONE &&
        ctx->chan_sta[chan_id] != SYN_CHAN_STOP_DONE) {
        dprint(PRN_ERROR, "syn_start_chan %d state %d\n", chan_id, ctx->chan_sta[chan_id]);
        ret = SYN_ERR;
        return ret;
    }

    syn_ctrl_start_chan(chan_id);

    ctx->chan_sta[chan_id] = SYN_CHAN_START_DONE;
    vdec_record_chr_event(chan_id, MT_VDEC_START, MT_VDEC_VFMW_START, MT_VFMW_SYN_START_OK);

    return ret;
}

STATIC td_s32 syn_stop_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret = SYN_OK;
    syn_intf_ctx *ctx = TD_NULL;

    vfmw_unused(args);
    syn_get_ctx_(ctx);

    if (ctx->chan_sta[chan_id] == SYN_CHAN_NONE) {
        ret = SYN_ERR;
        return ret;
    }

    syn_ctrl_stop_chan(chan_id);

    ctx->chan_sta[chan_id] = SYN_CHAN_STOP_DONE;
    vdec_record_chr_event(chan_id, MT_VDEC_STOP, MT_VDEC_VFMW_STOP, MT_VFMW_SYN_STOP_OK);

    return ret;
}

STATIC td_s32 syn_reset_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret = SYN_OK;
    syn_intf_ctx *ctx = TD_NULL;

    syn_get_ctx_(ctx);

    if (ctx->chan_sta[chan_id] == SYN_CHAN_NONE) {
        ret = SYN_ERR;
        return ret;
    }

    syn_ctrl_reset_chan(chan_id, args);

    ctx->chan_sta[chan_id] = SYN_CHAN_RESET_DONE;
    vdec_record_chr_event(chan_id, MT_VDEC_RESET, MT_VDEC_VFMW_RESET, MT_VFMW_SYN_RESET_OK);

    return ret;
}

STATIC td_s32 syn_destroy_chan(td_s32 chan_id, td_void *args)
{
    td_s32 ret = SYN_OK;
    syn_intf_ctx *ctx = TD_NULL;

    vfmw_unused(args);
    syn_get_ctx_(ctx);
    if (ctx->chan_sta[chan_id] == SYN_CHAN_NONE) {
        ret = SYN_ERR;
        return ret;
    }

    syn_ctrl_destroy_chan(chan_id);

    ctx->chan_sta[chan_id] = SYN_CHAN_NONE;

    return ret;
}

td_s32 syn_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    syn_ctrl_get_mem_need_size(mem);

    return SYN_OK;
}

td_s32 syn_open(td_void)
{
    td_s32 ret;
    syn_intf_ctx *ctx = TD_NULL;

    syn_get_ctx_(ctx);

    if (ctx->is_open == TD_TRUE) {
        dprint(PRN_ERROR, "syn has been opened!\n");
        return SYN_ERR;
    }

    vfmw_check_sec_func(memset_s(ctx, sizeof(syn_intf_ctx), 0, sizeof(syn_intf_ctx)));

    ret = syn_ctrl_open();

    ctx->is_open = TD_TRUE;

    return ret;
}

td_s32 syn_close(td_void)
{
    td_s32 ret;
    syn_intf_ctx *ctx = TD_NULL;

    syn_get_ctx_(ctx);
    if (ctx->is_open == TD_FALSE) {
        dprint(PRN_ERROR, "syn has been closed!\n");
        return SYN_ERR;
    }

    ret = syn_ctrl_close();

    ctx->is_open = TD_FALSE;

    return ret;
}

td_s32 syn_suspend(const td_void *opt)
{
    td_s32 ret;
    syn_intf_ctx *ctx = TD_NULL;

    vfmw_unused(opt);
    syn_get_ctx_(ctx);

    if (ctx->is_open == TD_FALSE) {
        dprint(PRN_ERROR, "syn has not been Opened!\n");
        return SYN_ERR;
    }

    ret = syn_ctrl_suspend();

    return ret;
}

td_s32 syn_resume(const td_void *opt)
{
    td_s32 ret;
    syn_intf_ctx *ctx = TD_NULL;

    vfmw_unused(opt);
    syn_get_ctx_(ctx);

    if (ctx->is_open == TD_FALSE) {
        dprint(PRN_ERROR, "syn has not been Opened!\n");
        return SYN_ERR;
    }

    ret = syn_ctrl_resume();

    return ret;
}

td_s32 syn_control(td_s32 chan_id, syn_cmd_id cmd_id, td_void *args)
{
    td_s32 ret;
    td_u32 index = 0;
    fn_syn_cmd_handler target_handler = TD_NULL;

    STATIC const syn_cmd_node syn_cmd_handler_table[] = {
        { SYN_CID_CREATE_CHAN,     syn_create_chan },
        { SYN_CID_CONFIG_CHAN,     syn_config_chan },
        { SYN_CID_START_CHAN,      syn_start_chan },
        { SYN_CID_STOP_CHAN,       syn_stop_chan },
        { SYN_CID_RESET_CHAN,      syn_reset_chan },
        { SYN_CID_DESTROY_CHAN,    syn_destroy_chan },
        { SYN_CID_CONFIG_EXT_INTF, syn_cfg_ext_intf },
        { SYN_CID_SET_TRICKMODE,   syn_cfg_fffb_speed },
        { SYN_CID_SET_CTRLINFO,    syn_cfg_pvr_info },
        { SYN_CID_GET_INFO,        syn_get_info },
        { SYN_CID_BUTT,            TD_NULL }
    };

    if (chan_id >= VFMW_CHAN_NUM || chan_id < 0) {
        dprint(PRN_ERROR, "syn receive wrong chan_id = %d\n", chan_id);
        return SYN_ERR;
    }

    while (index < sizeof(syn_cmd_handler_table) / sizeof(syn_cmd_node)) {
        if (syn_cmd_handler_table[index].cmd_id == SYN_CID_BUTT ||
            syn_cmd_handler_table[index].handler == TD_NULL) {
            break;
        }

        if (cmd_id == syn_cmd_handler_table[index].cmd_id) {
            target_handler = syn_cmd_handler_table[index].handler;
            break;
        }

        index++;
    }

    if (target_handler != TD_NULL) {
        ret = target_handler(chan_id, args);
    } else {
        dprint(PRN_ERROR, "unsupport cmd_id %d\n", cmd_id);
        ret = SYN_ERR;
    }

    return ret;
}

td_s32 syn_wake_up(td_void)
{
    td_s32 ret;

    ret = syn_ctrl_wake_up();

    return ret;
}

td_s32 syn_process(td_void)
{
    td_s32 ret;

    ret = syn_ctrl_run_thread();

    return ret;
}
