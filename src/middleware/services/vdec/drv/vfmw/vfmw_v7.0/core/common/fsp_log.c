/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "fsp_log.h"
#include "fsp.h"
#include "vfmw_proc.h"
#include "dbg.h"

STATIC td_void fsp_log_read_entry(td_void *buf, td_s32 *used, td_s32 len)
{
    fsp_state stateid;
    td_char *state_str[FSP_STATE_BUTT + 1] = { "Null", "Open", "Init", "Exit", "NA" };
    fsp_entry *entry = TD_NULL;
    fsp_entry *fsp_get_entry(td_void);

    entry = fsp_get_entry();
    stateid = (entry->state < FSP_STATE_BUTT) ? entry->state : FSP_STATE_BUTT;

    os_dump_proc(buf, len, used, 0, LOG_FSP_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_S, "check1", entry->reserve1, "state", state_str[stateid]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "check2", entry->reserve2, "ref_cnt", entry->ref_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "ctx_size", entry->ctx_size, " ");
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "no_dec", entry->no_dec_fs, "no_dsp", entry->no_dsp_fs);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "no_pmv", entry->no_pmv_fs, "no_att", entry->no_att_fs);
}

STATIC td_void fsp_log_read_logic(fsp_chan *chan, td_void *buf, td_s32 *used, td_s32 len)
{
    td_s32 idx;
    td_bool is_free;
    td_s32 free_num = 0;
    td_s32 used_num = 0;
    fsp_logic *fs = TD_NULL;

    os_dump_proc(buf, len, used, 0, LOG_FLUI_FMT_HEAD);
    os_dump_proc(buf, len, used, 0,
                 "%-3s %-3s %-3s %-7s %-7s %-7s %-7s %-7s %-3s %-7s %-4s %-10s %-10s %-10s %-7s|\r\n",
                 "num", "use", "ref", "dec_ref", "ref_cnt", "dec_sta", "dsp_sta",
                 "fld_vld", "voq", "img_vld", "tick", "dec_addr",
                 "pmv_addr", "disp_addr", "air_sta");
    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        fs = &chan->logic_fs[idx];
        is_free = fsp_is_logic_fs_free(fs, chan->fs_mem.need.disp_info.is_disp_need);
        if (is_free == TD_TRUE) {
            free_num++;
        } else {
            used_num++;
            os_dump_proc(buf, len, used, 0,
                "%-3d %-3d %-3d %-7d %-7d %-7d %-7d %-7d %-3d %-7d %-4d 0x%x 0x%x 0x%x %7d|\r\n",
                fs->logic_id, fs->is_use, fs->is_ref, fs->is_dec_ref, (fs->dec_fs == TD_NULL) ?
                0 : fs->dec_fs->ref_cnt,
                fs->dec_state.data, fs->disp_state, fs->image.format.field_valid_flag, fs->is_in_voq,
                fs->image.disp_info.is_valid, fs->tick,
                (fs->dec_fs) ? log_get_mask_phy_addr(fs->dec_fs->phy_addr) : 0,
                (fs->pmv_fs) ? log_get_mask_phy_addr(fs->pmv_fs->phy_addr) : 0,
                (fs->disp_fs) ? log_get_mask_phy_addr(fs->disp_fs->phy_addr) : 0, fs->fs_air_state
            );
        }
    }
    os_dump_proc(buf, len, used, 0, LOG_FLN_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "free_logic_num", free_num, "used_logic_num", used_num);
}

STATIC td_void fsp_log_read_dec(fsp_chan *chan, td_void *buf, td_s32 *used, td_s32 len)
{
    td_s32 idx;
    td_bool is_free;
    td_s32 free_num = 0;
    fsp_phy *dec = TD_NULL;

    os_dump_proc(buf, len, used, 0, LOG_FSP_DEC_INFO_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, "%-4s %-6s %-6s %-6s %-10s %-14s %-18s %-8s|\r\n", "num", "is_use",
                 "is_map", "tick", "size", "mem_fd", "address", "is_free");
    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        dec = &chan->dec_fs[idx];
        if (dec->is_valid == 0) {
            continue;
        }
        is_free = fsp_is_dec_fs_free(chan, dec);
        if (is_free == TD_TRUE) {
            free_num++;
        }
        os_dump_proc(buf, len, used, 0, "%-4d %-6d %-6d %-6d %-10d %-14lld 0x%-16x %-8s|\r\n",
                     dec->fs_id, dec->is_use, dec->is_map, dec->tick, dec->buf_size, dec->mem_fd,
                     log_get_mask_phy_addr(dec->phy_addr), (is_free) ? "Yes" : "No");
    }
    os_dump_proc(buf, len, used, 0, LOG_FSP_DEC_NUM_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "free_dec_num", free_num, "total_dec_num", chan->fs_mem.total_dec_num);
}

