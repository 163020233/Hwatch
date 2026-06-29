/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stream_log.h"
#include "stream.h"
#include "vfmw_proc.h"
#include "dbg.h"

typedef struct {
    td_bool print_block;
} stream_log_entry;

static stream_log_entry g_stm_log_entry = {0};
static td_char *g_stream_mode[2] = {"outer", "inner"}; /* 2 :a number */
static td_char *g_block_status[3] = {"write", "free", "read"}; /* 3 :a number */

static stream_log_entry *stream_log_get_entry(td_void)
{
    return &g_stm_log_entry;
}

STATIC td_void stream_log_set_print_block(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 print_en = 0;
    stream_log_entry *entry = stream_log_get_entry();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((*buffer)[1], &print_en);
    vfmw_check_print(print_en == 0 || print_en == 1, "print_block %d out of range[0,1].\n", print_en);

    entry->print_block = (td_bool)print_en;
    os_print("set stream print_block %d\n", entry->print_block);
}

STATIC td_void stream_log_save_raw(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 chan_id = 0;
    stream_chan *chan = TD_NULL;
    td_char name[FILE_NAME_MAX_LEN];
    td_char *dbg_path = dbg_get_dbg_path();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val ((td_char *)((*buffer)[1]), &chan_id); /* 1 : 2nd param */
    vfmw_check_print(chan_id < VFMW_CHAN_NUM, "chan_id %d out of range[0,%d).\n", chan_id, VFMW_CHAN_NUM);

    chan = stream_get_chan((td_s32)chan_id);

    os_sema_down(chan->sema);

    if (chan->file == TD_NULL) {
        if (snprintf_s(name, sizeof(name), sizeof(name) - 1, "%s/vfmw_raw%d.bin", dbg_path, (td_s32)chan_id) < 0) {
            os_print("%s %d sec func err\n", __func__, __LINE__);
            goto out;
        }
        if (os_fopen == TD_NULL) {
            goto out;
        }
        chan->file = os_fopen(name, OS_RDWR | OS_CREATE | OS_TRUNC, OS_OPEN_MODE);
        if (chan->file == TD_NULL) {
            os_print("open %s failed.\n", name);
        } else {
            os_print("start save chan %d raw.\n", chan_id);
        }
    } else {
        os_fclose(chan->file);
        chan->file = TD_NULL;
        os_print("stop save chan %d raw.\n", chan_id);
    }
out:
    os_sema_up(chan->sema);
}

STATIC td_void stream_log_print_block(const stream_log_entry *entry, stream_chan *chan, td_void *buf, td_s32 len,
    td_s32 *used)
{
    stream_block *block = TD_NULL;

    if (entry->print_block) {
        os_dump_proc(buf, len, used, 0, LOG_STREAM_BLOCK_FMT_HEAD);
        os_list_for_each_entry(block, &chan->list, node) {
            os_dump_proc(buf, len, used, 0, LOG_FMT_X_S,
                         "phy_addr", log_get_mask_phy_addr(block->buf.phy_addr),
                         "status", g_block_status[block->status]);
        }
    }
}

td_s32 stream_log_read(td_void *buf, td_s32 len, td_s32 *used)
{
    td_s32 chan_id;
    stream_log_entry *entry = stream_log_get_entry();
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(buf != TD_NULL, EXT_FAILURE);

    os_dump_proc(buf, len, used, 0, LOG_STREAM_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "pr_block", entry->print_block, " ");

    for (chan_id = 0; chan_id < VFMW_CHAN_NUM; chan_id++) {
        chan = stream_get_chan(chan_id);
        stream_lock_chan(chan_id);
        if (chan->is_used) {
            vfmw_check_goto(chan->mode < 2, out); /* 2: array size */
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "chan_id", chan_id, "mode", g_stream_mode[chan->mode]);
            os_dump_proc(buf, len, used, 0, LOG_FMT_X_X, "phy_addr", log_get_mask_phy_addr(chan->start_phy_addr),
                         "length", chan->length);
            os_dump_proc(buf, len, used, 0, LOG_FMT_X_X,
                         "kern_addr", log_get_mask_vir_addr(chan->start_kern_vir_addr),
                         "usr_addr", log_get_mask_vir_addr(chan->start_usr_vir_addr));
            os_dump_proc(buf, len, used, 0, LOG_FMT_X_X, "write_offst", chan->write_offst,
                         "read_offst", chan->read_offst);
            os_dump_proc(buf, len, used, 0, LOG_FMT_X_X, "free_len", chan->free_len, "used_len", chan->used_len);
            os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "freeze", chan->freeze_size, "sec", chan->is_sec);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "try_read", chan->try_read_cnt,
                         "read_suc", chan->read_ok_cnt);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "try_rel", chan->try_release_cnt,
                         "rel_suc", chan->release_ok_cnt);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "try_deq", chan->try_dequeue_cnt,
                         "deq_suc", chan->dequeue_ok_cnt);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "try_que", chan->try_queue_cnt,
                         "que_suc", chan->queue_ok_cnt);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "eos", chan->eos_flag, "valid_pts", chan->valid_pts_flag);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "max_raw_num", chan->max_raw_num, "raw_num", chan->raw_num);
            os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "que_eos_cnt", chan->que_eos_cnt, " ");

            stream_log_print_block(entry, chan, buf, len, used);
        }
out:
        stream_unlock_chan(chan_id);
    }

    return EXT_SUCCESS;
}

td_s32 stream_log_write(cmd_str_ptr buffer, td_u32 count)
{
    td_u32 i;
    td_char *cmd_id = ((*buffer)[0]);
    cmd_handler handler = TD_NULL;

    STATIC const log_cmd com_cmd[] = {
        { LOG_CMD_PRT_STM_BLK, stream_log_set_print_block },
        { LOG_CMD_SAVE_RAW,    stream_log_save_raw },
    };
    vfmw_check_ret(os_strncmp != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(os_strlen != TD_NULL, EXT_FAILURE);

    for (i = 0; i < sizeof(com_cmd) / sizeof(log_cmd); i++) {
        if (os_strncmp(cmd_id, com_cmd[i].cmd_name, os_strlen(cmd_id)) == 0) {
            handler = com_cmd[i].handler;
            break;
        }
    }

    if (handler == TD_NULL) {
        return EXT_FAILURE;
    }

    handler(buffer, count);

    return EXT_SUCCESS;
}
