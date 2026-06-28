/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "syn_log.h"
#include "syn_ctrl.h"
#include "syn_parse.h"

STATIC td_void syn_log_read_std(td_s32 chan_id, td_void *buf, td_s32 len, td_s32 *used)
{
    os_dump_proc(buf, len, used, 0, LOG_STD_FMT_HEAD);

    syn_ctrl_read_std_log(chan_id, buf, len, used);

    return;
}

STATIC td_void syn_log_read_chn(td_void *buf, td_s32 chan_id, td_s32 len, td_s32 *used)
{
    syn_ctrl_ctx *ctx = TD_NULL;
    syn_chan *chan = TD_NULL;
    syn_chan_state sta_id;
    td_u32 i;
    td_s32 dps_id;
    td_char *sta_str[SYN_CHAN_BUTT + 1] = { "None", "Create", "Config", "Start", "Stop", "Reset", "NA" };
    td_char *seg_type_str[STM_SEG_END] = { "INVLD", "SLC", "PPS", "SPS", "VPS", "SEI", "EOP", "EOS" };

    os_dump_proc(buf, len, used, 0, LOG_CHN_FMT_HEAD);

    ctx = syn_ctrl_get_ctx();
    if (ctx == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "syn entry not exists.%-58s|\r\n", " ");
        os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
        return;
    }

    chan = &(ctx->chan_ctx[chan_id]);
    if (chan->state == SYN_CHAN_NONE) {
        os_dump_proc(buf, len, used, 0, "syn chan %d not exists.%-57s|\r\n", chan_id, " ");
        os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
        return;
    }

    os_sema_down(chan->sema);
    sta_id = (chan->state < SYN_CHAN_BUTT) ? chan->state : SYN_CHAN_BUTT;
    dps_id = (chan->curr_dps != TD_NULL) ? chan->curr_dps->dps_id : -1;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "chan_id", chan->chan_id, "state", sta_str[sta_id]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_S, "unid", chan->unid, "vid_std",
                 dbg_show_std(chan->vid_std, TD_NULL));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "priority", chan->priority, "is_need_seg",
                 chan->parse_result.is_need_seg);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "is_need_dps", chan->parse_result.is_need_dps, " ");
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "get_seg_try", chan->get_seg_try_cnt, "get_dps_try",
                 chan->get_dps_try_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "get_seg_suc", chan->get_seg_succ_cnt, "get_dps_suc",
                 chan->get_dps_succ_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "cur_dps_id", dps_id, "create_dps_num", chan->creat_dps_num);

    if (dcheck(PRN_DBG)) {
        os_dump_proc(buf, len, used, 0, LOG_PCTI_FMT_HEAD);
        os_dump_proc(buf, len, used, 0, "%-14s %-64s|\n", "time(us)", "stype");
        for (i = 0; i < SYN_DEBUG_CNT; i++) {
            os_dump_proc(buf, len, used, 0, "%-14d %-64s|\n", chan->parse_cost_time[i],
                seg_type_str[chan->seg_type[i]]);
        }
        os_dump_proc(buf, len, used, 0, LOG_PCMT_FMT_HEAD);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "parse_cost_maxtime", chan->parse_cost_max_time, " ");
    }

    syn_log_read_std(chan_id, buf, len, used);
    os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
    os_sema_up(chan->sema);
}

STATIC td_void syn_log_read_entry(td_void *buf, td_s32 len, td_s32 *used)
{
    syn_thread_status sta_id;
    syn_thread_cmd cmd_id;
    syn_thread_work_pos pos_id;
    syn_ctrl_ctx *ctx = TD_NULL;
    td_char *sta_str[THREAD_STA_BUTT + 1] = { "Exit", "Start", "Stop", "NA" };
    td_char *cmd_str[THREAD_CMD_BUTT + 1] = { "Create", "Delete", "Start", "Stop", "NA" };
    td_char *pos_str[THREAD_POS_BUTT + 1] = { "Waiting", "Working", "Sleeping", "NA" };

    os_dump_proc(buf, len, used, 0, LOG_VFMW_SYN_FMT_HEAD);

    ctx = syn_ctrl_get_ctx();
    if (ctx == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "syn entry not exists.%-58s|\r\n", " ");
        return;
    }

    os_sema_down(ctx->sema);
    sta_id = (ctx->thread_ctx.state < THREAD_STA_BUTT) ? ctx->thread_ctx.state : THREAD_STA_BUTT;
    cmd_id = (ctx->thread_ctx.cmd < THREAD_CMD_BUTT) ? ctx->thread_ctx.cmd : THREAD_CMD_BUTT;
    pos_id = (ctx->thread_ctx.work_pos < THREAD_POS_BUTT) ? ctx->thread_ctx.work_pos : THREAD_POS_BUTT;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "started_chan_num", ctx->started_chan_num, "created_chan_num",
                 ctx->created_chan_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "high_prior_chan", ctx->curr_high_prior_chan, "is_state_changed",
                 ctx->is_chan_state_changed);
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_S, "thread_state", sta_str[sta_id], "thread_cmd", cmd_str[cmd_id]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_D, "thread_pos", pos_str[pos_id], "thread_run_cnt",
                 ctx->thread_ctx.run_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "max_cost(us)", ctx->thread_ctx.thread_max_cost_time,
        "bypass_chan_id", ctx->bypass_chan);
    os_sema_up(ctx->sema);
}

