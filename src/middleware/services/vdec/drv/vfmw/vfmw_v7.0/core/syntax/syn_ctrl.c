/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "syn_ctrl.h"
#include "syn_parse.h"
#include "vdec_chr.h"

static syn_ctrl_ctx g_syn_ctrl_ctx;
static td_s32 g_parse_result[SYN_PARSE_END - (SYN_PARSE_BEGIN + 1)][2] = { /* 2 :a number */
    /* IsNeedSeg    IsNeedDps */
    { TD_TRUE,  TD_TRUE },   /* SYN_PARSE_CURR_FRMDONE */
    { TD_FALSE, TD_TRUE },  /* SYN_PARSE_PRE_FRMDONE */
    { TD_FALSE, TD_FALSE }, /* SYN_PARSE_PRE_FRMERR */
    { TD_TRUE,  TD_FALSE },  /* SYN_PARSE_NOT_FRMDONE */
    { TD_FALSE, TD_FALSE }, /* SYN_PARSE_RESOURCE_INVALID */
    { TD_TRUE,  TD_FALSE }   /* SYN_PARSE_ERR */
};

#define syn_ctrl_get_ctx_(ptr) do {  \
    (ptr) = &g_syn_ctrl_ctx;    \
} while (0)

td_void syn_ctrl_sema_down(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_ctrl_get_ctx_(ctx);
    os_sema_down(ctx->sema);
}

td_void syn_ctrl_sema_up(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_ctrl_get_ctx_(ctx);
    os_sema_up(ctx->sema);
}

STATIC td_void syn_ctrl_print_block(td_s32 chan_id, const td_char *info)
{
    td_u32 cur_time;
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;

    (td_void)info; /* avoid to build err when printing is not supported. */
    syn_ctrl_get_ctx_(ctx);
    chan = &ctx->chan_ctx[chan_id];

    cur_time = os_get_time_ms();
    if (cur_time - chan->block_time < 300) { /* 300 :a number */
        return;
    }

    chan->block_time = cur_time;

    dprint(PRN_SYN_BLK, "Chan %d SYN block of %s\n", chan->chan_id, info);
}

STATIC td_s32 syn_ctrl_decode_stream(syn_chan *chan_ctx)
{
    syn_parse_state ret;
    syn_ctrl_ctx *ctx;
    syn_parse_result *parse_result = &chan_ctx->parse_result;
    td_u32 pre_time;
    td_u32 curr_time;
    td_u32 cost_time;
    td_u32 index;

    ctx = syn_ctrl_get_ctx();
    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);
    vfmw_check_ret(os_get_time_us != TD_NULL, SYN_ERR);

    pre_time = os_get_time_us();
    chan_ctx->curr_seg.seg_type = STM_SEG_INVALID;
    if (ctx->syn_spec_invalid == 1) {
        return SYN_PARSE_ERR;
    }
    ret = syn_parse_decode_stream(&chan_ctx->curr_seg, chan_ctx->curr_dps, chan_ctx->chan_id);
    curr_time = os_get_time_us();

    cost_time = curr_time - pre_time;
    chan_ctx->parse_cost_time[chan_ctx->get_seg_num % SYN_DEBUG_CNT] = cost_time;
    chan_ctx->parse_cost_max_time = (chan_ctx->parse_cost_max_time < cost_time)
                                    ? cost_time : chan_ctx->parse_cost_max_time;

    chan_ctx->seg_type[chan_ctx->get_seg_num % SYN_DEBUG_CNT] = chan_ctx->curr_seg.seg_type;
    chan_ctx->get_seg_num++;

    ret = (ret <= SYN_PARSE_BEGIN + 1) ? SYN_PARSE_BEGIN + 1 : ret;
    index = (td_u32)ret - (td_u32)(SYN_PARSE_BEGIN + 1);
    parse_result->is_need_seg = g_parse_result[index][0];
    parse_result->is_need_dps = g_parse_result[index][1];

    if (ret == SYN_PARSE_RESOURCE_INVALID) {
        syn_ctrl_print_block(chan_ctx->chan_id, "no resource");
        return SYN_PARSE_RESOURCE_INVALID; /* here need free cpu */
    }

    return SYN_OK;
}

