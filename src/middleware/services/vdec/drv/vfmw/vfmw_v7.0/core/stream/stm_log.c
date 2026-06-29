/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stm_log.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include "stm_base.h"
#include "stm_intf.h"
#include "stm_ctrl.h"
#include "stm_chan.h"
#include "stm_thread.h"
#include "stm_dev.h"

STATIC td_void stm_log_read_rawlist(td_void *buf, const stm_chan *chan, td_s32 len, td_s32 *used)
{
    raw_list_info *raw_list = TD_NULL;

    os_dump_proc(buf, len, used, 0, LOG_RAW_FMT_HEAD);
    raw_list = chan->raw_list;
    if (raw_list == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "stm chan %d raw_list not exists.%-48s|\r\n", chan->id, " ");
        return;
    }
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "fresh_num", raw_list->fresh_num,
                 "all_num", raw_list->raw_num_all);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "fresh_size", raw_list->fresh_size,
                 "all_size", raw_list->raw_size_all);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "push_num", raw_list->push_num,
                 "num_rate(n/s)", raw_list->num_rate_ops);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "push_len", raw_list->push_len,
                 "bit_rate(kbps)", raw_list->bitrate_kbps);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_U, "head", raw_list->head,
                 "cfg_dev_cnt", raw_list->cfg_dev_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "tail", raw_list->tail,
                 "last_start_time", raw_list->last_start_time);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_U, "history", raw_list->history,
                 "recv_try_cnt", raw_list->recv_try_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_U, "show_id", raw_list->show_id,
                 "recv_ok_cnt", raw_list->recv_ok_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "rels_try_cnt", raw_list->rels_try_cnt,
                 "rels_ok_cnt", raw_list->rels_ok_cnt);
}

STATIC td_void stm_log_read_seglist(td_void *buf, const stm_chan *chan, td_s32 len, td_s32 *used)
{
    seg_list *seg_list_info = TD_NULL;
    mpeg4_sub_std mpeg4_flag;
    td_char *mpeg4_str[SM_MPEG4_MAX + 1] = { "NOT_MPEG4", "MPEG4_NSH", "MPEG4_SH", "NA" };

    os_dump_proc(buf, len, used, 0, LOG_SEG_FMT_HEAD);
    seg_list_info = chan->seg_list_info;
    if (seg_list_info == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "stm chan %d seg_list_info not exists.%-43s|\r\n", chan->id, " ");
        return;
    }

    mpeg4_flag = (seg_list_info->mpeg4_mode_flag < SM_MPEG4_MAX) ? seg_list_info->mpeg4_mode_flag : SM_NOT_MPEG4;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "fresh_num", seg_list_info->seg_fresh_num,
                 "all_num", seg_list_info->seg_num_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "fresh_size", seg_list_info->seg_fresh_size,
                 "all_size", seg_list_info->seg_size_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_X, "seg_total_size", seg_list_info->seg_total_size,
                 "last_seg_addr", log_get_mask_vir_addr(seg_list_info->last_seg_addr));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "full_flag", seg_list_info->full_flag,
                 "last_seg_size", seg_list_info->last_seg_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_X,
                 "seg_write_addr", log_get_mask_vir_addr(seg_list_info->seg_buf_write_addr),
                 "seg_read_addr", log_get_mask_vir_addr(seg_list_info->seg_buf_read_addr));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_X, "head", seg_list_info->head,
                 "seg_buf_addr", log_get_mask_phy_addr(seg_list_info->seg_buf_phy_addr));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_X, "tail", seg_list_info->tail,
                 "seg_buf_size", seg_list_info->seg_buf_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "history", seg_list_info->history,
                 "roll_flag", seg_list_info->roll_flag);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "current", seg_list_info->seg_current,
                 "mpeg4_mode", mpeg4_str[mpeg4_flag]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_U_U, "recv_try_cnt", seg_list_info->recv_try_cnt,
                 "rels_try_cnt", seg_list_info->rels_try_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_U_U, "recv_ok_cnt", seg_list_info->recv_ok_cnt,
                 "rels_ok_cnt", seg_list_info->rels_ok_cnt);
}

