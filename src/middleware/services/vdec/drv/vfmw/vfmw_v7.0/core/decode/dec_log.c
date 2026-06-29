/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dec_log.h"
#include "dec_base.h"
#include "dec_intf.h"
#include "dec_chn.h"
#include "dec_dev.h"
#include "dec_thread.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include "fsp.h"

STATIC td_void dec_log_read_cfg_task(td_void *buf, dec_chan *chan, td_s32 len, td_s32 *used)
{
    td_s32 i;
    td_s32 task_id;
    td_s32 dps_id;
    dec_task_state task_state;
    dec_chan_task *task = TD_NULL;
    dec_chan_task_pool *task_pool = TD_NULL;
    td_char *task_state_str[DEC_TASK_MAX + 1] = { "Idle", "DpsErr", "Start", "Split", "Ref", "Mem",
                                                  "Msg", "Dec", "Temp", "Perm", "Bsp", "Pxp", "NA"
                                                };

    vfmw_check(buf != TD_NULL);
    vfmw_check(chan != TD_NULL);
    vfmw_check(used != TD_NULL);

    task_pool = &chan->chan_task_pool;

    os_dump_proc(buf, len, used, 0, LOG_CTL_FMT_HEAD);
    for (i = 0; i < (td_s32)task_pool->cfg_task_num; i++) {
        task_id = ((td_s32)task_pool->next_cfg_task + i) % DEC_MAX_TASK_NUM;
        task = &(task_pool->task[task_id]);
        task_state = (task->state < DEC_TASK_MAX) ? task->state : DEC_TASK_MAX;
        dps_id = task->dps == TD_NULL ? -1 : task->dps->dps_id;
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "task_id", task_id, " ");
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "dps_id", dps_id, "state", task_state_str[task_state]);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dev_id", task->dev_id, "msg_id", task->msg_id);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "reg_id", task->reg_id, "cfg_id", task->cfg_id);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "sec_en", task->mem_param.vdh_safe_flag, "linear_en",
                     task->mem_param.vdh_2d_en);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "cmp_en", task->mem_param.compress_en, "low_delay_en",
                     task->mem_param.low_delay_en);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "mmu_en", task->mem_param.vdh_mmu_en, "pxp_in_ram",
                     task->mem_param.pxp_in_ram);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "bsp_in_ram", task->mem_param.bsp_in_ram, " ");
    }
    if (task_pool->cfg_task_num == 0) {
        os_dump_proc(buf, len, used, 0, "cfg_task_num is 0!%61s|\n", " ");
    }
}

STATIC td_void dec_log_read_dec_task(td_void *buf, dec_chan *chan, td_s32 len, td_s32 *used)
{
    td_s32 i;
    td_s32 task_id;
    td_s32 dps_id;
    dec_task_state task_state;
    dec_chan_task *task = TD_NULL;
    dec_chan_task_pool *task_pool = TD_NULL;
    td_char *task_state_str[DEC_TASK_MAX + 1] = { "Idle", "DpsErr", "Start", "Split", "Ref", "Mem",
                                                  "Msg", "Dec", "Temp", "Perm", "Bsp", "Pxp", "NA"
                                                };
    vfmw_check(chan != TD_NULL);

    task_pool = &chan->chan_task_pool;
    os_dump_proc(buf, len, used, 0, LOG_DTL_FMT_HEAD);

    for (i = 0; i < (td_s32)task_pool->dec_task_num; i++) {
        task_id = ((td_s32)task_pool->next_pxp_task + i) % DEC_MAX_TASK_NUM;
        task = &(task_pool->task[task_id]);
        task_state = (task->state < DEC_TASK_MAX) ? task->state : DEC_TASK_MAX;
        dps_id = task->dps == TD_NULL ? -1 : task->dps->dps_id;

        os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "task_id", task_id, " ");
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "dps_id", dps_id, "state", task_state_str[task_state]);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dev_id", task->dev_id, "msg_id", task->msg_id);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "reg_id", task->reg_id, "cfg_id", task->cfg_id);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "sec_en", task->mem_param.vdh_safe_flag, "linear_en",
                     task->mem_param.vdh_2d_en);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "cmp_en", task->mem_param.compress_en, "low_delay_en",
                     task->mem_param.low_delay_en);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "mmu_en", task->mem_param.vdh_mmu_en, "pxp_in_ram",
                     task->mem_param.pxp_in_ram);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "bsp_in_ram", task->mem_param.bsp_in_ram, " ");
    }
    if (task_pool->dec_task_num == 0) {
        os_dump_proc(buf, len, used, 0, "dec_task_num is 0!%61s|\n", " ");
    }
}