STATIC td_s32 syn_ctrl_prepare_data(syn_chan *chan_ctx)
{
    td_s32 ret = SYN_OK;
    syn_ctrl_ctx *ctx;
    syn_parse_result *parse_result = &chan_ctx->parse_result;

    ctx = syn_ctrl_get_ctx();
    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);
    vfmw_check_ret(os_get_time_us != TD_NULL, SYN_ERR);

    if (parse_result->is_need_dps != 0) {
        chan_ctx->get_dps_try_cnt++;
        ret = dps_get_node(chan_ctx->chan_id, &chan_ctx->curr_dps);
        if (ret != DPS_OK) {
            syn_ctrl_print_block(chan_ctx->chan_id, "no dps");
            chan_ctx->get_dps_fail_cnt++;
            return SYN_ERR;
        }

        chan_ctx->get_dps_succ_cnt++;
        chan_ctx->create_dps_time = os_get_time_us();
        parse_result->is_need_dps = TD_FALSE;
    }

    if (ctx->syn_no_recv_seg == 1) {
        parse_result->is_need_seg = 0;
        ret = SYN_PARSE_RESOURCE_INVALID;
    }

    if (parse_result->is_need_seg != 0) {
        chan_ctx->get_seg_try_cnt++;
        vfmw_check_ret(chan_ctx->read_seg != TD_NULL, SYN_ERR);
        ret = chan_ctx->read_seg(chan_ctx->chan_id, &chan_ctx->curr_seg);
        if (ret == SYN_ERR) {
            syn_ctrl_print_block(chan_ctx->chan_id, "no seg");
            chan_ctx->get_seg_fail_cnt++;
            return SYN_ERR;
        }
        if (ctx->bypass_chan == chan_ctx->chan_id) {
            stm_release_seg(chan_ctx->chan_id, chan_ctx->curr_seg.stream_id);
            chan_ctx->get_seg_fail_cnt++;
            return SYN_ERR;
        }
        chan_ctx->get_seg_succ_cnt++;
        parse_result->is_need_seg = TD_FALSE;
    }

    if (fsp_check_fs_ok(chan_ctx->chan_id) != FSP_OK) {
        return SYN_ERR;
    }

    return ret;
}

STATIC td_s32 syn_ctrl_dec_chan_syntax(td_void)
{
    td_s32 ret = SYN_OK;
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan_ctx = TD_NULL;
    td_s32 chan_id;
    td_u32 created_dps_num;
    td_bool one_time_done = TD_FALSE;

    syn_ctrl_get_ctx_(ctx);

    chan_id = ctx->curr_high_prior_chan;
    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        dprint(PRN_ALWS, "%s chan_id is invalide:%d \n", __func__, chan_id);
        return SYN_ERR;
    }
    chan_ctx = &ctx->chan_ctx[chan_id];

    os_sema_down(chan_ctx->sema);
    created_dps_num = chan_ctx->creat_dps_num;

    while (one_time_done == TD_FALSE) {
        if (chan_ctx->state != SYN_CHAN_START_DONE) {
            goto one_time_out;
        }

        ret = syn_ctrl_prepare_data(chan_ctx);
        if (ret != SYN_OK) {
            goto one_time_out;
        }

        ret = syn_ctrl_decode_stream(chan_ctx);
        if (ret != SYN_OK) {
            goto one_time_out;
        }

        if (chan_ctx->parse_result.is_need_dps != 0) {
            (td_void)dps_insert(chan_ctx->chan_id, chan_ctx->curr_dps);
            chan_ctx->creat_dps_num++;
        }

        one_time_done = (chan_ctx->creat_dps_num != created_dps_num) ? TD_TRUE : TD_FALSE;
    }

one_time_out:

    os_sema_up(chan_ctx->sema);

    return ret;
}