STATIC td_void stm_log_read_chn(td_void *buf, td_s32 chan_id, td_s32 len, td_s32 *used)
{
    stm_chan *chan = TD_NULL;
    stm_chan_state state;
    stm_eos_state eos;
    td_char *eos_str[STM_EOS_MAX + 1] = { "wait", "recv", "insert", "parse", "done", "NA" };
    td_char *state_str[STM_CHAN_STATE_MAX + 1] = { "null", "run", "stop", "NA" };

    os_dump_proc(buf, len, used, 0, LOG_CHN_FMT_HEAD);
    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan == TD_NULL || chan->handle == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "stm chan %d not exists.%-57s|\r\n", chan_id, " ");
        os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
        return;
    }

    eos = (chan->eos < STM_EOS_MAX) ? chan->eos : STM_EOS_MAX;
    state = (chan->state < STM_CHAN_STATE_MAX) ? chan->state : STM_CHAN_STATE_MAX;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "chan_id", chan->id, "state", state_str[state]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_S, "unid", chan->unid, "eos", eos_str[eos]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "cfg_seg_size", chan->seg_buf_size, "vid_std",
                 dbg_show_std(chan->cfg.vid_std, &chan->cfg.std_ext));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "priority", chan->cfg.priority,
                 "dn_msg_num", chan->cfg.dn_msg_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "half_seg_en", chan->cfg.half_seg_en,
                 "fill_eop_en", chan->cfg.fill_eop_en);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "scd_bypass", chan->cfg.scd_bypass,
                 "seg_bypass", chan->cfg.seg_bypass);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "sec_flag", chan->cfg.sec_flag,
                 "is_an_test", chan->cfg.is_android_test);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "is_omx_path", 0,
                 "is_tunnel_path", chan->cfg.is_tunnel_path);

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "std_err", chan->std_err, " ");

    stm_log_read_rawlist(buf, chan, len, used);

    stm_log_read_seglist(buf, chan, len, used);
    os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
}

STATIC td_void stm_log_read_dev(td_void *buf, td_s32 len, td_s32 *used)
{
    stm_dev *dev = TD_NULL;
    stm_dev_state state_id;
    td_char *state_str[STM_DEV_STATE_MAX + 1] = { "null", "idle", "busy", "NA" };

    dev = stm_ctrl_get_dev(0);
    if (dev == TD_NULL || dev->handle == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "stm device not open.%-59s|\r\n", " ");
        return;
    }

    state_id = (dev->state < STM_DEV_STATE_MAX) ? dev->state : STM_DEV_STATE_MAX;

    os_dump_proc(buf, len, used, 0, LOG_DEV_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "dev_id", dev->id, "state", state_str[state_id]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "unid", dev->unid, "int_time", dev->int_time);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chan_id", dev->chan_id, "cancel", dev->cancel);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "int_count", dev->int_count, "start_count", dev->start_cnt);
}

STATIC td_void stm_log_read_entry(td_void *buf, td_s32 len, td_s32 *used)
{
    stm_entry *entry = TD_NULL;
    stm_thread *thread = TD_NULL;
    stm_thread_status cmd_id;
    stm_state state_id;
    stm_thread_status thread_state_id;
    td_char *state_str[STM_STATE_MAX + 1] = { "null", "run", "NA" };
    td_char *thread_str[STM_THREAD_BUTT + 1] = { "null", "create", "start", "stop", "destroy", "NA" };

    entry = stm_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);

    state_id = (entry->state < STM_STATE_MAX) ? entry->state : STM_STATE_MAX;

    os_dump_proc(buf, len, used, 0, LOG_VFMW_STM_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_S, "check1", entry->reserve1, "state", state_str[state_id]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "check2", entry->reserve2, "ref_cnt", entry->ref_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "ctx_size", entry->ctx_size, "hal_size", entry->hal_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "bypass_chan_id", entry->bypass_chan, " ");

    thread = stm_ctrl_get_thread();
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "stm thread not exists.%-57s|\r\n", " ");
        return;
    }

    cmd_id = (thread->cmd < STM_THREAD_BUTT) ? thread->cmd : STM_THREAD_BUTT;
    thread_state_id = (thread->state < STM_THREAD_BUTT) ? thread->state : STM_THREAD_BUTT;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "thread", thread->id, " ");
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_S, "cmd", thread_str[cmd_id], "state",
                 thread_str[thread_state_id]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_U, "run_pos", thread->pos, "run_cnt", thread->run_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "max_time(us)", thread->max_time, " ");
}