STATIC td_void fsp_log_read_disp(fsp_chan *chan, td_void *buf, td_s32 *used, td_s32 len)
{
    td_s32 idx;
    td_bool is_free;
    td_s32 free_num = 0;
    fsp_phy *disp = TD_NULL;

    os_dump_proc(buf, len, used, 0, LOG_FSP_DISP_INFO_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, "%-4s %-6s %-6s %-6s %-10s %-14s %-18s %-8s|\r\n", "num", "is_use",
                 "is_map", "tick", "size", "mem_fd", "address", "air_sta");
    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        disp = &chan->disp_fs[idx];
        if (disp->is_valid == 0) {
            continue;
        }
        is_free = fsp_is_disp_fs_free(chan, disp);
        if (is_free == TD_TRUE) {
            free_num++;
        }
        os_dump_proc(buf, len, used, 0, "%-4d %-6d %-6d %-6d %-10d 0x%-12x 0x%-16llx %-8d|\r\n",
                     disp->fs_id, disp->is_use, disp->is_map, disp->tick, disp->buf_size, disp->mem_fd,
                     log_get_mask_phy_addr(disp->phy_addr), disp->fs_air_state);
    }
    os_dump_proc(buf, len, used, 0, LOG_FSP_DISP_NUM_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "free_disp_num", free_num, "total_disp_num",
        chan->fs_mem.total_disp_num);
}

STATIC td_void fsp_log_read_pmv(fsp_chan *chan, td_void *buf, td_s32 *used, td_s32 len)
{
    td_s32 idx;
    td_bool is_free;
    td_s32 free_num = 0;
    fsp_pmv *pmv = TD_NULL;

    os_dump_proc(buf, len, used, 0, LOG_FSP_PMV_INFO_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, "%-5s %-7s %-8s %-9s %-14s %-18s %-12s|\r\n", "num", "is_use",
                 "is_map", "tick", "size", "address", "is_free");
    for (idx = 0; idx < FSP_PMV_NUM; idx++) {
        pmv = &chan->pmv_fs[idx];
        if (pmv->is_valid == 0) {
            continue;
        }
        is_free = fsp_is_pmv_free(pmv, (td_u32)chan->fs_mem.wait_pmv_size);
        if (is_free == TD_TRUE) {
            free_num++;
        }
        os_dump_proc(buf, len, used, 0, "%-5d %-7d %-8d %-9d %-14d 0x%-16x %-12s|\r\n",
                     pmv->pmv_id, pmv->is_use, pmv->is_map, pmv->tick, pmv->buf_size,
                     log_get_mask_phy_addr(pmv->phy_addr), (is_free) ? "yes" : "no");
    }
    os_dump_proc(buf, len, used, 0, LOG_FSP_PMV_NUM_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "free_pmv_num", free_num, "total_pmv_num", chan->fs_mem.total_pmv_num);
}

STATIC td_void fsp_log_read_nor(fsp_chan *chan, td_void *buf, td_s32 *used, td_s32 len)
{
    td_s32 idx;
    td_bool is_free;
    td_s32 free_num = 0;
    td_s32 valid_num = 0;
    fsp_attach *attach = TD_NULL;

    if (chan->fs_mem.need.nor_size == 0) {
        return;
    }
    os_dump_proc(buf, len, used, 0, LOG_FSP_NOR_INFO_FMT_HEAD);
    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        attach = &chan->nor_fs[idx];
        if (attach->is_valid == 0) {
            continue;
        }
        valid_num++;
        is_free = fsp_is_attach_free(attach);
        if (is_free == TD_TRUE) {
            free_num++;
        }
        os_dump_proc(buf, len, used, 0, "%-19d:%-19s|", attach->fs_id,
                     (attach->is_use != 0) ? "InUse" : "Free");
        if (valid_num % 2 == 0) { /* 2 :a number */
            os_dump_proc(buf, len, used, 0, "\n");
        }
    }

    if (valid_num % 2 != 0) { /* 2 :a number */
        os_dump_proc(buf, len, used, 0, "\n");
    }
    os_dump_proc(buf, len, used, 0, LOG_FSP_NOR_NUM_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "free_nor_num", free_num, "valid_nor_num", valid_num);
}