STATIC td_s32 syn_ctrl_update_chan_prior_tab(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    td_s32 i, j, tmp_pri_chan[2]; /* 2 :a number */
    td_s32 soft_flag;

    syn_ctrl_get_ctx_(ctx);

    ctx->chan_pri_tab_index = 0;

    /* Mutil Chan, No changed, No need Sort priority */
    if (ctx->is_chan_state_changed == TD_FALSE) {
        return SYN_OK;
    } else {
        /* When Vctrl destroy the chan, CreatedChanNum is 1,
           It will destroy thread, Limited > 1 can avoid
           dead lock here, so singel chan is invalid to the scence,
           However, the sort alg is high-performance for singel chan */
        if (ctx->created_chan_num > 1) {
            os_sema_down(ctx->sema);
            ctx->is_chan_state_changed = TD_FALSE;
            os_sema_up(ctx->sema);
        }
    }

    /* snap current prior status */
    ctx->started_chan_num = 0;
    for (i = 0; i < VFMW_CHAN_NUM; i++) {
        if (ctx->chan_ctx[i].state == SYN_CHAN_START_DONE && ctx->started_chan_num < VFMW_CHAN_NUM) {
            ctx->chan_pri_tab[ctx->started_chan_num][1] = ctx->chan_ctx[i].priority + 1; /* 0 is invalid */
            ctx->chan_pri_tab[ctx->started_chan_num][0] = i;
            ctx->started_chan_num++;
        }
    }

    for (i = (td_s32)ctx->started_chan_num; i < VFMW_CHAN_NUM; i++) {
        ctx->chan_pri_tab[i][1] = 0;
        ctx->chan_pri_tab[i][0] = 0;
    }

    if (ctx->started_chan_num == 0) {
        return SYN_OK;
    }

    /* sort prio */
    for (i = 0; i < VFMW_CHAN_NUM - 1; i++) {
        soft_flag = TD_TRUE;
        for (j = i + 1; j < VFMW_CHAN_NUM; j++) {
            if (ctx->chan_pri_tab[i][1] < ctx->chan_pri_tab[j][1]) {
                tmp_pri_chan[0] = ctx->chan_pri_tab[i][0];
                tmp_pri_chan[1] = ctx->chan_pri_tab[i][1];

                ctx->chan_pri_tab[i][0] = ctx->chan_pri_tab[j][0];
                ctx->chan_pri_tab[i][1] = ctx->chan_pri_tab[j][1];

                ctx->chan_pri_tab[j][0] = tmp_pri_chan[0];
                ctx->chan_pri_tab[j][1] = tmp_pri_chan[1];
                soft_flag = TD_FALSE;
            }
        }
        if (soft_flag == TD_TRUE) {
            break;
        }
    }

    return SYN_OK;
}

STATIC td_s32 syn_ctrl_get_high_prior_chan(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;

    syn_ctrl_get_ctx_(ctx);

    ctx->curr_high_prior_chan = ctx->chan_pri_tab[ctx->chan_pri_tab_index][0];
    ctx->chan_pri_tab_index++;
    if (ctx->curr_high_prior_chan < 0 || ctx->curr_high_prior_chan >= VFMW_CHAN_NUM) {
        dprint(PRN_ERROR, "%s chan_id is invalide:%d tab index:%d start:%d\n", __func__, ctx->curr_high_prior_chan,
            ctx->chan_pri_tab_index, ctx->started_chan_num);
        ctx->curr_high_prior_chan = 0;
    }

    return SYN_OK;
}

td_s32 syn_ctrl_run_thread(td_void)
{
    td_s32 ret;
    syn_ctrl_ctx *ctx = TD_NULL;
    td_s32 started_chan_num;

    ret = SYN_OK;
    syn_ctrl_get_ctx_(ctx);

    if (syn_ctrl_update_chan_prior_tab() == SYN_ERR) {
        dprint(PRN_ERROR, "%s update chan prior error!\n", __func__);
    }

    started_chan_num = (td_s32)ctx->started_chan_num;
    while ((started_chan_num--) != 0) {
        if (syn_ctrl_get_high_prior_chan() == SYN_ERR) {
            dprint(PRN_ERROR, "%s get high chan prior error!\n", __func__);
        }
        ret = syn_ctrl_dec_chan_syntax();
    }

    return ret;
}