STATIC td_void stm_log_open_file(td_s32 chan_id, const td_char *name, td_u32 name_len, os_file *file)
{
    os_file file_info;

    vfmw_unused(chan_id);
    vfmw_unused(name_len);

    *file = TD_NULL;
    vfmw_check(os_fopen != TD_NULL);
    file_info = os_fopen(name, OS_RDWR | OS_CREATE | OS_TRUNC, OS_OPEN_MODE);
    if (file_info == TD_NULL) {
        dprint(PRN_ALWS, "open %s failed.\n", name);
        return;
    }

    *file = file_info;
}

STATIC td_void stm_log_close_file(td_s32 chan_id, os_file *file)
{
    vfmw_unused(chan_id);

    os_fclose(*file);
    *file = TD_NULL;
}

STATIC td_void stm_log_set_no_raw(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "stm_no_raw %d out of range[0,1].\n", data);

    entry->stm_no_raw = data;
    dprint(PRN_ALWS, "set stm no_raw %d\n", entry->stm_no_raw);
}

STATIC td_void stm_log_set_no_seg(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "stm_no_seg %d out of range[0,1].\n", data);

    entry->stm_no_seg = data;
    dprint(PRN_ALWS, "set stm no_seg %d\n", entry->stm_no_seg);
}

STATIC td_void stm_log_set_seg_full(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "stm_seg_full %d out of range[0,1].\n", data);

    entry->stm_seg_full = data;
    dprint(PRN_ALWS, "set stm seg_full %d\n", entry->stm_seg_full);
}

STATIC td_void stm_log_set_bypass(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print((data < VFMW_CHAN_NUM || data == VFMW_MAX_UINT32), "chn_id %d invalid.\n", data);

    entry->bypass_chan = (td_s32)data;
    dprint(PRN_ALWS, "set stm bypass chan %d\n", entry->bypass_chan);
}

STATIC td_void stm_log_set_save_seg(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 chan_id = 0;
    os_file *file = TD_NULL;
    td_char file_name[FILE_NAME_MAX_LEN];
    td_char *dbg_path = dbg_get_dbg_path();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val ((td_char *)((*buffer)[1]), &chan_id); /* 1 means get 2nd param */
    vfmw_check_print(chan_id < VFMW_CHAN_NUM, "chan_id %d out of range[0,%d).\n", chan_id, VFMW_CHAN_NUM);

    os_sema_down(stm_ctrl_get_entry()->seg_sema);

    file = &(SEG_FILE(chan_id));
    if (*file == TD_NULL) {
        if (snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1,
                "%s/vfmw_seg%d.bin", dbg_path, (td_s32)chan_id) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            goto out;
        }
        stm_log_open_file((td_s32)chan_id, file_name, FILE_NAME_MAX_LEN, file);
        if (*file != TD_NULL) {
            dprint(PRN_ALWS, "start save chan %d seg to %s.\n", chan_id, file_name);
        }
    } else {
        stm_log_close_file((td_s32)chan_id, file);
        dprint(PRN_ALWS, "stop save chan %d seg.\n", chan_id);
    }
out:
    os_sema_up(stm_ctrl_get_entry()->seg_sema);
}

STATIC td_void stm_log_set_save_seek_raw(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val ((td_char *)((*buffer)[1]), &data); /* 1 means get 2nd param */
    vfmw_check_print(data == 0 || data == 1, "stm_seg_full %d out of range[0,1].\n", data);

    entry->save_seek_raw = (td_s8)data;

    dprint(PRN_ALWS, "set stm save_seek_raw %d.\n", data);
}

