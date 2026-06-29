/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
* Description: decoder
* Author: AuthorNameMagicTag
* Create: 2019-04-22
 */

#include "vpp_log.h"
#include "vpp.h"
#include "dbg.h"

STATIC td_char *vpp_show_thread_state(vpp_thread_state type)
{
    switch (type) {
        case THREA_IDLE:
            return "IDLE";

        case THREA_RUNNING:
            return "RUNNING";

        case THREA_SLEEP:
            return "SLEEP";

        default:
            return "INVALID";
    }
}

STATIC td_void vpp_log_set_bypass(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 chn_id = 0;
    vpp_entry *entry = TD_NULL;

    entry = vpp_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd count %d err\n", count);

    str_to_val((td_char *)((*buffer)[1]), &chn_id);
    vfmw_check(chn_id < VFMW_CHAN_NUM || chn_id == VFMW_MAX_UINT32);
    vfmw_check_print((chn_id < VFMW_CHAN_NUM || chn_id == VFMW_MAX_UINT32), "chn_id %d invalid.\n", chn_id);

    entry->bypass_chan = (td_s32)chn_id;
    dprint(PRN_ALWS, "set bypass_chan %d\n", entry->bypass_chan);
}

STATIC td_void vpp_log_set_prt_frm(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vpp_entry *entry = TD_NULL;

    entry = vpp_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd count %d err\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "print_frm %d out of range[0,1].\n", data);

    entry->print_frm = data;
    dprint(PRN_ALWS, "set prt_frm %d\n", entry->print_frm);
}

STATIC td_void vpp_log_set_prt_buf(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vpp_entry *entry = TD_NULL;

    entry = vpp_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd count %d err\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "print_buf %d out of range[0,1].\n", data);

    entry->print_buf = data;
    dprint(PRN_ALWS, "set prt_buf %d\n", entry->print_buf);
}

STATIC td_void vpp_log_set_no_recv(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    vpp_entry *entry = TD_NULL;

    entry = vpp_get_entry();
    vfmw_check(entry != TD_NULL);

    vfmw_check_print(count >= 0x2, "cmd count %d err\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "no_recv_frm %d out of range[0,1].\n", data);

    entry->no_recv_frm = data;
    dprint(PRN_ALWS, "set no_recv_frm %d\n", entry->no_recv_frm);
}

STATIC td_s32 vpp_log_handle_cmd(cmd_str_ptr cmd_str, td_u32 cmd_len)
{
    td_u32 i;
    td_u32 count;
    cmd_handler handle = TD_NULL;
    td_char *cmd_id = (td_char *)((*cmd_str)[0]);

    STATIC const log_cmd cmds[] = {
        { LOG_CMD_VPP_BYPASS,    vpp_log_set_bypass },
        { LOG_CMD_VPP_PR_FRM,    vpp_log_set_prt_frm },
        { LOG_CMD_VPP_PR_BUF,    vpp_log_set_prt_buf },
        { LOG_CMD_VPP_NO_RECV,   vpp_log_set_no_recv },
    };
    vfmw_check_ret(os_strncmp != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(os_strlen != TD_NULL, EXT_FAILURE);

    count = (td_u32)sizeof(cmds) / (td_u32)sizeof(log_cmd);
    for (i = 0; i < count; i++) {
        if (os_strncmp(cmd_id, cmds[i].cmd_name, os_strlen(cmd_id)) == 0) {
            handle = cmds[i].handler;
            break;
        }
    }

    if (handle != TD_NULL) {
        handle(cmd_str, cmd_len);
        return EXT_SUCCESS;
    }

    return EXT_FAILURE;
}

td_s32 vpp_log_read(td_void *buf, td_s32 len, td_s32 *used)
{
    td_s32 i;
    vpp_entry *entry = vpp_get_entry();
    vpp_chan *chan = TD_NULL;

    os_dump_proc(buf, len, used, 0, LOG_VPP_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "init", entry->inited, "thread_state",
                 vpp_show_thread_state(entry->thread_state));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "bypass_chan", entry->bypass_chan, "print_frm", entry->print_frm);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "no_recv_frm", entry->no_recv_frm, "no_rels_frm", entry->no_rels_frm);

    for (i = 0; i < VFMW_CHAN_NUM; i++) {
        vpp_sema_down();
        chan = vpp_get_chan(i);
        if (chan != TD_NULL) {
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chan_id", chan->chan_id, "state", chan->state);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "work_mode", chan->work_mode, "source", chan->source);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "err_thr", chan->err_thr, "dec_mode", chan->dec_mode);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "recv_try", chan->receive_try, "recv_ok", chan->receive_ok);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "rels_try", chan->release_try, "rels_ok", chan->release_ok);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "set_3d", 0, "frm_pack_type", chan->last_image.frm_pack_type);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "frm_fence_enable", chan->frm_fence_enable, " ");
        }
        vpp_sema_up();
    }

    return EXT_SUCCESS;
}

td_s32 vpp_log_write(cmd_str_ptr buffer, td_u32 count)
{
    if (buffer == TD_NULL || count <= 0) {
        return EXT_FAILURE;
    }

    return vpp_log_handle_cmd(buffer, count);
}