STATIC td_s32 syn_ctrl_lowdly_en(syn_ctrl_ctx *ctx)
{
    td_s32 chan_id;
    td_s32 ret = TD_FALSE;
    syn_chan *chan_ctx = TD_NULL;

    if (ctx->started_chan_num > 1) {
        return TD_FALSE;
    }
    chan_id = ctx->curr_high_prior_chan;
    if (chan_id < 0 || chan_id >= VFMW_CHAN_NUM) {
        dprint(PRN_ALWS, "%s chan_id is invalide:%d \n", __func__, chan_id);
        return TD_FALSE;
    }

    chan_ctx = &ctx->chan_ctx[chan_id];

    if (chan_ctx->lowdly != 0) {
        ret = TD_TRUE;
    } else {
        ret = TD_FALSE;
    }
    return ret;
}

STATIC td_void syn_ctrl_wait(td_void)
{
    td_u32 wait_time = 0;
    syn_ctrl_ctx *ctx = TD_NULL;

    syn_ctrl_get_ctx_(ctx);

    if (ctx->thread_ctx.wait_time > 0) {
        wait_time = ctx->thread_ctx.wait_time;
    } else {
        if (syn_ctrl_lowdly_en(ctx) == TD_TRUE) {
            wait_time = 1;
        } else {
            wait_time = 5; /* 5 :a number */
        }
    }
    os_event_wait(ctx->thread_ctx.event, (td_s32)wait_time);
}

STATIC td_void syn_ctrl_sleep(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;

    syn_ctrl_get_ctx_(ctx);
    if (ctx->thread_ctx.sleep_time > 0) {
        os_msleep(ctx->thread_ctx.sleep_time);
    }
}

STATIC td_s32 syn_ctrl_thread_func(td_void *data)
{
    td_s32 ret;
    syn_ctrl_ctx *ctx = TD_NULL;
    td_u32 curr_time, end_time, cost_time;
    td_u32 curr_run_cnt;
    td_s32 thread_unid;

    vfmw_unused(data);
    vfmw_check_ret(os_get_time_us != TD_NULL, SYN_ERR);

    ret = SYN_OK;
    syn_ctrl_get_ctx_(ctx);
    thread_unid = ctx->thread_ctx.unid;

    while (thread_unid == ctx->thread_ctx.unid &&
           ctx->thread_ctx.state != THREAD_STA_EXIT) {
        while (thread_unid == ctx->thread_ctx.unid &&
               ctx->thread_ctx.state == THREAD_STA_START) {
            ctx->thread_ctx.work_pos = THREAD_POS_WAITING;
            syn_ctrl_wait();
            ctx->thread_ctx.work_pos = THREAD_POS_WORKING;

            curr_time = os_get_time_us();
            cost_time = (curr_time > ctx->thread_ctx.last_run_time) ? (curr_time - ctx->thread_ctx.last_run_time) : 0;
            curr_run_cnt = ctx->thread_ctx.run_cnt;
            curr_run_cnt = curr_run_cnt % SYN_DEBUG_CNT;
            vfmw_check_ret(curr_run_cnt < SYN_DEBUG_CNT, ret);
            ctx->thread_ctx.scheduling_interval[curr_run_cnt] = cost_time;
            ctx->thread_ctx.last_run_time = curr_time;

            ret = syn_ctrl_run_thread();
            end_time = os_get_time_us();
            cost_time = (end_time > curr_time) ? (end_time - curr_time) : 0;
            ctx->thread_ctx.thread_cost_time[curr_run_cnt] = cost_time;
            ctx->thread_ctx.thread_max_cost_time = (ctx->thread_ctx.thread_max_cost_time < cost_time)
                                                   ?  cost_time : ctx->thread_ctx.thread_max_cost_time;
            ctx->thread_ctx.run_cnt++;

            ctx->thread_ctx.work_pos = THREAD_POS_SLEEPING;

            syn_ctrl_sleep();
        }
        while (thread_unid == ctx->thread_ctx.unid &&
               ctx->thread_ctx.state == THREAD_STA_STOP) {
            os_msleep(10); /* 10 :a number */
        }
    }

    if (thread_unid == ctx->thread_ctx.unid) {
        syn_ctrl_clean_thread();
    } else {
        dprint(PRN_ERROR, "syn thread unid changed!\n");
    }

    return ret;
}

