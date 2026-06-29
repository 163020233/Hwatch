/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dps_log.h"
#include "dps.h"
#include "vfmw_osal.h"
#include "dbg.h"

STATIC td_void dps_log_read_chan(td_void *buf, td_s32 chan_id, td_s32 *used, td_s32 len)
{
    dps_chan_info *dps_get_chan(td_s32 chan_id);

    td_s32 cnt;
    dps_chan_info *chan;
    dps_state state_id;
    td_char *dps_str[DPS_MAX + 1] = { "idle", "make", "wait", "dec", "NA" };
    td_char *sed_str[SED_MAX + 1] = { "idle", "try", "succ", "NA" };

    chan = dps_get_chan(chan_id);
    if (chan == TD_NULL || chan->handle == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "dps chan %d not exists.%-57s|\r\n", chan_id, " ");
        return;
    }

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chan_id", chan->chan_id, "state", chan->state);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_P, "unid", chan->unid, "handle", chan->handle);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "mem_mode", chan->cfg.mem_mode, "vid_std",
                 dbg_show_std(chan->cfg.vid_std, TD_NULL));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "max_num", chan->cfg.max_num, "max_slc_num",
                 chan->cfg.max_slc_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "head", chan->head, "tail", chan->tail);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "make_num", chan->make_num, "recv_try", chan->recv_try);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "rels_num", chan->rels_num, "recv_num", chan->recv_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "sed_try", chan->sed_try, "sed_succ", chan->sed_succ);
    os_dump_proc(buf, len, used, 0, LOG_DPS_LIST_FMT_HEAD);

    for (cnt = 0; cnt < MAX_DPS_NUM; cnt++) {
        state_id = (chan->dps[cnt].state < DPS_MAX) ? chan->dps[cnt].state : DPS_MAX;
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "dps_id", chan->dps[cnt].dps_id, "state",
                     dps_str[state_id]);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "slice_num", chan->dps[cnt].slice_num,
                     "is_frm_cpy", chan->dps[cnt].is_frm_cpy);
        state_id = (dps_state)((chan->dps[cnt].sed_state < SED_MAX) ? chan->dps[cnt].sed_state : SED_MAX);
        os_dump_proc(buf, len, used, 0, LOG_FMT_S_N, "sed_state", sed_str[state_id], " ");
    }
}

td_s32 dps_log_read(td_void *buf, td_s32 chan_id, td_s32 *used, td_s32 len)
{
    dps_entry_info *entry = TD_NULL;
    dps_mod_state state_id;
    td_char *state_str[DPS_MOD_MAX + 1] = { "null", "open", "init", "exit", "NA" };

    dps_sema_down();

    entry = dps_get_entry();
    state_id = (entry->state < DPS_MOD_MAX) ? entry->state : DPS_MOD_MAX;

    os_dump_proc(buf, len, used, 0, LOG_DPS_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_S, "check1", entry->reserve1, "state", state_str[state_id]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "check2", entry->reserve2, "ref_cnt", entry->ref_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "ctx_size", entry->ctx_size, "ser_mod", entry->ser_mode);

    dps_log_read_chan(buf, chan_id, used, len);

    dps_sema_up();

    return EXT_SUCCESS;
}

STATIC td_void dps_log_set_ser_mode(cmd_str_ptr buffer, td_u32 count)
{
    dps_entry_info *entry = TD_NULL;
    td_u32 data = 0;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */
    vfmw_check_print(data == 0 || data == 1, "dps ser_mode %d out of range[0,1].\n", data);

    entry = dps_get_entry();
    entry->ser_mode = (td_s32)data;
    dprint(PRN_ALWS, "set dps ser_mode = %d.\n", data);
}

STATIC td_void dps_log_set_dps_num(cmd_str_ptr buffer, td_u32 count)
{
    dps_entry_info *entry = TD_NULL;
    td_u32 data = 0;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    entry = dps_get_entry();
    entry->dps_num = (td_s32)data;
    dprint(PRN_ALWS, "set dps num = %d.\n", data);
}

STATIC td_void dps_log_set_no_recv(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    dps_entry_info *entry = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "dps no_recv %d out of range[0,1].\n", data);

    entry = dps_get_entry();
    entry->no_dps = data;
    dprint(PRN_ALWS, "set dps no_recv %d\n", entry->no_dps);
}


STATIC td_void dps_log_handle_cmd(cmd_str_ptr cmd_str, td_u32 cmd_len)
{
    td_s32 i;
    td_s32 count;
    cmd_handler handler = TD_NULL;
    td_char *cmd_id = (td_char *)((*cmd_str)[0]);

    STATIC const log_cmd dps_cmd[] = {
        { LOG_CMD_DPS_SER,       dps_log_set_ser_mode },
        { LOG_CMD_DPS_NUM,       dps_log_set_dps_num },
        { LOG_CMD_DPS_NO_RECV,   dps_log_set_no_recv },
    };

    count = (td_s32)(sizeof(dps_cmd) / sizeof(log_cmd));
    vfmw_check(os_strncmp != TD_NULL);
    vfmw_check(os_strlen != TD_NULL);

    for (i = 0; i < count; i++) {
        if (os_strncmp(cmd_id, dps_cmd[i].cmd_name, os_strlen(cmd_id)) == 0) {
            handler = dps_cmd[i].handler;
            break;
        }
    }

    if (handler == TD_NULL) {
        dprint(PRN_ALWS, "unsupport command.\n");
        return;
    }

    handler(cmd_str, cmd_len);
}

td_s32 dps_log_write(cmd_str_ptr buffer, td_s32 count)
{
    if (buffer == TD_NULL || count <= 0) {
        return DPS_ERR;
    }

    dps_log_handle_cmd(buffer, (td_u32)count);

    return DPS_OK;
}