STATIC td_void dec_log_read_chan(td_void *buf, td_s32 chan_id, td_s32 len, td_s32 *used)
{
    dec_chan *chan = TD_NULL;
    dec_chan_state chan_state;
    dec_chan_task_pool *task_pool = TD_NULL;
    td_char *chan_state_str[DEC_CHAN_STATE_MAX + 1] = { "Null", "Suspend", "Run", "Cancel", "NA" };

    os_dump_proc(buf, len, used, 0, LOG_CHN_FMT_HEAD);
    chan = dec_ctrl_get_chan(chan_id);
    if (chan == TD_NULL || chan->state == DEC_CHAN_STATE_NULL) {
        os_dump_proc(buf, len, used, 0, "dec chan %d not exists.%-57s|\r\n", chan_id, " ");
        os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
        return;
    }
    task_pool = &chan->chan_task_pool;

    chan_state = (chan->state < DEC_CHAN_STATE_MAX) ? chan->state : DEC_CHAN_STATE_MAX;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "chan_id", chan->chan_id, "state",
                 chan_state_str[chan_state]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_P, "unid", chan->unid, "handle", chan);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_U, "sec_flag", chan->chan_cfg.sec_flag, "run_cnt",
                 chan->run_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "priority", chan->chan_cfg.priority, "vid_std",
                 dbg_show_std(chan->chan_cfg.vid_std, TD_NULL));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "cmp_mode", chan->chan_cfg.vcmp_mode, "low_delay_en",
                 chan->chan_cfg.low_delay_en);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "repair_en", chan->chan_cfg.repair_en, "repair_thr",
                 chan->chan_cfg.repair_en);

    os_dump_proc(buf, len, used, 0, LOG_FMT_U_U, "dps_try_cnt", chan->dps_try_cnt, "mem_try_cnt", chan->mem_try_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_U_U, "dps_succ_cnt", chan->dps_succ_cnt, "mem_succ_cnt",
                 chan->mem_succ_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_U_U, "rcv_dps_num", chan->rcv_dps_num, "dec_dps_num", chan->dec_dps_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_U_U, "cfg_task_num", task_pool->cfg_task_num, "dec_task_num",
                 task_pool->dec_task_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "next_pxp_task", task_pool->next_pxp_task, "next_bsp_task",
                 task_pool->next_bsp_task);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "next_cfg_task", task_pool->next_cfg_task, "dec_err_num",
                 chan->err_frame_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "air_dec", chan->chan_cfg.air_version, "uv_order_en",
                 chan->chan_cfg.uv_order_en);

    dec_log_read_dec_task(buf, chan, len, used);

    dec_log_read_cfg_task(buf, chan, len, used);
    os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
}

STATIC td_void dec_log_read_vdh(td_s32 chan_id, dec_dev_info *dev, td_void *buf, td_s32 len, td_s32 *used)
{
    td_s32 dec_width = 0;
    td_s32 dec_height = 0;
    td_s32 bsp_cycle = 0;
    td_u32 tick = 0;
    td_s32 pxp_cycle = 0;
    dec_dev_vdh *vdh = TD_NULL;

    vdh = (dec_dev_vdh *)dev->handle;
    if (fsp_get_pic_wh(chan_id, &dec_height, &dec_width, &tick) == FSP_OK) {
        pxp_cycle = (dec_height != 0 && dec_width != 0) ?
            (td_s32)dev->avg_pxp_cycle / ((dec_height / 16) * (dec_width / 16)) : pxp_cycle; /* 16 :a number */
    }
    os_dump_proc(buf, len, used, 0, LOG_FMT_P_U, "handle", vdh, "reg_cfg_cnt", vdh->reg_cfg_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_U_U, "bsp_int_cnt", vdh->bsp_int_cnt, "pxp_int_cnt", vdh->pxp_int_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "reg_num", vdh->vdh_reg_pool.reg_num, "next_cfg_reg",
                 vdh->vdh_reg_pool.next_cfg_reg);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "next_bsp_reg", vdh->vdh_reg_pool.next_bsp_reg, "next_pxp_reg",
                 vdh->vdh_reg_pool.next_pxp_reg);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "msg_num", vdh->vdh_msg_pool.msg_num, "next_cfg_msg",
                 vdh->vdh_msg_pool.next_cfg_msg);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "bsp_cycle(/mb)", bsp_cycle, "pxp_cycle(/mb)", pxp_cycle);
}