STATIC td_s32 syn_ctrl_manage_thread(syn_thread_cmd cmd)
{
    td_s32 ret = SYN_OK;
    syn_ctrl_ctx *ctx = TD_NULL;

    vfmw_check_ret(os_get_unid != TD_NULL, SYN_ERR);
    vfmw_check_ret(os_create_thread != TD_NULL, SYN_ERR);

    syn_ctrl_get_ctx_(ctx);

    ctx->thread_ctx.cmd = cmd;

    switch (cmd) {
        case THREAD_CMD_START:
            ctx->thread_ctx.state = THREAD_STA_START;
            break;

        case THREAD_CMD_STOP:
            ctx->thread_ctx.state = THREAD_STA_STOP;
            break;

        case THREAD_CMD_CREATE:
            ctx->thread_ctx.unid = os_get_unid();
            ctx->thread_ctx.state = THREAD_STA_START;
            ctx->thread_ctx.work_pos = THREAD_POS_WORKING;
            ctx->thread_ctx.run_cnt = 0;
            ctx->thread_ctx.last_run_time = 0;
            ctx->thread_ctx.thread_max_cost_time = 0;
            ctx->thread_ctx.thread = os_create_thread(syn_ctrl_thread_func, TD_NULL, VFMW_SYN_THREAD_NAME);
            vfmw_check_ret(ctx->thread_ctx.thread != TD_NULL, SYN_ERR);
            ctx->thread_ctx.sleep_time = ctx->sleep_time;
            ctx->thread_ctx.wait_time = ctx->wait_time;
            vdec_record_chr_event(-1, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_SYN_THREAD_OK);
            break;

        case THREAD_CMD_DELETE:
            ctx->thread_ctx.state = THREAD_STA_EXIT;
            os_event_give(ctx->thread_ctx.event);
            break;

        default:
            break;
    }

    return ret;
}

/* extern func */
td_s32 syn_ctrl_open(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    td_s32 chan_id;

    syn_ctrl_get_ctx_(ctx);
    vfmw_check_ret(ctx != TD_NULL, SYN_ERR);
    vfmw_check_ret(os_event_init != TD_NULL, SYN_ERR);
    vfmw_check_sec_func(memset_s(ctx, sizeof(syn_ctrl_ctx), 0, sizeof(syn_ctrl_ctx)));

    ctx->is_block_cmd = TD_TRUE; /*  support block cmd, others no block */
    ctx->is_chan_state_changed = TD_FALSE;
    ctx->created_chan_num = 0;
    ctx->started_chan_num = 0;
    ctx->sleep_time = 5; /* 5 num */
    ctx->wait_time = 0;
    ctx->ctx_size = (td_s32)sizeof(syn_ctrl_ctx);
    ctx->bypass_chan = -1;

    os_sema_init(&ctx->sema);

    /* Thread event */
    os_event_init(&ctx->thread_ctx.event, 1);

    /* all chan sema */
    for (chan_id = 0; chan_id < VFMW_CHAN_NUM; chan_id++) {
        os_sema_init(&ctx->chan_ctx[chan_id].sema);
    }

    syn_parse_open();
    syn_ops_open();

    return SYN_OK;
}