STATIC td_void fsp_log_read_sec(fsp_chan *chan, td_void *buf, td_s32 *used, td_s32 len)
{
    td_s32 idx;
    td_bool is_free;
    td_s32 free_num = 0;
    td_s32 valid_num = 0;
    fsp_attach *attach = TD_NULL;

    if (chan->fs_mem.need.sec_size == 0) {
        return;
    }

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        attach = &chan->sec_fs[idx];
        if (attach->is_valid == 0) {
            continue;
        }
        valid_num++;
        is_free = fsp_is_attach_free(attach);
        if (is_free == TD_TRUE) {
            free_num++;
        }
        os_dump_proc(buf, len, used, 0, "  %-2d: %s  |", attach->fs_id,
                     (attach->is_use != 0) ? "InUse" : "Free ");
        if (valid_num % 5 == 0) { /* 5 :a number */
            os_dump_proc(buf, len, used, 0, "\n");
        }
    }

    if (valid_num % 5 != 0) { /* 5 :a number */
        os_dump_proc(buf, len, used, 0, "\n");
    }
    os_dump_proc(buf, len, used, 0, "  Sec: Free %d / Total %d\n", free_num, valid_num);
    os_dump_proc(buf, len, used, 0, "\n");
}

STATIC td_void fsp_log_read_com(fsp_chan *chan, td_void *buf, td_s32 *used, td_s32 len)
{
    mem_attach attach;
    fsp_mem *curr = TD_NULL;
    mem_need_info *need = TD_NULL;
    td_char *attach_str[ATTACH_BUTT + 1] = { "None", "Dec", "Disp", "NA" };

    curr = &chan->fs_mem;
    need = &chan->fs_mem.need;
    attach = (need->attach < ATTACH_BUTT) ? need->attach : ATTACH_BUTT;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "bit_depth", chan->cfg.bit_depth, "vid_std",
                 dbg_show_std(chan->cfg.vid_std, TD_NULL));
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dec_width", chan->cfg.dec_width, "cfg_dec_num",
                 chan->cfg.need_dec_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dec_height", chan->cfg.dec_height, "cfg_disp_num",
                 chan->cfg.need_disp_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "disp_width", chan->cfg.disp_width, "cfg_pmv_num",
                 chan->cfg.need_pmv_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "disp_height", chan->cfg.disp_height, "need_mtdt",
                 chan->cfg.need_mtdt);

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "need_dec_num", need->dec_info.dec_num, "need_pmv_num",
                 need->pmv_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "need_dec_size", need->dec_info.dec_size, "need_pmv_size",
                 need->pmv_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "need_disp_num", need->disp_info.disp_num, "head_stride",
                 need->disp_info.head_stride);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "need_disp_size", need->disp_info.disp_size, "head_size",
                 need->disp_info.head_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_D, "attach", attach_str[attach], "nor_size", need->nor_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "dec_cmp", need->dec_info.compress_en, "disp_cmp",
                 need->disp_info.compress_en);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "add_pmv_num", need->extra_pmv_num, "add_fs_num",
                 need->extra_fs_num);

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "resize_succ", curr->resize_succ, " ");
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "have_dec_num", curr->have_dec_num, "total_dec_num",
                 curr->total_dec_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "have_pmv_num", curr->have_pmv_num, "total_pmv_num",
                 curr->total_pmv_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "have_disp_num", curr->have_disp_num, "total_disp_num",
                 curr->total_disp_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "apply_dec_num", curr->apply_dec_num, "wait_dec_size",
                 curr->wait_dec_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "apply_pmv_num", curr->apply_pmv_num, "wait_pmv_size",
                 curr->wait_pmv_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "apply_disp_num", curr->apply_disp_num, "wait_disp_size",
                 curr->wait_disp_size);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "air_dec", chan->attr.air_version, "out_frm_mode",
                 chan->attr.outer_frm_mode);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "out_frm_st", chan->outer_frm_status, "wait_unbind",
                 chan->wait_unbind);
}