STATIC td_void dec_log_read_dev(td_s32 chan_id, td_void *buf, td_s32 len, td_s32 *used)
{
    td_u32 i, integer, decimal;
    dec_dev_info *dev = TD_NULL;
    dec_dev_mdma *mdma = TD_NULL;
    dec_dev_state state;
    dec_dev_type type;
    td_char *state_str[DEC_DEV_STATE_MAX + 1] = { "Null", "Run", "Suspend", "Stop", "NA" };
    td_char *type_str[DEC_DEV_TYPE_MAX + 1] = { "Null", "VDH", "MDMA", "NA" };

    for (i = 0; i < DEC_DEV_NUM; i++) {
        os_dump_proc(buf, len, used, 0, LOG_DEV_X_FMT_HEAD, i);
        dev = dec_ctrl_get_dev(i);
        if (dev == TD_NULL || dev->handle == TD_NULL) {
            os_dump_proc(buf, len, used, 0, "dec device %d not opens.%-56s|\r\n", i, " ");
            continue;
        }

        state = (dev->state < DEC_DEV_STATE_MAX) ? dev->state : DEC_DEV_STATE_MAX;
        type = (dev->type < DEC_DEV_TYPE_MAX) ? dev->type : DEC_DEV_TYPE_MAX;

        integer = dev->ratio / 10; /* 10 :a number */
        decimal = dev->ratio % 10; /* 10 :a number */

        os_dump_proc(buf, len, used, 0, LOG_FMT_S_N, "type", type_str[type], " ");
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "dev_id", dev->dev_id, "state", state_str[state]);
        os_dump_proc(buf, len, used, 0, LOG_FMT_U_U, "cfg_frm_cnt", dev->cfg_frm_cnt, "dec_frm_cnt", dev->dec_frm_cnt);
        os_dump_proc(buf, len, used, 0, "%-19s:%-19d|%-19s:%03d.%-15d|\r\n", "isr_max_cost(us)", dev->isr_max_cost_time,
            "work_load", integer, decimal);

        if (dev->type == DEC_DEV_TYPE_VDH) {
            dec_log_read_vdh(chan_id, dev, buf, len, used);
        } else if (dev->type == DEC_DEV_TYPE_MDMA) {
            mdma = (dec_dev_mdma *)dev->handle;
            os_dump_proc(buf, len, used, 0, LOG_FMT_P_U, "handle", mdma, "reg_cfg_cnt", mdma->reg_cfg_cnt);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_U, "handle", mdma->chan_id, "int_cnt", mdma->int_cnt);
        } else {
            os_dump_proc(buf, len, used, 0, "dec device %d type %d invalid.%-51s|\r\n", i, dev->type, " ");
            continue;
        }
    }
}

STATIC td_void dec_log_read_chan_pool(td_void *buf, td_s32 len, td_s32 *used)
{
    dec_chan_pool *chn_pool = TD_NULL;
    td_u32 i;

    chn_pool = dec_ctrl_get_chan_pool();
    vfmw_check(chn_pool != TD_NULL);

    os_dump_proc(buf, len, used, 0, LOG_CHN_POOL_FMT_HEAD);

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "start_num", chn_pool->started_chan_num,
        "curr_chn", chn_pool->curr_high_prior_chan);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "pri_tab_index", chn_pool->chan_pri_tab_index, " ");

    for (i = 0; i < DEC_MAX_CHAN_NUM; i++) {
        if (chn_pool->chan_pri_tab[i][1] != 0) {
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chan", chn_pool->chan_pri_tab[i][0],
                "pri", chn_pool->chan_pri_tab[i][1]);
        }
    }
}

STATIC td_void dec_log_read_entry(td_void *buf, td_s32 len, td_s32 *used)
{
    dec_entry *entry = TD_NULL;
    dec_thread *thread = TD_NULL;
    dec_thread_cmd cmd_id;
    dec_thread_state state_id;
    td_char *cmd_str[DEC_THREAD_CMD_BUTT + 1] = { "Null", "Start", "Stop", "Destroy", "NA" };
    td_char *state_str[DEC_THREAD_STATE_BUTT + 1] = { "Null", "Run", "Wait", "Suspend", "NA" };

    entry = dec_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);

    os_dump_proc(buf, len, used, 0, LOG_DEC_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_X, "check1", entry->dec_check_0, "check2",
                 entry->dec_check_1);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "check_interval", entry->check_interval, "bypass_chan",
                 entry->bypass_chan);

    thread = dec_ctrl_get_thread(0);
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "dec thread not exists.%-57s|\r\n", " ");
        return;
    }

    cmd_id = (thread->cmd < DEC_THREAD_CMD_BUTT) ? thread->cmd : DEC_THREAD_CMD_BUTT;
    state_id = (thread->state < DEC_THREAD_STATE_BUTT) ? thread->state : DEC_THREAD_STATE_BUTT;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "thread", thread->id, " ");
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_S, "cmd", cmd_str[cmd_id], "state", state_str[state_id]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_U, "runPos", thread->run_pos, "run_cnt", thread->run_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_P, "time_out(ms)", thread->time_out, "handle", thread->handle);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "max_time(us)", thread->cost_max_time, " ");

    dec_log_read_chan_pool(buf, len, used);
}