td_s32 syn_ctrl_close(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    td_s32 chan_id;

    syn_ctrl_get_ctx_(ctx);

    os_event_exit(ctx->thread_ctx.event);

    for (chan_id = 0; chan_id < VFMW_CHAN_NUM; chan_id++) {
        os_sema_exit(ctx->chan_ctx[chan_id].sema);
    }

    ctx->sleep_time = 0;
    ctx->wait_time = 0;

    os_sema_exit(ctx->sema);

    syn_parse_close();
    syn_ops_close();

    return SYN_OK;
}

td_s32 syn_ctrl_suspend(td_void)
{
    if (syn_ctrl_manage_thread(THREAD_CMD_STOP) == SYN_ERR) {
        dprint(PRN_FATAL, "%s syn thread stop error!\n", __func__);
    }
    vdec_record_chr_event(-1, MT_VDEC_SUSPEND, MT_VDEC_VFMW_SUSPEND, MT_VDEC_VFMW_SYN_SUSPEND_OK);

    return SYN_OK;
}

td_s32 syn_ctrl_resume(td_void)
{
    if (syn_ctrl_manage_thread(THREAD_CMD_START) == SYN_ERR) {
        dprint(PRN_FATAL, "%s syn thread start error!\n", __func__);
    }
    vdec_record_chr_event(-1, MT_VDEC_RESUME, MT_VDEC_VFMW_RESUME, MT_VDEC_VFMW_SYN_RESUME_OK);

    return SYN_OK;
}

td_s32 syn_ctrl_create_chan(td_s32 chan_id)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;

    vfmw_check_ret(os_get_unid != TD_NULL, SYN_ERR);

    syn_ctrl_get_ctx_(ctx);
    chan = &(ctx->chan_ctx[chan_id]);

    os_sema_down(ctx->sema);
    /* Create Thread at first chan */
    if (ctx->created_chan_num == 0) {
        if (ctx->thread_ctx.thread != TD_NULL) {
            ctx->thread_ctx.unid = 0;
            ctx->thread_ctx.run_cnt = 0;
            ctx->thread_ctx.last_run_time = 0;
            ctx->thread_ctx.thread_max_cost_time = 0;
            ctx->thread_ctx.state = THREAD_STA_BUTT;
            ctx->thread_ctx.thread = TD_NULL;
        }

        syn_ctrl_manage_thread(THREAD_CMD_CREATE);
    }
    ctx->created_chan_num++;
    os_sema_up(ctx->sema);

    os_sema_down(chan->sema);
    chan->vid_std = -1;
    chan->priority = 1;
    chan->chan_id = chan_id;
    chan->unid = os_get_unid();
    chan->creat_dps_num = 0;
    chan->be_taken_dps_num = 0;
    chan->get_dps_try_cnt = 0;
    chan->get_dps_succ_cnt = 0;
    chan->get_dps_fail_cnt = 0;
    chan->be_taken_dps_fail_cnt = 0;
    chan->give_back_dps_num = 0;
    chan->get_seg_num = 0;
    chan->get_seg_try_cnt = 0;
    chan->get_seg_succ_cnt = 0;
    chan->get_seg_fail_cnt = 0;
    chan->one_dps_cost_max_time = 0;
    chan->dps_consume_max_seg_num = 0;
    chan->last_consume_seg_pos = 0;

    chan->parse_result.is_need_seg = TD_TRUE;
    chan->parse_result.is_need_dps = TD_TRUE;
    chan->read_seg = stm_recvive_seg;

    chan->state = SYN_CHAN_CREATE_DONE;
    chan->curr_dps = TD_NULL;
    chan->block_time = 0;
    os_sema_up(chan->sema);
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_SYN_OK);

    return SYN_OK;
}