STATIC td_void stm_log_thread_sleep(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 sleep_time = 0;
    stm_thread *thread = TD_NULL;
    stm_entry *entry = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val ((td_char *)((*buffer)[1]), &sleep_time); /* 1 means get 2nd param */
    vfmw_check_print(sleep_time <= LOG_MAX_SLEEP_TIME,
        "sleep_time %d out of range(0, %d].\n", sleep_time, LOG_MAX_SLEEP_TIME);

    entry = stm_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);
    entry->sleep_time = sleep_time;

    thread = stm_ctrl_get_thread();
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        dprint(PRN_ALWS, "stm thread not exist, entry->sleep_time %d\n", sleep_time);
    } else {
        thread->sleep_time = sleep_time;
        dprint(PRN_ALWS, "set Stm thread sleep_time: %d\n", sleep_time);
    }
}

STATIC td_void stm_log_thread_wait(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 wait_time = 0;
    stm_entry *entry = TD_NULL;
    stm_thread *thread = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val ((td_char *)((*buffer)[1]), &wait_time); /* 1 means get 2nd param */
    vfmw_check_print(wait_time <= LOG_MAX_WAIT_TIME, "wait_time %d out of range[0,%d]\n", wait_time, LOG_MAX_WAIT_TIME);

    entry = stm_ctrl_get_entry();
    vfmw_check(entry != TD_NULL);
    entry->wait_time = wait_time;

    thread = stm_ctrl_get_thread();
    if (thread == TD_NULL || thread->handle == TD_NULL) {
        dprint(PRN_ALWS, "stm thread not exist, entry->wait_time %d\n", wait_time);
    } else {
        thread->wait_time = wait_time;
        dprint(PRN_ALWS, "set stm thread wait_time: %d\n", wait_time);
    }
}

STATIC td_void stm_log_handle_cmd(cmd_str_ptr cmd_str, td_u32 cmd_len)
{
    td_s32 i;
    td_s32 count;
    cmd_handler handle = TD_NULL;
    td_char *cmd_id = (td_char *)((*cmd_str)[0]);

    STATIC const log_cmd cmds[] = {
        { LOG_CMD_SAVE_SEG,      stm_log_set_save_seg },
        { LOG_CMD_STM_SLEEP,     stm_log_thread_sleep },
        { LOG_CMD_STM_WAIT,      stm_log_thread_wait },
        { LOG_CMD_SAVE_SEEK_RAW, stm_log_set_save_seek_raw },
        { LOG_CMD_STM_NO_RAW,    stm_log_set_no_raw },
        { LOG_CMD_STM_NO_SEG,    stm_log_set_no_seg },
        { LOG_CMD_STM_SEG_FULL,  stm_log_set_seg_full },
        { LOG_CMD_STM_BYPASS,    stm_log_set_bypass },
    };

    count = (td_s32)(sizeof(cmds) / sizeof(log_cmd));
    vfmw_check(os_strncmp != TD_NULL);
    vfmw_check(os_strlen != TD_NULL);

    for (i = 0; i < count; i++) {
        if (os_strncmp(cmd_id, cmds[i].cmd_name, os_strlen(cmd_id)) == 0) {
            handle = cmds[i].handler;
            break;
        }
    }

    if (handle == TD_NULL) {
        dprint(PRN_ALWS, "%s unsupport command.\n", __func__);
        return;
    }

    handle(cmd_str, cmd_len);
}

td_s32 stm_log_read(td_void *buf, td_s32 chan_id, td_s32 len)
{
    td_s32 used = 0;

    vfmw_check_ret(buf != TD_NULL, STM_ERR);

    stm_ctrl_sema_down();

    stm_log_read_entry(buf, len, &used);
    stm_log_read_dev(buf, len, &used);
    stm_log_read_chn(buf, chan_id, len, &used);

    stm_ctrl_sema_up();

    return STM_OK;
}

td_s32 stm_log_write(cmd_str_ptr buffer, td_u32 count)
{
    if (buffer == TD_NULL || count <= 0) {
        return STM_ERR;
    }

    stm_log_handle_cmd(buffer, count);

    return STM_OK;
}

td_s32 stm_log_need_save_seek(td_void)
{
    stm_entry *entry = stm_ctrl_get_entry();

    return (entry->save_seek_raw != 0) ? TD_TRUE : TD_FALSE;
}