td_s32 dec_log_read(td_void *buf, td_s32 chan_id, td_s32 len)
{
    td_s32 used = 0;
    dec_entry *entry = TD_NULL;
    entry = dec_ctrl_get_entry();

    os_sema_down(entry->sema);
    dec_log_read_entry(buf, len, &used);
    dec_log_read_dev(chan_id, buf, len, &used);
    dec_log_read_chan(buf, chan_id, len, &used);
    os_sema_up(entry->sema);

    return DEC_OK;
}

STATIC td_void dec_log_thread_sleep(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 sleep_time = 0;
    dec_entry *entry = TD_NULL;
    dec_thread *thread = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &sleep_time); /* 1 means 2nd param */
    vfmw_check_print(sleep_time <= LOG_MAX_SLEEP_TIME,
        "sleep_time %d out of range[0,%d]\n", sleep_time, LOG_MAX_SLEEP_TIME);

    entry = dec_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);
    entry->sleep_time = sleep_time;

    thread = dec_ctrl_get_thread(0);
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        dprint(PRN_ALWS, "dec thread not exist, entry->sleep_time %d\n", entry->sleep_time);
    } else {
        thread->sleep_time = sleep_time;
        dprint(PRN_ALWS, "Set dec thread sleep_time %d\n", sleep_time);
    }
}

STATIC td_void dec_log_set_check_interval(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 interval_time_ms = 0;
    dec_entry *entry = TD_NULL;
    dec_thread *thread = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &interval_time_ms); /* 1 means 2nd param */

    entry = dec_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);
    entry->check_interval = interval_time_ms;

    thread = dec_ctrl_get_thread(0);
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        dprint(PRN_ALWS, "dec thread not exist, entry->check_interval %d\n", entry->check_interval);
    } else {
        thread->check_interval = interval_time_ms;
        dprint(PRN_ALWS, "set dec thread check_interval %d\n", interval_time_ms);
    }
}

STATIC td_void dec_log_set_bypass(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 chn_id = 0;
    dec_entry *entry = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &chn_id); /* 1 means 2nd param */
    vfmw_check_print((chn_id < VFMW_CHAN_NUM || chn_id == VFMW_MAX_UINT32), "chn_id %d invalid.\n", chn_id);

    entry = dec_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);

    entry->bypass_chan = (td_s32)chn_id;
    dprint(PRN_ALWS, "set dec bypass chan %d\n", entry->bypass_chan);
}

STATIC td_void dec_log_thread_wait(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 wait_time = 0;
    dec_entry *entry = TD_NULL;
    dec_thread *thread = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &wait_time); /* 1 means 2nd param */
    vfmw_check_print(wait_time <= LOG_MAX_WAIT_TIME, "wait_time %d out of range[0,%d]\n", wait_time, LOG_MAX_WAIT_TIME);

    entry = dec_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);
    entry->wait_time = wait_time;

    thread = dec_ctrl_get_thread(0);
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        dprint(PRN_ALWS, "dec thread not exist, entry->wait_time %d\n", entry->wait_time);
    } else {
        thread->wait_time = wait_time;
        dprint(PRN_ALWS, "set dec thread wait_time %d\n", wait_time);
    }
}

td_s32 dec_log_write(cmd_str_ptr buffer, td_u32 count)
{
    td_s32 n, i;
    cmd_handler handler = TD_NULL;
    td_char *cmd_id = (td_char *)((*buffer)[0]);

    STATIC const log_cmd com_cmd[] = {
        {LOG_CMD_DEC_SLEEP, dec_log_thread_sleep},
        {LOG_CMD_DEC_WAIT, dec_log_thread_wait},
        {LOG_CMD_DEC_CHECK, dec_log_set_check_interval},
        {LOG_CMD_DEC_BYPASS, dec_log_set_bypass},
    };

    n = (td_s32)(sizeof(com_cmd) / sizeof(log_cmd));
    vfmw_check_ret(os_strncmp != TD_NULL, DEC_ERR);
    vfmw_check_ret(os_strlen != TD_NULL, DEC_ERR);

    for (i = 0; i < n; i++) {
        if (os_strncmp(cmd_id, com_cmd[i].cmd_name, os_strlen(cmd_id)) == 0) {
            handler = com_cmd[i].handler;
            break;
        }
    }

    if (handler == TD_NULL) {
        dprint(PRN_ALWS, "Unsupport command.\n");
        return DEC_ERR;
    }

    handler(buffer, count);

    return DEC_OK;
}