td_s32 syn_ctrl_config_chan(td_s32 chan_id, const td_void *cfg)
{
    const syn_cfg_info *cfg_info = (syn_cfg_info *)cfg;
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;

    vfmw_check_ret(cfg_info != TD_NULL, SYN_ERR);

    syn_ctrl_get_ctx_(ctx);
    chan = &(ctx->chan_ctx[chan_id]);

    os_sema_down(chan->sema);
    if (chan->vid_std != cfg_info->vid_std) {
        syn_parse_deinit_ctx(chan_id);
    }

    syn_parse_init_ctx(chan_id, cfg);
    chan->priority = cfg_info->priority;
    chan->vid_std = cfg_info->vid_std;
    chan->lowdly = cfg_info->lowdly_en;
    chan->adpt_type = cfg_info->adpt_type;
    if (chan->state < SYN_CHAN_CONFIG_DONE) {
        chan->state = SYN_CHAN_CONFIG_DONE; /* set state to config only at first config */
    }
    os_sema_up(chan->sema);

    return SYN_OK;
}

td_s32 syn_ctrl_cfg_ext_intf(td_s32 chan_id, const td_void *cfg)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;
    const syn_ext_intf *ext_cfg = (syn_ext_intf *)cfg;

    vfmw_check_ret(ext_cfg != TD_NULL, SYN_ERR);

    syn_ctrl_get_ctx_(ctx);
    chan = &(ctx->chan_ctx[chan_id]);

    os_sema_down(chan->sema);
    chan->read_seg = ext_cfg->read_seg;
    os_sema_up(chan->sema);

    return SYN_OK;
}

td_s32 syn_ctrl_get_info(td_s32 chan_id, td_void *args)
{
    td_s32 ret;
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan_ctx = TD_NULL;
    syn_syntax_info *info = (syn_syntax_info *)args;
    vfmw_check_ret(info != TD_NULL, SYN_ERR);

    syn_ctrl_get_ctx_(ctx);

    chan_ctx = &ctx->chan_ctx[chan_id];

    info->get_seg_try_cnt = chan_ctx->get_seg_try_cnt;
    info->get_seg_succ_cnt = chan_ctx->get_seg_succ_cnt;

    ret = syn_parse_get_info(chan_id, args);
    if (ctx->syn_spec_invalid == 1) {
        info->spec_invalid = SPEC_WIDTH_INVALID;
    }

    return ret;
}

td_s32 syn_ctrl_cfg_fffb_speed(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;

    ret = syn_parse_cfg_fffb_speed(chan_id, args);

    return ret;
}

td_s32 syn_ctrl_cfg_pvr_info(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;

    ret = syn_parse_cfg_pvr_info(chan_id, args);

    return ret;
}

td_s32 syn_ctrl_start_chan(td_s32 chan_id)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;

    syn_ctrl_get_ctx_(ctx);
    chan = &(ctx->chan_ctx[chan_id]);

    if (syn_ctrl_manage_thread(THREAD_CMD_START) == SYN_ERR) {
        dprint(PRN_FATAL, "%s syn thread start error!\n", __func__);
    }

    os_sema_down(chan->sema);
    chan->state = SYN_CHAN_START_DONE;
    os_sema_up(chan->sema);

    os_sema_down(ctx->sema);
    ctx->is_chan_state_changed = TD_TRUE;
    os_sema_up(ctx->sema);

    return SYN_OK;
}

td_s32 syn_ctrl_stop_chan(td_s32 chan_id)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;

    syn_ctrl_get_ctx_(ctx);
    chan = &(ctx->chan_ctx[chan_id]);

    os_sema_down(chan->sema);
    chan->state = SYN_CHAN_STOP_DONE;
    os_sema_up(chan->sema);

    os_sema_down(ctx->sema);
    ctx->is_chan_state_changed = TD_TRUE;
    os_sema_up(ctx->sema);

    return SYN_OK;
}

td_s32 syn_ctrl_clean_thread(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_ctrl_get_ctx_(ctx);

    if (ctx->created_chan_num > 0) {
        return SYN_OK;
    }

    if (ctx->thread_ctx.thread == TD_NULL) {
        return SYN_OK;
    }

    ctx->thread_ctx.unid = 0;
    ctx->thread_ctx.run_cnt = 0;
    ctx->thread_ctx.last_run_time = 0;
    ctx->thread_ctx.thread_max_cost_time = 0;
    ctx->thread_ctx.state = THREAD_STA_BUTT;
    ctx->thread_ctx.thread = TD_NULL;

    return SYN_OK;
}

