/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "voque_log.h"
#include "voque.h"
#include "vfmw_osal.h"
#include "vfmw_proc.h"
#include "dbg.h"

STATIC td_void voq_log_read_chan(td_void *buf, td_s32 chan_id, td_s32 *used, td_s32 len)
{
    td_s32 head_fsp = -1;
    td_u32 empty, wait_get, wait_rel;
    voq_chan *voq_get_chan(td_s32 chan_id);
    voq_chan *chan = TD_NULL;
    voq_eos_state eos_state;
    voq_eos_type eos_type;
    td_char *eos_state_str[VOQ_EOS_STATE_BUTT + 1] = { "Null", "Wait", "Out", "NA" };
    td_char *eos_type_str[VOQ_EOS_TYPE_BUTT + 1] = { "Null", "Succ", "Fail", "ImageId", "NA" };

    chan = voq_get_chan(chan_id);
    if (chan == TD_NULL || chan->handle == TD_NULL) {
        os_dump_proc(buf, len, used, 0, "voq chan %d not exists.%-57s|\r\n", chan_id, " ");
        os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
        return;
    }

    eos_state = (chan->eos.state < VOQ_EOS_STATE_BUTT) ? chan->eos.state : VOQ_EOS_STATE_BUTT;
    eos_type = (chan->eos.type < VOQ_EOS_TYPE_BUTT) ? chan->eos.type : VOQ_EOS_TYPE_BUTT;

    empty = (chan->history >= chan->tail) ? chan->history - chan->tail : VOQ_IMAGE_NUM + chan->history - chan->tail;
    wait_get = (chan->tail >= chan->head) ? chan->tail - chan->head : VOQ_IMAGE_NUM + chan->tail - chan->head;
    wait_rel = (chan->head >= chan->history) ? chan->head - chan->history : VOQ_IMAGE_NUM + chan->head - chan->history;
    if (chan->head < VOQ_IMAGE_NUM && chan->img[chan->head] != TD_NULL) {
        head_fsp = (td_s32)chan->img[chan->head]->image_id;
    }

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chan_id", chan->id, "state", chan->state);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "un_id", chan->un_id, "view_num", chan->view_num);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "frm_total", chan->frm_total, "eos_state",
                 eos_state_str[eos_state]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_S, "frm_rate", chan->frm_rate, "eos_type",
                 eos_type_str[eos_type]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "rcv_try", chan->rcv_try, "rls_try", chan->rls_try);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "rcv_ok", chan->rcv_ok, "rls_ok", chan->rls_ok);

    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "tail", chan->tail, "empty", empty);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "head", chan->head, "wait_get", wait_get);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "history", chan->history, "wait_rel", wait_rel);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "new_frm", chan->new_frm_cnt, "head_fsp", head_fsp);
    os_dump_proc(buf, len, used, 0, LOG_FMT_D_N, "rpt_eos_cnt", chan->rpt_eos_cnt, " ");
    os_dump_proc(buf, len, used, 0, LOG_FMT_TAIL);
}

td_s32 voq_log_read(td_void *buf, td_s32 chan_id, td_s32 *used, td_s32 len)
{
    voq_entry *entry = TD_NULL;
    voq_state state_id;
    td_char *state_str[VOQ_STATE_BUTT + 1] = { "Null", "Open", "Init", "Exit", "NA" };

    vfmw_check_ret(buf != TD_NULL, EXT_FAILURE);

    voq_sema_down();

    entry = voq_get_entry();
    state_id = (entry->state < VOQ_STATE_BUTT) ? entry->state : VOQ_STATE_BUTT;

    os_dump_proc(buf, len, used, 0, LOG_VOQ_FMT_HEAD);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_S, "check1", entry->reserve1, "state", state_str[state_id]);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "check2", entry->reserve2, "ref_cnt", entry->ref_cnt);
    os_dump_proc(buf, len, used, 0, LOG_FMT_X_D, "ctx_size", entry->ctx_size, "max_image_num", VOQ_IMAGE_NUM);

    voq_log_read_chan(buf, chan_id, used, len);

    voq_sema_up();

    return EXT_SUCCESS;
}