STATIC td_void syn_log_thread_sleep(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 sleep_time = 0;
    syn_ctrl_ctx *ctx = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &sleep_time); /* 1 means get 2nd param */
    vfmw_check_print(sleep_time <= LOG_MAX_SLEEP_TIME,
        "sleep_time %d out of range[0,%d]\n", sleep_time, LOG_MAX_SLEEP_TIME);

    ctx = syn_ctrl_get_ctx();
    vfmw_check(ctx != TD_NULL);
    ctx->sleep_time = sleep_time;

    if (ctx->thread_ctx.thread == TD_NULL) {
        dprint(PRN_ALWS, "syn thread is not exist, pctx->sleep_time %d\n", sleep_time);
    } else {
        ctx->thread_ctx.sleep_time = sleep_time;
        dprint(PRN_ALWS, "set syn thread sleep_time: %d\n", sleep_time);
    }
}

STATIC td_void syn_log_thread_wait(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 wait_time = 0;
    syn_ctrl_ctx *ctx = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &wait_time); /* 1 means get 2nd param */
    vfmw_check_print(wait_time <= LOG_MAX_WAIT_TIME, "wait_time %d out of range[0,%d]\n", wait_time, LOG_MAX_WAIT_TIME);

    ctx = syn_ctrl_get_ctx();
    vfmw_check(ctx != TD_NULL);
    ctx->wait_time = wait_time;

    if (ctx->thread_ctx.thread == TD_NULL) {
        dprint(PRN_ALWS, "syn thread is not exist, pctx->wait_time %d\n", wait_time);
    } else {
        ctx->thread_ctx.wait_time = wait_time;
        dprint(PRN_ALWS, "set syn thread wait_time: %d\n", wait_time);
    }
}

STATIC td_void syn_log_set_bypass(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    syn_ctrl_ctx *ctx = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means get 2nd param */
    vfmw_check_print((data < VFMW_CHAN_NUM || data == VFMW_MAX_UINT32), "chn_id %d invalid.\n", data);

    ctx = syn_ctrl_get_ctx();
    vfmw_check(ctx != TD_NULL);

    ctx->bypass_chan = (td_s32)data;
    dprint(PRN_ALWS, "set syn bypass chan %d\n", ctx->bypass_chan);
}

STATIC td_void syn_log_set_no_recv_seg(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    syn_ctrl_ctx *ctx = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "syn_no_recv_seg %d out of range[0,1].\n", data);

    ctx = syn_ctrl_get_ctx();
    vfmw_check(ctx != TD_NULL);

    ctx->syn_no_recv_seg = data;
    dprint(PRN_ALWS, "set syn_no_recv_seg %d\n", ctx->syn_no_recv_seg);
}

STATIC td_void syn_log_handle_cmd(cmd_str_ptr cmd_str, td_u32 cmd_len)
{
    td_s32 i;
    td_s32 count;
    cmd_handler handler = TD_NULL;
    td_char *cmd_id = (td_char *)((*cmd_str)[0]);

    STATIC const log_cmd syn_cmd[] = {
        { LOG_CMD_SYN_SLEEP, syn_log_thread_sleep },
        { LOG_CMD_SYN_WAIT, syn_log_thread_wait },
        { LOG_CMD_NO_RECV_SEG, syn_log_set_no_recv_seg },
        { LOG_CMD_SYN_BYPASS, syn_log_set_bypass },
    };

    count = (td_s32)(sizeof(syn_cmd) / sizeof(log_cmd));
    vfmw_check(os_strncmp != TD_NULL);
    vfmw_check(os_strlen != TD_NULL);

    for (i = 0; i < count; i++) {
        if (os_strncmp(cmd_id, syn_cmd[i].cmd_name, os_strlen(cmd_id)) == 0) {
            handler = syn_cmd[i].handler;
            break;
        }
    }

    if (handler == TD_NULL) {
        dprint(PRN_ALWS, "unsupport command.\n");
        return;
    }

    handler(cmd_str, cmd_len);
}

td_s32 syn_log_read(td_void *buf, td_s32 chan_id, td_s32 len)
{
    td_s32 used = 0;

    vfmw_check_ret(buf != TD_NULL, EXT_FAILURE);

    syn_log_read_entry(buf, len, &used);
    syn_log_read_chn(buf, chan_id, len, &used);

    os_dump_proc(buf, len, &used, 0, "\n");

    return SYN_OK;
}

td_s32 syn_log_write(cmd_str_ptr buffer, td_u32 count)
{
    if (buffer == TD_NULL || count <= 0) {
        return SYN_ERR;
    }

    syn_log_handle_cmd(buffer, count);

    return SYN_OK;
}