td_s32 syn_ctrl_destroy_chan(td_s32 chan_id)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;
    td_void *handle = TD_NULL;

    syn_ctrl_get_ctx_(ctx);
    chan = &(ctx->chan_ctx[chan_id]);

    os_sema_down(chan->sema);

    syn_parse_deinit_ctx(chan_id);

    chan->state = SYN_CHAN_NONE;

    os_sema_up(chan->sema);

    os_sema_down(ctx->sema);
    ctx->created_chan_num--;

    /* Delete Thread at last chan */
    if (ctx->created_chan_num == 0) {
        ctx->is_chan_state_changed = TD_FALSE;
        handle = ctx->thread_ctx.thread;
        syn_ctrl_manage_thread(THREAD_CMD_DELETE);
        ctx->thread_ctx.work_pos = THREAD_POS_WAIT_DESTROY;
        while (ctx->thread_ctx.thread != TD_NULL) {
            os_msleep(VFMW_THREAD_WAIT_EXIT_TIME);
        }
        ctx->thread_ctx.work_pos = THREAD_POS_DESTROY_DONE;
        os_destroy_thread(handle, TD_FALSE);
        handle = TD_NULL;
    } else {
        ctx->is_chan_state_changed = TD_TRUE;
    }

    chan->curr_dps = TD_NULL;

    os_sema_up(ctx->sema);
    vdec_record_chr_event(chan_id, MT_VDEC_DESTROY, MT_VDEC_STORE_CLOSE, MT_VDEC_VFMW_SYN_THREAD_DESTROY_OK);

    return SYN_OK;
}

td_s32 syn_ctrl_reset_chan(td_s32 chan_id, td_void *args)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;

    syn_ctrl_get_ctx_(ctx);
    chan = &(ctx->chan_ctx[chan_id]);

    os_sema_down(chan->sema);

    syn_parse_reset_ctx(chan_id, args);

    chan->creat_dps_num = 0;
    chan->be_taken_dps_num = 0;
    chan->get_dps_try_cnt = 0;
    chan->get_dps_succ_cnt = 0;
    chan->get_dps_fail_cnt = 0;
    chan->be_taken_dps_fail_cnt = 0;
    chan->give_back_dps_num = 0;
    chan->get_seg_num = 0;
    chan->get_seg_try_cnt = 0;
    chan->get_seg_succ_cnt = 0;
    chan->get_seg_fail_cnt = 0;
    chan->one_dps_cost_max_time = 0;
    chan->dps_consume_max_seg_num = 0;
    chan->last_consume_seg_pos = 0;
    chan->parse_result.is_need_seg = TD_TRUE;
    chan->parse_result.is_need_dps = TD_TRUE;

    chan->state = SYN_CHAN_RESET_DONE;
    os_sema_up(chan->sema);

    os_sema_down(ctx->sema);
    ctx->is_chan_state_changed = TD_TRUE;
    os_sema_up(ctx->sema);

    return SYN_OK;
}

td_s32 syn_ctrl_wake_up(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;

    syn_ctrl_get_ctx_(ctx);

    if (ctx->thread_ctx.work_pos == THREAD_POS_SLEEPING || ctx->thread_ctx.work_pos == THREAD_POS_WAITING) {
        os_event_give(ctx->thread_ctx.event);
    }

    return SYN_OK;
}

syn_ctrl_ctx *syn_ctrl_get_ctx(td_void)
{
    syn_ctrl_ctx *ctx = TD_NULL;

    syn_ctrl_get_ctx_(ctx);

    return ctx;
}

td_s32 syn_ctrl_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    syn_parse_get_mem_need_size(mem);

    return SYN_OK;
}

td_void syn_ctrl_read_std_log(td_s32 chan_id, td_void *buf, td_s32 len, td_s32 *used)
{
    syn_parse_read_std_log(chan_id, buf, len, used);
}