td_void stm_log_start_save_raw(td_s32 chan_id)
{
    os_file *file = TD_NULL;
    td_char file_name[FILE_NAME_MAX_LEN];

    vfmw_check(chan_id >= 0 && chan_id < STM_CHAN_NUM);

    os_sema_down(stm_ctrl_get_entry()->raw_sema);

    file = &(RAW_FILE(chan_id));
    if (*file == TD_NULL) {
        if (snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "vfmw_raw%d.bin", chan_id) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        stm_log_open_file(chan_id, file_name, FILE_NAME_MAX_LEN, file);
        if (*file != TD_NULL) {
            dprint(PRN_ALWS, "start save chan %d seek raw to %s.\n", chan_id, file_name);
        }
    }

    os_sema_up(stm_ctrl_get_entry()->raw_sema);
}

td_void stm_log_stop_save_raw(td_s32 chan_id)
{
    os_file *file = TD_NULL;

    vfmw_check(chan_id >= 0 && chan_id < STM_CHAN_NUM);

    os_sema_down(stm_ctrl_get_entry()->raw_sema);

    file = &(RAW_FILE(chan_id));
    if (*file != TD_NULL) {
        stm_log_close_file(chan_id, file);
        dprint(PRN_ALWS, "stop save chan %d seek raw.\n", chan_id);
    }

    os_sema_up(stm_ctrl_get_entry()->raw_sema);
}

td_void stm_log_save_raw(td_s32 chan_id, td_void *raw_packet)
{
#ifdef ENV_ARMLINUX_KERNEL
    td_s32 write_len;
    const td_u32 ivf_file_head = 32;
    const td_u32 ivf_frm_head = 12;
    const td_u32 raw_frm_head = 4;
    stm_chan *chan = STM_CTRL_GET_CHAN(chan_id);
    stm_raw_info *raw = (stm_raw_info *)raw_packet;

    vfmw_check(os_fwrite != TD_NULL);

    if (RAW_FILE(chan_id) == TD_NULL || chan == TD_NULL ||
        raw == TD_NULL || raw->is_extern_eop_flag == 1 || raw->vir_addr == 0) {
        return;
    }

    os_sema_down(stm_ctrl_get_entry()->raw_sema);

    if (RAW_FILE(chan_id) == TD_NULL) {
        os_sema_up(stm_ctrl_get_entry()->raw_sema);
        return;
    }

    if (chan->cfg.scd_bypass) {
        if (chan->cfg.vid_std == VFMW_VP9) {
            const td_u8 *ivf_signature = "DKIF";

            if (chan->raw_list->raw_num_all == 1) {
                os_fwrite(ivf_signature, ivf_file_head, RAW_FILE(chan_id));
            }

            os_fwrite ((td_u8 *)(&(raw->length)), ivf_frm_head, RAW_FILE(chan_id));
        } else {
            os_fwrite ((td_void *)(&(raw->length)), raw_frm_head, RAW_FILE(chan_id));
        }
    }

    write_len = os_fwrite(uint64_ptr(raw->vir_addr), raw->length, RAW_FILE(chan_id));
    if (write_len < raw->length) {
        dprint(PRN_ERROR, "%s %d error !\n", __func__, __LINE__);
    }
    os_fsync(RAW_FILE(chan_id));

    os_sema_up(stm_ctrl_get_entry()->raw_sema);
#else
    vfmw_unused(chan_id);
    vfmw_unused(raw_packet);
#endif
}

td_void stm_log_save_seg(td_s32 chan_id, const td_void *seg_packet)
{
    td_s32 write_len;
    stm_seg_info *seg = (stm_seg_info *)seg_packet;

    vfmw_check(os_fwrite != TD_NULL);

    if (SEG_FILE(chan_id) == TD_NULL || seg == TD_NULL ||
        seg->cut_result == SEG_CUT_HALF || seg->vir_addr == TD_NULL) {
        return;
    }

    os_sema_down(stm_ctrl_get_entry()->seg_sema);

    if (SEG_FILE(chan_id) != TD_NULL) {
        write_len = os_fwrite((td_char *)seg->vir_addr, seg->length, SEG_FILE(chan_id));
        if (write_len < (td_s32)seg->length) {
            dprint(PRN_ERROR, "%s %d error !\n", __func__, __LINE__);
        }
    }
    os_fsync(SEG_FILE(chan_id));

    os_sema_up(stm_ctrl_get_entry()->seg_sema);
}