STATIC td_void fsp_log_read_chan(td_void *buf, td_s32 chan_id, td_s32 *used, td_s32 len)
{
    fsp_perf perf;
    fsp_chan *chan = TD_NULL;
    td_ulong lockflag;
    td_char *mem_mode[] = {"INNER", "OUTER"};
    td_char *perf_str[FSP_PERF_BUTT + 1] = {"Low", "High", "NA"};

    fsp_lock(chan_id, &lockflag);

    chan = fsp_get_chan(chan_id);
    if (chan == TD_NULL || chan->handle == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "fsp chan %d not exists.%-57s|\r\n", chan_id, " ");
        goto out;
    }

    perf = (chan->attr.perf < FSP_PERF_BUTT) ? chan->attr.perf : FSP_PERF_BUTT;

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chan_id", chan->id, "state", chan->state);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_P, "unid", chan->unid, "handle", chan->handle);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "is_omx_path", 0, "extra_fs_num",
                 chan->attr.extra_fs_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_S_S, "performance", perf_str[perf], "mem_mode",
                 mem_mode[chan->attr.alloc_type]);

    fsp_log_read_com(chan, buf, used, len);
    fsp_log_read_logic(chan, buf, used, len);
    fsp_log_read_dec(chan, buf, used, len);
    fsp_log_read_disp(chan, buf, used, len);
    fsp_log_read_pmv(chan, buf, used, len);
    fsp_log_read_nor(chan, buf, used, len);
    fsp_log_read_sec(chan, buf, used, len);

out:
    fsp_unlock(chan_id, &lockflag);
    os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
    return;
}

td_s32 fsp_log_read(td_void *buf, td_s32 chan_id, td_s32 len)
{
    td_s32 used = 0;

    vfmw_check_ret(buf != TD_NULL, EXT_FAILURE);

    fsp_sema_down();

    fsp_log_read_entry(buf, &used, len);

    fsp_log_read_chan(buf, chan_id, &used, len);

    fsp_sema_up();

    return EXT_SUCCESS;
}

STATIC td_void fsp_log_set_no_dec(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    fsp_entry *entry = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "no_dec %d out of range[0,1].\n", data);

    entry = fsp_get_entry();
    vfmw_check(entry != TD_NULL);

    entry->no_dec_fs = data;
    dprint(PRN_ALWS, "set fsp no_dec %d\n", entry->no_dec_fs);
}

STATIC td_void fsp_log_set_no_dsp(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    fsp_entry *entry = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "no_dsp %d out of range[0,1].\n", data);

    entry = fsp_get_entry();
    vfmw_check(entry != TD_NULL);

    entry->no_dsp_fs = data;
    dprint(PRN_ALWS, "set fsp no_dsp %d\n", entry->no_dsp_fs);
}

STATIC td_void fsp_log_set_no_pmv(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    fsp_entry *entry = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "no_pmv %d out of range[0,1].\n", data);

    entry = fsp_get_entry();
    vfmw_check(entry != TD_NULL);

    entry->no_pmv_fs = data;
    dprint(PRN_ALWS, "set fsp no_pmv %d\n", entry->no_pmv_fs);
}

STATIC td_void fsp_log_set_no_att(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 data = 0;
    fsp_entry *entry = TD_NULL;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((td_char *)((*buffer)[1]), &data);
    vfmw_check_print(data == 0 || data == 1, "no_att %d out of range[0,1].\n", data);

    entry = fsp_get_entry();
    vfmw_check(entry != TD_NULL);

    entry->no_att_fs = data;
    dprint(PRN_ALWS, "set fsp no_att %d\n", entry->no_att_fs);
}

STATIC td_void fsp_log_handle_cmd(cmd_str_ptr cmd_str, td_u32 cmd_len)
{
    td_s32 i;
    td_s32 count;
    cmd_handler handle = TD_NULL;
    td_char *cmd_id = (td_char *)((*cmd_str)[0]);

    STATIC const log_cmd cmds[] = {
        { LOG_CMD_FSP_NO_DEC,    fsp_log_set_no_dec },
        { LOG_CMD_FSP_NO_DSP,    fsp_log_set_no_dsp },
        { LOG_CMD_FSP_NO_PMV,    fsp_log_set_no_pmv },
        { LOG_CMD_FSP_NO_ATT,    fsp_log_set_no_att },
    };
    vfmw_check(os_strncmp != TD_NULL);
    vfmw_check(os_strlen != TD_NULL);
    count = (td_s32)(sizeof(cmds) / sizeof(log_cmd));
    for (i = 0; i < count; i++) {
        if (os_strncmp(cmd_id, cmds[i].cmd_name, os_strlen(cmd_id)) == 0) {
            handle = cmds[i].handler;
            break;
        }
    }

    vfmw_alert(handle != TD_NULL);
    handle(cmd_str, cmd_len);
}

td_s32 fsp_log_write(cmd_str_ptr buffer, td_u32 count)
{
    if (buffer == TD_NULL || count <= 0) {
        return EXT_FAILURE;
    }

    fsp_log_handle_cmd(buffer, count);

    return EXT_SUCCESS;
}
