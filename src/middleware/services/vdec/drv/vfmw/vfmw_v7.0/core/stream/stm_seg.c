/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stm_seg.h"
#include "stm_base.h"
#include "stm_chan.h"
#include "stm_ctrl.h"
#include "syn_intf.h"
#include "event.h"
#include "stm_log.h"
#include "vfmw_define.h"

static inline td_s32 stm_seg_inc(td_s32 x, td_s32 size)
{
    if (size == 0) {
        return 0;
    }

    return (x + 1) % size;
}

static inline td_void stm_seg_set_state(stm_seg_info *seg, td_s8 seg_state)
{
    seg->seg_state = seg_state;
}

static inline td_s32 stm_seg_check_state(const stm_seg_info *seg, td_s8 seg_state)
{
    if (seg->seg_state != seg_state) {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static inline td_void stm_seg_clip_zero(td_s32 *x)
{
    if (x == TD_NULL) {
        return;
    }

    (*x) = ((*x) >= 0) ? (*x) : 0;
}

/* seg info init */
td_s32 stm_seg_init(td_s32 chan_id, td_s32 opt)
{
    seg_list *seg_list_info = TD_NULL;
    td_ulong lock_flag;

    stm_chan_lock(chan_id, &lock_flag);

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    seg_list_info->head = 0;
    seg_list_info->tail = 0;
    seg_list_info->history = 0;
    seg_list_info->seg_current = 0;
    seg_list_info->last_pts = VFMW_INVALID_PTS;

    seg_list_info->seg_total_size = 0;
    seg_list_info->seg_fresh_size = 0;
    seg_list_info->seg_fresh_num = 0;

    seg_list_info->first_start_flag = 1;
    seg_list_info->pre_lsb = 0xffffffff;
    seg_list_info->pre_msb = 0xffffffff;
    seg_list_info->seg_next_addr = 0;

    seg_list_info->full_flag = 0;
    seg_list_info->roll_flag = 0;
    seg_list_info->roll_addr = 0;
    seg_list_info->last_seg_addr = 0;
    seg_list_info->last_seg_size = 0;
    seg_list_info->insert_seg_num = 0;

    seg_list_info->seg_num_cnt = 0;
    seg_list_info->seg_size_cnt = 0;
    seg_list_info->recv_try_cnt = 0;
    seg_list_info->recv_ok_cnt = 0;
    seg_list_info->recv_fail_cnt = 0;
    seg_list_info->rels_try_cnt = 0;
    seg_list_info->rels_ok_cnt = 0;

    seg_list_info->seg_buf_read_addr = seg_list_info->seg_buf_phy_addr;
    seg_list_info->seg_buf_write_addr = seg_list_info->seg_buf_phy_addr;

    if (opt == 1) {
        /* tplay keep state for less seg */
        seg_list_info->last_h263_num = 0;
        seg_list_info->mpeg4_mode_flag = 0;
    }

    stm_chan_unlock(chan_id, &lock_flag);

    return STM_OK;
}

/* check if seglist is full */
td_bool stm_seg_is_full(const seg_list *seg_list_info)
{
    td_s32 tail_add1;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();
    if (entry->stm_seg_full == 1) {
        return TD_TRUE;
    }

    vfmw_check_ret(seg_list_info != TD_NULL, TD_FALSE);
    tail_add1 = stm_seg_inc(seg_list_info->tail, MAX_SEG_NUM);

    return tail_add1 == seg_list_info->history ? TD_TRUE : TD_FALSE;
}

td_bool stm_seg_has_fresh(const seg_list *seg_list_info)
{
    vfmw_check_ret(seg_list_info != TD_NULL, TD_FALSE);

    return seg_list_info->head == seg_list_info->tail ? TD_FALSE : TD_TRUE;
}

STATIC td_bool stm_seg_empty(const seg_list *seg_list_info)
{
    vfmw_check_ret(seg_list_info != TD_NULL, TD_FALSE);

    return seg_list_info->history == seg_list_info->tail ? TD_TRUE : TD_FALSE;
}

STATIC td_void stm_seg_check_data_h264_score(stm_chan *chan, td_u8 data)
{
    if (((data & 0x1f) >= 1 && (data & 0x1f) <= 13) || /* 0x1f is mask. 13 is specific start code */
        ((data & 0x1f) == 19) || ((data & 0x1f) == 30) || /* 0x1f is mask. 19 30 is specific start code */
        ((data & 0x1f) == 31)) { /* 0x1f is mask. 31 is specific start code */
        chan->chk_param.h264_score++;
    }
}

STATIC td_void stm_seg_check_data_mpeg2_score(stm_chan *chan, td_u8 data)
{
    if ((data == 0x0) || (data == 0xb1) || (data == 0xb2) ||  /* 0x0 0xb1 0xb2 is specific start code */
        (data == 0xb3) || (data == 0xb4) || (data == 0xb5) || /* 0xb3 0xb4 0xb5 is specific start code */
        (data >= 0xb7)) { /* 0xb7 0xb8 0xb9 0xba 0xbb 0xff is specific start code */
        chan->chk_param.mpeg2_score++;
    }
}

STATIC td_void stm_seg_check_data(stm_chan *chan, td_u8 data)
{
    vfmw_check(chan != TD_NULL);

    stm_seg_check_data_h264_score(chan, data);
    stm_seg_check_data_mpeg2_score(chan, data);
}

STATIC td_void stm_seg_check_start_code_cnt(stm_chan *chan, td_u8 data)
{
    vfmw_check(chan != TD_NULL);

    if ((chan->chk_param.find_start_code_cnt >= 3) && ((data & 0x1f) >= 1)) { /* 3 is threshold 0x1f is mask */
        if ((((chan->chk_param.current_start_code - chan->chk_param.last_one_start_code == 1) &&
              (chan->chk_param.last_one_start_code - chan->chk_param.last_two_start_code == 1)) ||
             ((chan->chk_param.current_start_code == chan->chk_param.last_one_start_code) &&
              (chan->chk_param.last_one_start_code == chan->chk_param.last_two_start_code))) &&
            (chan->chk_param.current_start_code != 1) &&
            (chan->chk_param.current_start_code != 0x14)) { /* 0x14 is specific start code */
            if (chan->chk_param.h264_score > 0) {
                chan->chk_param.h264_score--;
            }

            chan->chk_param.mpeg2_score++;
        }
    }
}

STATIC td_void stm_seg_check_stream(stm_chan *chan, const td_u8 *seg_buf_addr)
{
    td_u8 data = 0;

    vfmw_check(seg_buf_addr != TD_NULL);
    vfmw_check(chan != TD_NULL);

    if ((chan->cfg.vid_std == VFMW_H264) || (chan->cfg.vid_std == VFMW_MPEG2)) {
        vfmw_check_no_print(chan->chk_param.find_start_code_cnt < 200); /* 200 is the threshold */

        if (((*seg_buf_addr) == 0) && ((*(seg_buf_addr + 1)) == 0) &&
            ((*(seg_buf_addr + 2)) == 1)) { /* 2 if offset */
            data = (*(seg_buf_addr + 3)); /* 3 is offset */
        } else if (((*seg_buf_addr) == 0) && ((*seg_buf_addr + 1) == 0) &&
                   ((*seg_buf_addr + 2) == 0) && ((*seg_buf_addr + 3) == 1)) { /* 2 3 is offset */
            data = (*(seg_buf_addr + 4)); /* 4 is offset */
        }

        chan->chk_param.find_start_code_cnt++;
        chan->chk_param.last_two_start_code = chan->chk_param.last_one_start_code;
        chan->chk_param.last_one_start_code = chan->chk_param.current_start_code;
        chan->chk_param.current_start_code = (data & 0x1f); /* 0x1f is mask */

        stm_seg_check_data(chan, data);
        stm_seg_check_start_code_cnt(chan, data);

        if (chan->chk_param.find_start_code_cnt >= 200) { /* 200 is threshold */
            if (chan->chk_param.h264_score - chan->chk_param.mpeg2_score >= 50 &&  /* 50 is threshold */
                chan->cfg.vid_std == VFMW_MPEG2) {
                chan->std_err = TD_TRUE;
                report_err_std(chan->id, VFMW_H264);
            }

            if (chan->chk_param.mpeg2_score - chan->chk_param.h264_score >= 50 &&  /* 50 is threshold */
                chan->cfg.vid_std == VFMW_H264) {
                chan->std_err = TD_TRUE;
                report_err_std(chan->id, VFMW_MPEG2);
            }
        }
    }
}

STATIC td_s32 stm_seg_check_valid(td_s32 chan_id, const stm_seg_info *seg_packet)
{
    td_u32 seg_free_size;
    seg_list *seg_list_info = TD_NULL;

    vfmw_check_ret(seg_packet != TD_NULL, STM_ERR);
    vfmw_check_ret(seg_packet->phy_addr != 0, STM_ERR);
    vfmw_check_ret(seg_packet->vir_addr != TD_NULL, STM_ERR);

    if (STM_CTRL_GET_CHAN(chan_id)->cfg.seg_bypass == 0) {
        seg_free_size = (td_u32)stm_seg_get_free_size(chan_id);
        vfmw_check_ret(seg_packet->length <= seg_free_size, STM_ERR);

        seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
        vfmw_check_ret(seg_packet->phy_addr >= seg_list_info->seg_buf_phy_addr, STM_ERR);
        vfmw_check_ret(seg_packet->phy_addr + seg_packet->length <=
            seg_list_info->seg_buf_phy_addr + (td_u32)seg_list_info->seg_buf_size, STM_ERR);
    }

    return STM_OK;
}

/* insert one seg packet to seglist */
td_s32 stm_seg_insert(td_s32 chan_id, const stm_seg_info *seg)
{
    td_s32 ret = STM_ERR;
    td_s32 fresh_num = 0;
    stm_seg_info *tail = TD_NULL;
    seg_list *seg_list_info = TD_NULL;

    vfmw_check_ret((seg != TD_NULL) && (seg->vir_addr != TD_NULL), STM_ERR);
    vfmw_check_ret(os_get_time_us != TD_NULL, STM_ERR);

    if (stm_seg_check_valid(chan_id, seg) != STM_OK) {
        goto out;
    }

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    if (seg_list_info->tail < 0 || seg_list_info->tail >= MAX_SEG_NUM || stm_seg_is_full(seg_list_info) == TD_TRUE) {
        goto out;
    }

#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_cache(seg->vir_addr, seg->phy_addr, seg->length);
#endif
    stm_seg_check_stream(STM_CTRL_GET_CHAN(chan_id), seg->vir_addr);

    fresh_num = seg_list_info->seg_fresh_num;

    tail = &seg_list_info->seg[seg_list_info->tail];
    vfmw_check_sec_func(memcpy_s(tail, sizeof(*tail), seg, sizeof(stm_seg_info)));
    tail->stream_id = seg_list_info->tail;
    tail->ref_cnt = 0;
    tail->rls_cnt = 0;
    stm_seg_set_state(tail, SEG_STATE_NEW);

    seg_list_info->tail = stm_seg_inc(seg_list_info->tail, MAX_SEG_NUM);

    if (seg->cut_result != SEG_CUT_HALF) {
        seg_list_info->seg_buf_write_addr = seg->phy_addr + seg->length;
        seg_list_info->seg_buf_write_vir_addr = seg->vir_addr + seg->length;
        seg_list_info->seg_total_size += (td_s32)seg->length;
        seg_list_info->seg_fresh_size += (td_s32)seg->length;
    }

    seg_list_info->seg_fresh_num++;
    seg_list_info->seg_num_cnt++;
    seg_list_info->seg_size_cnt += seg->length;

    if (seg->is_stream_end == 1) {
        stm_chan *chan = STM_CTRL_GET_CHAN(chan_id);
        chan->eos = STM_EOS_INSERT;
    }

    if (dcheck(PRN_TIME)) {
        tail->que_seg_time = os_get_time_us();
    }

    ret = STM_OK;

out:

    if (fresh_num == 0) {
        syn_wake_up();
    }

    return ret;
}

STATIC td_void stm_seg_save_file(td_s32 chan_id, const stm_seg_info *seg, td_s32 ret)
{
#ifdef VDEC_PROC_SUPPORT
    if (ret == STM_OK) {
        stm_log_save_seg(chan_id, seg);
    }
#else
    vfmw_unused(chan_id);
    vfmw_unused(seg);
    vfmw_unused(ret);
#endif
}

STATIC td_s32 stm_seg_recvive_proc(td_s32 chan_id, td_u32 seg_id, stm_seg_info *recv, seg_list *seg_list_info,
    stm_seg_info *seg)
{
    td_s32 ret = STM_ERR;
    stm_entry *entry = TD_NULL;

    vfmw_check_ret(os_get_time_us != TD_NULL, STM_ERR);

    entry = stm_ctrl_get_entry();
    if (entry->stm_no_seg == 1) {
        seg_list_info->seg_fresh_num = 0;
        return STM_ERR;
    }

    if (recv != TD_NULL) {
        stm_seg_set_state(recv, SEG_STATE_USE);
        vfmw_check_sec_func(memcpy_s(seg, sizeof(stm_seg_info), recv, sizeof(*recv)));

        seg_list_info->head = stm_seg_inc((td_s32)seg_id, MAX_SEG_NUM);
        seg_list_info->seg_fresh_num--;

        if (seg->cut_result != SEG_CUT_HALF) {
            seg_list_info->seg_fresh_size -= (td_s32)recv->length;
        }

        stm_seg_clip_zero(&seg_list_info->seg_fresh_num);
        stm_seg_clip_zero(&seg_list_info->seg_fresh_size);

        seg_list_info->recv_ok_cnt++;
        seg_list_info->recv_fail_cnt = 0;
        if (seg->is_stream_end == 1) {
            STM_CTRL_GET_CHAN(chan_id)->eos = STM_EOS_PARSING;
        }

        if (dcheck(PRN_TIME)) {
            seg->deq_seg_time = os_get_time_us();
        }

        ret = STM_OK;
    } else {
        seg_list_info->recv_fail_cnt++;
    }

    return ret;
}

/* get one seg packset from seglist and mark it */
td_s32 stm_seg_receive(td_s32 chan_id, stm_seg_info *seg)
{
    td_s32 ret;
    td_u32 seg_id;
    td_s32 run_cnt = 0;
    stm_seg_info *recv = TD_NULL;
    seg_list *seg_list_info = TD_NULL;
    td_ulong lock_flag;
    stm_entry *entry = TD_NULL;

    entry = stm_ctrl_get_entry();
    if (entry->stm_seg_full == 1) {
        return STM_ERR;
    }

    vfmw_check_ret(seg != TD_NULL, STM_ERR);
    stm_chan_lock(chan_id, &lock_flag);

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    for (seg_id = (td_u32)seg_list_info->head;;) {
        if (run_cnt++ >= MAX_SEG_NUM) { /* for csec */
            break;
        }

        if ((td_s32)seg_id == seg_list_info->tail) {
            break;
        }

        if (stm_seg_check_state(&seg_list_info->seg[seg_id], SEG_STATE_NEW) == EXT_SUCCESS) {
            recv = &seg_list_info->seg[seg_id];
            break;
        }

        seg_id = (td_u32)stm_seg_inc((td_s32)seg_id, MAX_SEG_NUM);
    }

    seg_list_info->recv_try_cnt++;

    ret = stm_seg_recvive_proc(chan_id, seg_id, recv, seg_list_info, seg);

    seg->seg_base_phy_addr = seg_list_info->seg_buf_phy_addr;

    stm_chan_unlock(chan_id, &lock_flag);

    stm_seg_save_file(chan_id, seg, ret);

    return ret;
}

/* update seglist head and rdaddr for next scd start */
STATIC td_s32 stm_seg_update(seg_list *seg_list_info, td_s32 stream_id, td_s32 chan_id)
{
    td_u32 seg_id;
    td_s32 run_cnt = 0;
    stm_seg_info *seg = TD_NULL;

    vfmw_unused(stream_id);
    vfmw_check_ret(seg_list_info != TD_NULL, STM_ERR);
    vfmw_check_ret(chan_id >= 0 && chan_id < STM_CHAN_NUM, STM_ERR);

    for (seg_id = (td_u32)seg_list_info->history;;) {
        if (run_cnt++ > MAX_SEG_NUM) { /* for codex */
            break;
        }

        seg = seg_list_info->seg + seg_id;
        if ((td_s32)seg_id == seg_list_info->head) {
            seg_list_info->history = (td_s32)seg_id;
            break;
        } else if (stm_seg_check_state(&seg_list_info->seg[seg_id], SEG_STATE_IDLE) == (td_s32)EXT_FAILURE) {
            seg_list_info->history = (td_s32)seg_id;
            break;
        } else {
            seg_list_info->history = (td_s32)seg_id;
            if (seg->cut_result != SEG_CUT_HALF) {
                seg_list_info->seg_buf_read_addr = seg->phy_addr + seg->length;
            }
        }

        if (STM_CTRL_GET_CHAN(chan_id)->cfg.seg_bypass == 1) {
            stm_ctrl_delete_raw(chan_id);
        }

        seg_id = (td_u32)stm_seg_inc((td_s32)seg_id, MAX_SEG_NUM);
    }

    return STM_OK;
}

STATIC stm_seg_info* stm_seg_find_seg(seg_list *seg_list_info, td_s32 stream_id)
{
    td_u32 seg_id;
    td_s32 run_cnt = 0;
    stm_seg_info *seg = TD_NULL;

    vfmw_check_ret(seg_list_info != TD_NULL, TD_NULL);

    for (seg_id = (td_u32)seg_list_info->history;;) {
        if (run_cnt++ >= MAX_SEG_NUM) { /* for csec */
            break;
        }

        if (seg_id == (td_u32)seg_list_info->head) {
            break;
        }

        if (seg_id == (td_u32)stream_id) {
            if (stm_seg_check_state(&seg_list_info->seg[seg_id], SEG_STATE_USE) == EXT_SUCCESS) {
                seg = &seg_list_info->seg[seg_id];
            }
            break;
        }

        seg_id = (td_u32)stm_seg_inc((td_s32)seg_id, MAX_SEG_NUM);
    }

    return seg;
}

/* release one seg packet from seglist */
td_s32 stm_seg_release(td_s32 chan_id, td_s32 stream_id)
{
    td_s32 ret = STM_ERR;
    td_u32 seg_id;
    td_s32 cnt;
    stm_seg_info *seg = TD_NULL;
    seg_list *seg_list_info = TD_NULL;
    stm_chan *chan  = TD_NULL;
    td_ulong lock_flag;

    stm_chan_lock(chan_id, &lock_flag);

    chan = STM_CTRL_GET_CHAN(chan_id);
    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;

    seg = stm_seg_find_seg(seg_list_info, stream_id);
    if (seg == TD_NULL) {
        goto out;
    }

    seg_list_info->rels_try_cnt++;

    seg->rls_cnt++;
    if (seg->rls_cnt < seg->ref_cnt) {
        ret = STM_OK;
        goto out;
    }

    stm_seg_set_state(seg, SEG_STATE_IDLE);
    if (seg->cut_result != SEG_CUT_HALF) {
        seg_list_info->seg_total_size -= (td_s32)seg->length;
    }
    stm_seg_clip_zero(&seg_list_info->seg_total_size);

    if (seg_list_info->history == stream_id) {
        stm_seg_update(seg_list_info, stream_id, chan_id);

        if (chan->rls_raw_wait == 1 && seg_list_info->history != stream_id) {
            seg_id = (td_u32)((seg_list_info->history + MAX_SEG_NUM - 1) % MAX_SEG_NUM);
            chan->rls_raw_idx = seg_list_info->seg[seg_id].raw_idx;
            if (seg_list_info->history == seg_list_info->head) {
                chan->rls_raw_idx = (chan->rls_raw_idx + 1) % MAX_RAW_NUM;
            }

            if (seg->is_stream_end == 1 && chan->cfg.scd_bypass == 1 && chan->cfg.seg_bypass == 0) {
                cnt = (td_s32)((chan->rls_raw_idx - (td_u32)chan->raw_list->history + MAX_RAW_NUM) % MAX_RAW_NUM);
                chan->raw_list->need_release = (cnt > 0) ? 1 : chan->raw_list->need_release;
            }
        }
    }

    seg_list_info->rels_ok_cnt++;
    if (seg->is_stream_end == 1) {
        chan->eos = STM_EOS_DONE;
    }

    ret = STM_OK;

out:
    stm_chan_unlock(chan_id, &lock_flag);

    return ret;
}

/* force release first seg packet */
td_s32 stm_seg_release_first(td_s32 chan_id)
{
    td_s32 ret;
    seg_list *seg_list_info = TD_NULL;

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    if (seg_list_info->history == seg_list_info->head) {
        return STM_ERR;
    }

    ret = stm_seg_release(chan_id, seg_list_info->history);

    return ret;
}

td_u32 stm_seg_get_min_buf_size(td_void)
{
    return SEG_TOP_BLANK_SIZE + SEG_BTM_BLANK_SIZE + SEG_BUF_MIN_SIZE;
}

/* config seg buffer info */
td_s32 stm_seg_config_buf(td_s32 chan_id, const vfmw_mem_desc *mem)
{
    UADDR phy_addr_align;
    td_s32 seg_blank_size;
    td_u32 addr_offset;
    seg_list *seg_list_info = TD_NULL;

    vfmw_check_ret(mem != TD_NULL, STM_ERR);
    seg_blank_size = SEG_TOP_BLANK_SIZE;
    phy_addr_align = vfmw_align_up(mem->phy_addr, 4); /* 4 is align len */
    phy_addr_align += (td_u32)seg_blank_size;
    addr_offset = phy_addr_align - mem->phy_addr;

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    seg_list_info->seg_dma_buf = mem->dma_buf;
    seg_list_info->seg_buf_phy_addr = phy_addr_align;
    seg_list_info->seg_buf_vir_addr = uint64_ptr(mem->vir_addr + addr_offset);
    seg_list_info->seg_buf_size = (td_s32)(mem->length - addr_offset - SEG_BTM_BLANK_SIZE);
    if (seg_list_info->seg_buf_size < SEG_BUF_MIN_SIZE) {
        return STM_ERR;
    }

    seg_list_info->seg_buf_read_addr = seg_list_info->seg_buf_phy_addr;
    seg_list_info->seg_buf_write_addr = seg_list_info->seg_buf_phy_addr;
    seg_list_info->seg_buf_write_vir_addr = seg_list_info->seg_buf_vir_addr;

    return STM_OK;
}

STATIC td_s32 stm_seg_get_recv_num(td_s32 chan_id)
{
    seg_list *seg_list_info = TD_NULL;

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    vfmw_check_ret(seg_list_info != TD_NULL, 0);

    return (seg_list_info->history + MAX_SEG_NUM - 1 -
        seg_list_info->head) % MAX_SEG_NUM;
}

td_s32 stm_seg_get_free_num(td_s32 chan_id)
{
    seg_list *seg_list_info = TD_NULL;

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    vfmw_check_ret(seg_list_info != TD_NULL, 0);

    return (seg_list_info->history + MAX_SEG_NUM - 1 -
        seg_list_info->tail) % MAX_SEG_NUM;
}

td_s32 stm_seg_get_free_size(td_s32 chan_id)
{
    td_s32 seg_buff_free_size;
    td_s32 tmp_size;
    seg_list *seg_list_info = TD_NULL;

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    vfmw_check_ret(seg_list_info != TD_NULL, 0);

    if (seg_list_info->seg_buf_write_addr > seg_list_info->seg_buf_read_addr) {
        tmp_size = (td_s32)(seg_list_info->seg_buf_write_addr - seg_list_info->seg_buf_read_addr);
        seg_buff_free_size = seg_list_info->seg_buf_size - tmp_size - 1;
    } else if (seg_list_info->seg_buf_write_addr < seg_list_info->seg_buf_read_addr) {
        tmp_size = (td_s32)(seg_list_info->seg_buf_read_addr - seg_list_info->seg_buf_write_addr);
        seg_buff_free_size = tmp_size - 1;
    } else {
        seg_buff_free_size = seg_list_info->seg_buf_size - 1;
    }

    return seg_buff_free_size;
}

td_s32 stm_seg_get_fresh_num(td_s32 chan_id)
{
    td_ulong lock_flag;
    td_s32 fresh_num;
    seg_list *seg_list_info = TD_NULL;

    fresh_num = 0;

    stm_chan_lock(chan_id, &lock_flag);

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    vfmw_check_goto(seg_list_info != TD_NULL, out);

    fresh_num = seg_list_info->seg_fresh_num;

out:
    stm_chan_unlock(chan_id, &lock_flag);

    return fresh_num;
}

td_s32 stm_seg_get_fresh_size(td_s32 chan_id)
{
    td_ulong lock_flag;
    td_s32 fresh_size;
    seg_list *seg_list_info = TD_NULL;

    fresh_size = 0;

    stm_chan_lock(chan_id, &lock_flag);

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    vfmw_check_goto(seg_list_info != TD_NULL, out);

    fresh_size = seg_list_info->seg_fresh_size;

out:
    stm_chan_unlock(chan_id, &lock_flag);

    return fresh_size;
}

STATIC td_s32 stm_seg_link_fill_seg_info(stm_chan *chan, stm_seg_info *seg)
{
    td_s32 idx, src_len, offset;
    td_u64 src_vir;

    vfmw_check_ret(chan != TD_NULL, STM_ERR);
    vfmw_check_ret(seg != TD_NULL, STM_ERR);
    vfmw_check_ret(chan->raw_list != TD_NULL, STM_ERR);

    idx = chan->raw_list->head;
    if (idx < 0 || idx >= MAX_RAW_NUM) {
        dprint(PRN_ERROR, "%s raw list head(%d) error\n", __func__, idx);
        return STM_ERR;
    }

    if (idx == chan->raw_list->tail) {
        stm_chan_print_block(chan->id, "no raw");
        return STM_ERR;
    }

    src_vir = chan->raw_list->raw[idx].vir_addr;
    src_len = (td_s32)chan->raw_list->raw[idx].length;

    if ((chan->raw_list->raw[idx].is_not_last_packet_flag != 0) || (src_vir == 0)) {
        chan->raw_list->delete_len = (td_u32)src_len;
        stm_raw_delete(chan->id);
        dprint(PRN_ERROR, "Not support discontinue raw or src_vir is TD_NULL.\n");
        return STM_ERR;
    }

    offset = (td_s32)(src_vir - chan->raw_list->raw[idx].vir_addr);
    seg->phy_addr = chan->raw_list->raw[idx].phy_addr + (td_u32)offset;
    seg->vir_addr = uint64_ptr(src_vir);
    seg->length = (td_u32)src_len;
    seg->is_last_seg = 1;
    seg->pts = chan->raw_list->raw[idx].pts;
    seg->user_tag = chan->raw_list->raw[idx].user_tag;
    seg->is_stream_end = chan->raw_list->raw[idx].is_stream_end_flag;

    seg->disp_time = chan->raw_list->raw[idx].disp_time;
    seg->disp_enable_flag = chan->raw_list->raw[idx].disp_enable_flag;
    seg->disp_frame_distance = chan->raw_list->raw[idx].disp_frame_distance;
    seg->distance_beforce_first_frame = chan->raw_list->raw[idx].distance_before_first_frame;
    seg->gop_num = chan->raw_list->raw[idx].gop_num;

    chan->raw_list->head = (chan->raw_list->head + 1) % MAX_RAW_NUM;

    return STM_OK;
}

STATIC td_s32 stm_seg_link_raw_to_seg(stm_chan *chan, stm_seg_info *seg)
{
    vfmw_check_ret(chan != TD_NULL, STM_ERR);
    vfmw_check_ret(seg != TD_NULL, STM_ERR);

    if (stm_seg_link_fill_seg_info(chan, seg) != STM_OK) {
        return STM_ERR;
    }

    if (stm_seg_insert(chan->id, seg) != STM_OK) {
        return STM_ERR;
    }

    return STM_OK;
}

STATIC td_s32 stm_seg_get_write_addr(stm_chan *chan, UADDR *phy_addr, td_u8 **vir_addr, td_s32 raw_len)
{
    td_ulong lock_flag;

    vfmw_check_ret(chan != TD_NULL, STM_ERR);
    vfmw_check_ret(phy_addr != TD_NULL, STM_ERR);
    vfmw_check_ret(vir_addr != TD_NULL, STM_ERR);

    stm_chan_lock(chan->id, &lock_flag);

    if (chan->seg_list_info == TD_NULL) {
        stm_chan_unlock(chan->id, &lock_flag);
        return STM_ERR;
    }

    /* Base............Read............Write */
    if (chan->seg_list_info->seg_buf_read_addr <= chan->seg_list_info->seg_buf_write_addr) {
        if (((td_u64)raw_len) > ((UADDR)chan->seg_list_info->seg_buf_size - (chan->seg_list_info->seg_buf_write_addr -
            chan->seg_list_info->seg_buf_phy_addr))) {
            /* if the Read == write means that seg buffer total size is zero */
            if (((td_u64)raw_len) < (chan->seg_list_info->seg_buf_read_addr - chan->seg_list_info->seg_buf_phy_addr)) {
                chan->seg_list_info->seg_buf_write_addr = chan->seg_list_info->seg_buf_phy_addr;
                chan->seg_list_info->seg_buf_write_vir_addr = chan->seg_list_info->seg_buf_vir_addr;
            } else if (chan->seg_list_info->seg_buf_read_addr == chan->seg_list_info->seg_buf_write_addr) {
                chan->seg_list_info->seg_buf_read_addr = chan->seg_list_info->seg_buf_phy_addr;
                chan->seg_list_info->seg_buf_write_addr = chan->seg_list_info->seg_buf_phy_addr;
                chan->seg_list_info->seg_buf_write_vir_addr = chan->seg_list_info->seg_buf_vir_addr;
            } else {
                stm_chan_unlock(chan->id, &lock_flag);
                dprint(PRN_ERROR, "rawlen1(%x) too big, read addr:0x%x, write addr:0x%x\n",
                       raw_len, chan->seg_list_info->seg_buf_read_addr, chan->seg_list_info->seg_buf_write_addr);
                return STM_ERR;
            }
        }
    } else {
        /* Base............Write............Read */
        if (((td_u64)raw_len) > (chan->seg_list_info->seg_buf_read_addr - chan->seg_list_info->seg_buf_write_addr)) {
            stm_chan_unlock(chan->id, &lock_flag);
            dprint(PRN_ERROR, "rawlen2(%x) too big, read addr:0x%x, write addr:0x%x\n",
                   raw_len, chan->seg_list_info->seg_buf_read_addr, chan->seg_list_info->seg_buf_write_addr);
            return STM_ERR;
        }
    }

    (*phy_addr) = chan->seg_list_info->seg_buf_write_addr;
    (*vir_addr) = chan->seg_list_info->seg_buf_write_vir_addr;

    stm_chan_unlock(chan->id, &lock_flag);

    return STM_OK;
}

STATIC td_s32 stm_seg_fill_seg_info(const stm_chan *chan, stm_seg_info *seg, const td_s32 *raw_num,
    const UADDR *phy_addr, td_u8 **vir_addr)
{
    td_s32 idx, src_len, all_len, free_len, cnt;
    td_u64 src_vir;

    vfmw_check_ret(seg != TD_NULL && raw_num != TD_NULL, STM_ERR);
    vfmw_check_ret(phy_addr != TD_NULL && vir_addr != TD_NULL, STM_ERR);
    vfmw_check_ret(chan != TD_NULL && chan->raw_list != TD_NULL && chan->seg_list_info != TD_NULL, STM_ERR);

    idx = chan->raw_list->head;
    if (idx < 0 || idx >= MAX_RAW_NUM) {
        dprint(PRN_ERROR, "%s rawlist head(%d) error\n", __func__, idx);
        return STM_ERR;
    }

    src_vir = chan->raw_list->raw[idx].vir_addr;
    src_len = (td_s32)chan->raw_list->raw[idx].length;

    seg->phy_addr = *phy_addr;
    seg->vir_addr = *vir_addr;
    seg->length = 0;
    seg->is_last_seg = 1;
    seg->raw_idx = (td_u32)idx;
    seg->pts = chan->raw_list->raw[idx].pts;
    seg->user_tag = chan->raw_list->raw[idx].user_tag;
    seg->is_stream_end = chan->raw_list->raw[idx].is_stream_end_flag;

    seg->disp_time = chan->raw_list->raw[idx].disp_time;
    seg->disp_enable_flag = chan->raw_list->raw[idx].disp_enable_flag;
    seg->disp_frame_distance = chan->raw_list->raw[idx].disp_frame_distance;
    seg->distance_beforce_first_frame = chan->raw_list->raw[idx].distance_before_first_frame;
    seg->gop_num = chan->raw_list->raw[idx].gop_num;

    all_len = 0;
    for (cnt = 0; cnt < *raw_num; cnt++) {
        free_len = chan->seg_list_info->seg_buf_size - all_len;
        if (src_vir == 0 || free_len <= 0) {
            return STM_ERR;
        }

        vfmw_check_sec_func(memcpy_s((*vir_addr), (td_u32)free_len, uint64_ptr(src_vir), (td_u32)src_len));
        (*vir_addr) += src_len;
        all_len += src_len;
        idx = stm_seg_inc(idx, MAX_RAW_NUM);
        src_vir = chan->raw_list->raw[idx].vir_addr;
        src_len = (td_s32)chan->raw_list->raw[idx].length;
    }

    seg->length = (td_u32)all_len;

    return STM_OK;
}

STATIC td_s32 stm_seg_copy_raw_to_seg(stm_chan *chan, stm_seg_info *seg)
{
    td_s32 ret, raw_num, raw_len;
    td_u8 *vir_addr = TD_NULL;
    UADDR phy_addr;

    vfmw_check_ret(chan != TD_NULL, STM_ERR);
    vfmw_check_ret(seg != TD_NULL, STM_ERR);
    vfmw_check_ret(chan->raw_list != TD_NULL, STM_ERR);
    vfmw_check_ret(chan->seg_list_info != TD_NULL, STM_ERR);

    ret = stm_raw_get_raw_num_in_frm(chan->id, &raw_num, &raw_len);
    if (ret != STM_OK) {
        stm_chan_print_block(chan->id, "no raw");
        return STM_ERR;
    }

    if (raw_num <= 0 || raw_num >= MAX_RAW_NUM) {
        dprint(PRN_ERROR, "raw_num(%d) error\n", raw_num);
        return STM_ERR;
    }

    if (raw_len > chan->seg_list_info->seg_buf_size) {
        dprint(PRN_ERROR, "raw_len(%d) too big\n", raw_len);
        chan->raw_list->delete_len = (td_u32)raw_len;
        stm_raw_delete(chan->id);
        return STM_ERR;
    }

    ret = stm_seg_get_write_addr(chan, &phy_addr, &vir_addr, raw_len);
    if (ret != STM_OK) {
        dprint(PRN_ERROR, "get write addr fail!\n");
        return STM_ERR;
    }

    ret = stm_seg_fill_seg_info(chan, seg, &raw_num, &phy_addr, &vir_addr);
    if (ret != STM_OK) {
        dprint(PRN_ERROR, "fill seg info fail!\n");
        return STM_ERR;
    }

    if (stm_seg_insert(chan->id, seg) != STM_OK) {
        return STM_ERR;
    }

    chan->raw_list->delete_len = (td_u32)raw_len;
    stm_raw_delete(chan->id);

    return STM_OK;
}

td_s32 stm_seg_raw_to_seg(td_s32 chan_id, stm_seg_info *seg)
{
    td_s32 ret;
    stm_chan *chan = TD_NULL;

    vfmw_check_ret(seg != TD_NULL, STM_ERR);

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->cfg.scd_bypass == 0) {
        return STM_OK;
    }

    if (chan->cfg.seg_bypass == 1) {
        ret = stm_seg_link_raw_to_seg(chan, seg);
    } else {
        ret = stm_seg_copy_raw_to_seg(chan, seg);
    }

    return ret;
}

STATIC td_void stm_seg_reset_block_timer(seg_list *seg_list_info)
{
    vfmw_check(seg_list_info != TD_NULL);
    vfmw_check(os_get_time_ms != TD_NULL);

    seg_list_info->block_time_in_ms = 0;
    seg_list_info->last_block_time = os_get_time_ms();
    seg_list_info->block_seg_num = seg_list_info->seg_num_cnt;
}

STATIC td_u32 stm_seg_run_block_timer(seg_list *seg_list_info)
{
    td_u32 cur_time;

    vfmw_check_ret(os_get_time_ms != TD_NULL, STM_ERR);
    cur_time = os_get_time_ms();

    vfmw_check_ret(seg_list_info != TD_NULL, cur_time);

    if (cur_time > seg_list_info->last_block_time) {
        seg_list_info->block_time_in_ms = cur_time - seg_list_info->last_block_time;
    } else {
        seg_list_info->block_time_in_ms = 0;
        seg_list_info->last_block_time = cur_time;
    }

    return seg_list_info->block_time_in_ms;
}

STATIC td_void stm_seg_force_release(td_s32 chan_id, seg_list *seg_list_info)
{
    td_s32 i;
    td_s32 rels_num;

    vfmw_check(seg_list_info != TD_NULL);

    if (seg_list_info->seg_fresh_size == 0 && seg_list_info->last_seg_size > 0) {
        rels_num = stm_seg_get_recv_num(chan_id) / 3; /* 3 is num */
        for (i = 0; i < rels_num; i++) {
            (td_void)stm_seg_release(chan_id, seg_list_info->history);
        }
        if (stm_seg_empty(seg_list_info)) {
            seg_list_info->last_seg_size = 0;
        }
        dprint(PRN_ERROR, "seg block, and no fresh seg, rels %d seg anyway!\n", rels_num);
    } else {
        dprint(PRN_ERROR, "seg block, rels first seg anyway!\n");
        (td_void)stm_seg_release_first(chan_id);
    }
}

STATIC td_bool stm_seg_is_free_enough(td_s32 chan_id, const seg_list *seg_list_info)
{
    td_s32 seg_free_num = stm_seg_get_free_num(chan_id);
    td_s32 seg_free_size = stm_seg_get_free_size(chan_id);
    const td_u32 seg_len_align = 128; /* seg len align to 128 */

    vfmw_check_ret(seg_list_info != TD_NULL, TD_FALSE);
    seg_free_size -= (td_s32)(seg_list_info->last_seg_size + seg_len_align);

    if (seg_free_num < (MAX_SEG_NUM / 4) || /* 4 is num */
        seg_free_size < (seg_list_info->seg_buf_size / 8)) { /* 8 is num */
        return TD_FALSE;
    } else {
        return TD_TRUE;
    }
}

td_s32 stm_seg_check(td_s32 chan_id)
{
    seg_list *seg_list_info = TD_NULL;
    td_u32 wait_time;
    const td_u32 max_block_time = 1000;

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    if (stm_seg_is_free_enough(chan_id, seg_list_info) == TD_TRUE) {
        seg_list_info->is_counting = 0;
        return STM_OK;
    }

#ifdef VT_SUPPORT
    if (STM_CTRL_GET_CHAN(chan_id)->cfg.half_seg_en == 1) {
        /* for sed ldy test, cost seg slowly, avoid force release seg */
        return STM_ERR;
    }
#endif

    if (seg_list_info->is_counting == 0) {
        if (seg_list_info->seg_fresh_num < 128 && /* 128 is num */
            seg_list_info->seg_fresh_size < (seg_list_info->seg_buf_size / 2)) { /* 2 is num */
            seg_list_info->is_counting = 1;
            stm_seg_reset_block_timer(seg_list_info);
        }
        return STM_ERR;
    }

    wait_time = stm_seg_run_block_timer(seg_list_info);
    if (wait_time > max_block_time &&
        seg_list_info->insert_seg_num == 0 &&
        seg_list_info->block_seg_num == seg_list_info->seg_num_cnt) {
        stm_seg_force_release(chan_id, seg_list_info);
        seg_list_info->is_counting = 0;
    }

    if (stm_seg_is_free_enough(chan_id, seg_list_info) == TD_FALSE) {
        return STM_ERR;
    }

    return STM_OK;
}

td_s32 stm_seg_set_ref_cnt(td_s32 chan_id, td_s32 seg_id, td_s32 ref_cnt)
{
    td_s32 ret = STM_ERR;
    stm_seg_info *seg = TD_NULL;
    seg_list *seg_list_info = TD_NULL;
    td_ulong lock_flag;

    stm_chan_lock(chan_id, &lock_flag);

    seg_list_info = STM_CTRL_GET_CHAN(chan_id)->seg_list_info;
    seg = &seg_list_info->seg[seg_id];

    if (seg != TD_NULL) {
        if (ref_cnt == 0) {
            seg->ref_cnt = 0;
            seg->rls_cnt = 0;
        } else {
            seg->ref_cnt += (td_u8)ref_cnt;
        }

        ret = STM_OK;
    }

    stm_chan_unlock(chan_id, &lock_flag);

    return ret;
}
