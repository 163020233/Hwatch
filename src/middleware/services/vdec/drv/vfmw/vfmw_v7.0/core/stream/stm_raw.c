/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stm_raw.h"
#include "stm_base.h"
#include "stm_chan.h"
#include "stm_ctrl.h"
#include "event.h"
#include "vfmw_define.h"

static td_u8 g_eop_data_flag[][20] = { /* 6 is group num, 20 is byte num */
    /* H264 & MVC */
    {
        0x00, 0x00, 0x01, 0x1e, 0x48, 0x53, 0x50, 0x49, 0x43, 0x45,
        0x4e, 0x44, 0x00, 0x00, 0x01, 0x1e, 0x00, 0x00, 0x00, 0x00
    },
    /* H265 */
    {
        0x00, 0x00, 0x01, 0x60, 0x00, 0x48, 0x53, 0x50, 0x49, 0x43,
        0x45, 0x4e, 0x44, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00
    },
};

static td_u8 g_eos_data_flag[][20] = { /* 5 is group num, 20 is byte num */
    /* H264 & MVC */
    {
        0x00, 0x00, 0x01, 0x0b, 0x48, 0x53, 0x50, 0x49, 0x43, 0x45,
        0x4e, 0x44, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
    },
    /* H265 */
    {
        0x00, 0x00, 0x01, 0x62, 0x00, 0x48, 0x53, 0x50, 0x49, 0x43,
        0x45, 0x4e, 0x44, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00
    },
};

static inline td_u32 stm_raw_inc(td_u32 x, td_u32 size)
{
    if (size == 0) {
        return 0;
    }

    return (x + 1) % size;
}

static inline td_void stm_raw_clip_zero(td_s32 *x)
{
    if (x == TD_NULL) {
        return;
    }

    (*x) = ((*x) >= 0) ? (*x) : 0;
}

td_s32 stm_raw_init(td_s32 chan_id)
{
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    raw_list->head = 0;
    raw_list->tail = 0;
    raw_list->history = 0;
    raw_list->fresh_size = 0;
    raw_list->fresh_num = 0;

    raw_list->push_num = 0;
    raw_list->push_len = 0;
    raw_list->is_cuting = 0;
    raw_list->fill_new_flag = 0;
    raw_list->last_cfg_all_flag = 1;
    raw_list->first_packet_offset = 0;
    raw_list->delete_len = 0;

    raw_list->cfg_dev_cnt = 0;
    raw_list->raw_num_cnt = 0;
    raw_list->raw_size_cnt = 0;
    raw_list->last_time_ms = 0;
    raw_list->bitrate_kbps = 1;
    raw_list->num_rate_ops = 0;

    raw_list->raw_num_all = 0;
    raw_list->raw_size_all = 0;

    raw_list->recv_try_cnt = 0;
    raw_list->recv_ok_cnt = 0;
    raw_list->recv_fail_cnt = 0;
    raw_list->rels_try_cnt = 0;
    raw_list->rels_ok_cnt = 0;

    raw_list->delete_num = 0;
    raw_list->cc_len = 0;

    return STM_OK;
}

STATIC td_void stm_raw_fill_eos_vid_std(const stm_chan *chan, td_u32 *data_len, td_u8 **data_flag)
{
    switch (chan->cfg.vid_std) {
        case VFMW_HEVC:
            *data_flag = g_eos_data_flag[1]; /* 1 is group idx */
            *data_len = 16; /* 16 is byte len */
            break;

        case VFMW_H264:
        case VFMW_MVC:
            *data_flag = g_eos_data_flag[0]; /* 0 is group idx */
            *data_len = 15; /* 15 is byte len */
            break;
        default:
            break;
    }
}

STATIC td_s32 stm_raw_fill_eos(td_s32 chan_id, stm_raw_info *raw)
{
    td_u8 *data_flag = TD_NULL;
    td_u32 data_len = 0;
    stm_chan *chan = STM_CTRL_GET_CHAN(chan_id);

    vfmw_check_ret(raw != TD_NULL, STM_ERR);

    stm_raw_fill_eos_vid_std(chan, &data_len, &data_flag);

    if (data_len != 0) {
        if (data_len > raw->length) {
            dprint(PRN_ERROR, "%s: flag len(%d) > raw len(%d)\n", __func__, data_len, raw->length);
            return STM_ERR;
        }
        vfmw_check_sec_func(memcpy_s(uint64_ptr(raw->vir_addr), raw->length, data_flag, data_len));
        raw->length = data_len;
    }

    return STM_OK;
}

STATIC td_s32 stm_raw_fill_eop(td_s32 chan_id, stm_raw_info *raw)
{
    td_u8 *data_flag = TD_NULL;
    td_u32 data_len = 0;
    stm_chan *chan = TD_NULL;

    if (raw == TD_NULL) {
        dprint(PRN_ERROR, "fill eop chan %d raw is null\n", chan_id);
        return STM_ERR;
    }

    chan = STM_CTRL_GET_CHAN(chan_id);
    switch (chan->cfg.vid_std) {
        case VFMW_H264:
            data_flag = g_eop_data_flag[0]; /* 0 is group idx */
            data_len = 15; /* 15 is byte len */
            break;
        case VFMW_HEVC:
            data_flag = g_eop_data_flag[1]; /* 1 is group idx */
            data_len = 16; /* 16 is byte len */
            break;

        default:
            dprint(PRN_ERROR, "%s: unsupport std %d!\n", __func__, chan->cfg.vid_std);
            return STM_ERR;
    }

    if (data_len != 0) {
        if (data_len > raw->length) {
            dprint(PRN_ERROR, "%s: flag len(%d) > raw len(%d)\n", __func__, data_len, raw->length);
            return STM_ERR;
        }
        vfmw_check_sec_func(memcpy_s(uint64_ptr(raw->vir_addr), raw->length, data_flag, data_len));
        raw->length = data_len;
    }

    return STM_OK;
}

/* release one raw packet to extern module */
td_s32 stm_raw_release_ext(td_s32 chan_id, stm_raw_info *raw)
{
    td_s32 ret;
    vfmw_stream_intf *raw_intf = TD_NULL;
    stm_chan *chan = TD_NULL;
    raw_list_info *raw_list = TD_NULL;

    vfmw_check_ret(raw != TD_NULL, STM_ERR);
    chan = STM_CTRL_GET_CHAN(chan_id);
    raw_intf = &(chan->raw_intf);
    vfmw_check_ret(raw_intf->release_stream != TD_NULL, STM_ERR);

    if (raw->phy_addr == chan->eop_mem.phy_addr) {
        return STM_OK;
    }

    raw_list = chan->raw_list;
    vfmw_check_ret(raw_list != TD_NULL, STM_ERR);

    raw_list->rels_try_cnt++;

    if (raw->is_stream_end_flag != 0) {
        raw->vir_addr = chan->eos_src.vir_addr;
        raw->phy_addr = chan->eos_src.phy_addr;
        raw->length = chan->eos_src.length;
    }
    /* for cpu load do not memcpy raw */
    ret = raw_intf->release_stream(raw_intf->stream_provider_inst_id, raw);
    if (ret == 0) {
        raw_list->rels_ok_cnt++;
    }

    return (ret == 0) ? STM_OK : STM_ERR;
}

STATIC td_s32 stm_raw_pre_process(stm_chan *chan, stm_raw_info *raw)
{
    vfmw_check_ret(chan != TD_NULL, STM_ERR);
    vfmw_check_ret(raw != TD_NULL, STM_ERR);
    vfmw_check_ret(os_get_time_us != TD_NULL, STM_ERR);

    if (dcheck(PRN_TIME)) {
        raw->que_time = os_get_time_us();
        raw->deq_time = os_get_time_us();
    }

    if (raw->is_stream_end_flag == 1) {
        chan->eos_src.vir_addr = raw->vir_addr;
        chan->eos_src.phy_addr = raw->phy_addr;
        chan->eos_src.length = raw->length;
        raw->vir_addr = chan->eos_mem.vir_addr;
        raw->phy_addr = chan->eos_mem.phy_addr;
        raw->length = chan->eos_mem.length;
        if (stm_raw_fill_eos(chan->id, raw) != STM_OK) {
            dprint(PRN_ERROR, "stm chan%d fill eos failed !\n", chan->id);
            return STM_ERR;
        }
    } else {
        if ((chan->cfg.fill_eop_en != 0 && raw->is_not_last_packet_flag == 0) &&
            (chan->cfg.vid_std == VFMW_H264 || chan->cfg.vid_std == VFMW_HEVC)) {
            chan->eop_flag = TD_TRUE;
        }
    }

    return STM_OK;
}

STATIC td_s32 stm_raw_check_eop(stm_chan *chan, stm_raw_info *raw)
{
    td_s32 ret;

    vfmw_check_ret(chan != TD_NULL, STM_ERR);
    vfmw_check_ret(raw != TD_NULL, STM_ERR);

    if (chan->eop_flag == TD_FALSE) {
        return STM_ERR;
    }

    chan->eop_flag = TD_FALSE;
    raw->vir_addr = chan->eop_mem.vir_addr;
    raw->phy_addr = chan->eop_mem.phy_addr;
    raw->length = chan->eop_mem.length;
    ret = stm_raw_fill_eop(chan->id, raw);
    vfmw_check_ret(ret == STM_OK, STM_ERR);

    return STM_OK;
}

STATIC td_void stm_raw_read_post(stm_chan *chan, const stm_raw_info *raw)
{
    if (raw->is_stream_end_flag == 1) {
        chan->eos = STM_EOS_RECV;
    }

    if (raw->is_not_last_packet_flag == 1) {
        chan->get_last_packet = STM_RAW_NOT_GET_LASTPACKET;
    } else {
        chan->get_last_packet = STM_RAW_GET_LASTPACKET;
    }

    if (chan->pts_state == STM_FIRST_SECOND_NOT_GET && raw->pts != VFMW_INVALID_PTS) {
        chan->pts_state = STM_FIRST_PTS_GET;
        report_first_pts(chan->id, raw->pts);
    } else if (chan->pts_state == STM_FIRST_PTS_GET && raw->pts != VFMW_INVALID_PTS) {
        chan->pts_state = STM_SECOND_PTS_GET;
        report_second_pts(chan->id, raw->pts);
    }
}

/* read one raw packet from extern module */
td_s32 stm_raw_read_ext(td_s32 chan_id, stm_raw_info *raw)
{
    td_s32 ret;
    vfmw_stream_intf *raw_intf = TD_NULL;
    stm_chan *chan = TD_NULL;
    raw_list_info *raw_list = TD_NULL;
    stm_entry *entry = TD_NULL;

    vfmw_check_ret(raw != TD_NULL, STM_ERR);
    entry = stm_ctrl_get_entry();
    chan = STM_CTRL_GET_CHAN(chan_id);
    raw_intf = &(chan->raw_intf);
    vfmw_check_ret(raw_intf->read_stream != TD_NULL, STM_ERR);

    if (stm_raw_check_eop(chan, raw) == STM_OK) {
        return STM_OK;
    }

    raw_list = chan->raw_list;
    vfmw_check_ret(raw_list != TD_NULL, STM_ERR);

    raw_list->recv_try_cnt++;
    if (entry->stm_no_raw == 1) {
        goto failed;
    }

    ret = raw_intf->read_stream(raw_intf->stream_provider_inst_id, raw);
    if (ret != 0) {
        goto failed;
    }

    if (entry->bypass_chan == chan_id) {
        goto err_rel;
    }

    if (stm_raw_pre_process(chan, raw) != STM_OK) {
        goto err_rel;
    }

    raw_list->recv_ok_cnt++;
    raw_list->recv_fail_cnt = 0;
    stm_raw_read_post(chan, raw);

    return STM_OK;

err_rel:
    if (stm_raw_release_ext(chan_id, raw) != STM_OK) {
        dprint(PRN_ERROR, "%s rels raw failed!\n", __func__);
    }

failed:
    raw_list->recv_fail_cnt++;

    return STM_ERR;
}

/* calc left raw stream size of raw list */
td_s32 stm_raw_get_fresh_size(td_s32 chan_id)
{
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;

    return (raw_list->fresh_size - raw_list->first_packet_offset);
}

/* calc left raw stream num of raw list */
td_s32 stm_raw_get_fresh_num(td_s32 chan_id)
{
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;

    return (raw_list->tail + MAX_RAW_NUM - raw_list->head) % MAX_RAW_NUM;
}

td_s32 stm_raw_get_first_offset(td_s32 chan_id)
{
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    if (raw_list->head == raw_list->tail) {
        return 0;
    }

    return raw_list->first_packet_offset;
}

STATIC td_s32 stm_raw_set_first_offset(td_s32 chan_id, td_s32 offset)
{
    td_s32 raw_id;
    td_s32 target_offset;
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    if (raw_list->head == raw_list->tail) {
        return STM_ERR;
    }

    raw_id = raw_list->head;
    target_offset = raw_list->first_packet_offset + offset;
    if (target_offset >= (td_s32)raw_list->raw[raw_id].length) {
        return STM_ERR;
    }

    raw_list->first_packet_offset = target_offset;

    return STM_OK;
}

STATIC td_void stm_raw_get_num_offset(td_s32 chan_id, td_s32 raw_len, td_s32 *raw_num, td_s32 *offset, td_u32 *raw_id)
{
    td_s32 cnt;
    td_u32 id;
    td_s32 total_len;
    td_s32 total_num;
    raw_list_info *raw_list = TD_NULL;

    vfmw_check(raw_num != TD_NULL);
    vfmw_check(offset != TD_NULL);
    vfmw_check(raw_id != TD_NULL);
    vfmw_check(chan_id >= 0 && chan_id < STM_CHAN_NUM);

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    id = (td_u32)raw_list->head;
    total_len = (td_s32)raw_list->raw[id].length - raw_list->first_packet_offset;

    total_num = stm_raw_get_fresh_num(chan_id);
    for (cnt = 0; cnt < total_num; cnt++) {
        if (total_len > raw_len) {
            break;
        }

        id = stm_raw_inc(id, MAX_RAW_NUM);
        total_len += (td_s32)raw_list->raw[id].length;
        if (raw_list->raw[id].length == 0 && STM_CTRL_GET_CHAN(chan_id)->cfg.vid_std == VFMW_VP6) {
            cnt = 1;
            break;
        }
    }

    if (cnt == 0) {
        *offset = raw_len;
    } else {
        if (total_len > raw_len) {
            total_len -= (td_s32)raw_list->raw[id].length;
        }
        *offset = raw_len - total_len;
    }

    *raw_num = cnt;
    *raw_id = id;
}

td_void stm_raw_get_pts(td_s32 chan_id, td_s32 len, td_s64 *last_pts, td_void *seg)
{
    td_s32 num = 0;
    td_s32 offset = 0;
    td_u32 index = 0;
    raw_list_info *raw_list = TD_NULL;
    seg_list *list = TD_NULL;
    stm_seg_info *seg_packet = TD_NULL;
    vfmw_stream_data *raw_packet = TD_NULL;
    stm_chan *chan = TD_NULL;

    (td_void)list; /* avoid to build err when printing is not supported. */
    vfmw_check(seg != TD_NULL);
    vfmw_check(last_pts != TD_NULL);
    stm_raw_get_num_offset(chan_id, len, &num, &offset, &index);

    chan = STM_CTRL_GET_CHAN(chan_id);

    raw_list = chan->raw_list;
    raw_packet = &(raw_list->raw[index]);

    list = chan->seg_list_info;

    seg_packet = (stm_seg_info *)seg;
    seg_packet->pts = raw_packet->pts;
    seg_packet->timestamp = raw_packet->time_stamp;
    seg_packet->user_tag = raw_packet->user_tag;
    seg_packet->raw_idx = index;
    seg_packet->disp_time = raw_packet->disp_time;
    seg_packet->disp_enable_flag = raw_packet->disp_enable_flag;
    seg_packet->disp_frame_distance = raw_packet->disp_frame_distance;
    seg_packet->distance_beforce_first_frame = raw_packet->distance_before_first_frame;
    seg_packet->gop_num = raw_packet->gop_num;
    seg_packet->is_stream_end = raw_packet->is_stream_end_flag;
    seg_packet->dv_dual_layer = raw_packet->dv_dual_layer;

    if (dcheck(PRN_TIME)) {
        seg_packet->que_raw_time = raw_packet->que_time;
        seg_packet->deq_raw_time = raw_packet->deq_time;
    }

    if (*last_pts == seg_packet->pts && chan->cfg.is_android_test == 0) {
        seg_packet->pts = VFMW_INVALID_PTS;
    } else {
        *last_pts = seg_packet->pts;
    }

    dprint(PRN_PTS, "cut seg %d pts %lld.\n", list->seg_num_cnt, seg_packet->pts);

    return;
}

/* delete those used raws packet from rawlist */
td_s32 stm_raw_delete(td_s32 chan_id)
{
    td_u32 cnt;
    td_u32 index;
    td_u32 del_num;
    td_ulong lock_flag;
    stm_chan *chan = TD_NULL;
    raw_list_info *raw_list = TD_NULL;

    chan = STM_CTRL_GET_CHAN(chan_id);
    raw_list = chan->raw_list;

    if (chan->cfg.seg_bypass == 1) {
        stm_chan_lock(chan_id, &lock_flag);
        del_num = raw_list->delete_num;
        raw_list->delete_num = 0;
        del_num = (raw_list->history == raw_list->tail) ? 0 : del_num;
        stm_chan_unlock(chan_id, &lock_flag);
    } else {
        if (raw_list->delete_len > 0 ||
            (raw_list->delete_len == 0 &&
             STM_CTRL_GET_CHAN(chan_id)->cfg.vid_std == VFMW_VP6)) {
            stm_raw_release_len(chan_id, (td_s32)raw_list->delete_len);
            raw_list->delete_len = 0;
        }
        del_num = (td_u32)((raw_list->head + MAX_RAW_NUM - raw_list->history) % MAX_RAW_NUM);

        if (chan->rls_raw_wait == 1) {
            cnt = (chan->rls_raw_idx + MAX_RAW_NUM - (td_u32)raw_list->history) % MAX_RAW_NUM;
            del_num = vfmw_min(cnt, del_num);
        }
    }

    index = (td_u32)raw_list->history;
    for (cnt = 0; cnt < del_num && index < MAX_RAW_NUM; cnt++) {
        if (chan->cfg.seg_bypass == 1) {
            raw_list->fresh_num--;
            raw_list->fresh_size -= (td_s32)raw_list->raw[index].length;
        }
        if (stm_raw_release_ext(chan_id, &(raw_list->raw[index])) != STM_OK) {
            dprint(PRN_ERROR, "%s rels raw failed!\n", __func__);
        }
        index = stm_raw_inc(index, MAX_RAW_NUM);
    }

    raw_list->history = (td_s32)(((td_u32)raw_list->history + del_num) % MAX_RAW_NUM);

    return STM_OK;
}

/* delete raw packet head */
td_s32 stm_raw_delete_head(td_s32 chan_id)
{
    td_s32 raw_num;
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    raw_num = (raw_list->head <= raw_list->tail) ? raw_list->tail - raw_list->head : raw_list->tail + MAX_RAW_NUM -
              raw_list->head;

    if (raw_num > 0) {
        raw_list->head = (td_s32)stm_raw_inc((td_u32)raw_list->head, MAX_RAW_NUM);
        raw_list->first_packet_offset = 0;
    }

    return STM_OK;
}

/* delete last push raw packet */
td_void stm_raw_del_cfg_raw(td_s32 chan_id)
{
    td_u32 cnt;
    td_u32 index;
    td_u32 del_num;
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    if (raw_list->push_len <= 0) {
        return;
    }

    if (stm_raw_release_len(chan_id, raw_list->push_len) != STM_OK) {
        return;
    }

    raw_list->delete_len = 0;
    del_num = (td_u32)((raw_list->head + MAX_RAW_NUM - raw_list->history) % MAX_RAW_NUM);

    index = (td_u32)raw_list->history;
    for (cnt = 0; cnt < del_num && index < MAX_RAW_NUM; cnt++) {
        if (stm_raw_release_ext(chan_id, &(raw_list->raw[index])) != STM_OK) {
            dprint(PRN_ERROR, "%s rels raw failed!\n", __func__);
        }
        index = stm_raw_inc(index, MAX_RAW_NUM);
    }

    raw_list->history = (td_s32)(((td_u32)raw_list->history + del_num) % MAX_RAW_NUM);

    return;
}

td_s32 stm_raw_deleta_all(td_s32 chan_id)
{
    td_u32 cnt;
    td_u32 index;
    td_u32 del_num;
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;

    del_num = (td_u32)((raw_list->tail + MAX_RAW_NUM - raw_list->history) % MAX_RAW_NUM);

    index = (td_u32)raw_list->history;
    for (cnt = 0; cnt < del_num && index < MAX_RAW_NUM; cnt++) {
        if (stm_raw_release_ext(chan_id, &(raw_list->raw[index])) != STM_OK) {
            dprint(PRN_ERROR, "%s rels raw failed!\n", __func__);
        }
        index = stm_raw_inc(index, MAX_RAW_NUM);
    }

    raw_list->history = (td_s32)(((td_u32)raw_list->history + del_num) % MAX_RAW_NUM);
    raw_list->head = raw_list->history;

    if (STM_CTRL_GET_CHAN(chan_id)->cfg.seg_bypass == 1) {
        raw_list->delete_num = 0;
    }

    return STM_OK;
}

STATIC td_s32 stm_raw_release_num(td_s32 chan_id, td_s32 del_num)
{
    td_s32 index;
    td_s32 cnt;
    td_s32 raw_num;
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    raw_num = stm_raw_get_fresh_num(chan_id);
    if (del_num < 0 ||
        del_num > raw_num ||
        raw_num <= 0 ||
        raw_num >= MAX_RAW_NUM) {
        dprint(PRN_ERROR, "del_num(%d) or raw_num(%d) error\n", del_num, raw_num);

        return STM_ERR;
    }

    index = raw_list->head;
    if (index < 0 || index >= MAX_RAW_NUM) {
        dprint(PRN_ERROR, "%s raw list head(%d) error\n", __func__, index);

        return STM_ERR;
    }

    for (cnt = 0; cnt < del_num; cnt++) {
        raw_list->fresh_num--;
        raw_list->fresh_size -= (td_s32)raw_list->raw[index].length;
        index = (td_s32)stm_raw_inc((td_u32)index, MAX_RAW_NUM);
    }

    stm_raw_clip_zero(&raw_list->fresh_num);
    stm_raw_clip_zero(&raw_list->fresh_size);
    raw_list->head = (raw_list->head + del_num) % MAX_RAW_NUM;

    if (del_num > 0) {
        raw_list->first_packet_offset = 0;
    }

    return STM_OK;
}

/* delete raw packet by length */
td_s32 stm_raw_release_len(td_s32 chan_id, td_s32 del_len)
{
    td_s32 del_num = 0;
    td_u32 raw_id;
    td_s32 raw_offset = 0;
    raw_list_info *raw_list = TD_NULL;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    if (del_len > raw_list->fresh_size - raw_list->first_packet_offset) {
        return STM_ERR;
    }

    stm_raw_get_num_offset(chan_id, del_len, &del_num, &raw_offset, &raw_id);

    if (del_num != 0) {
        stm_raw_release_num(chan_id, del_num);
    }

    return stm_raw_set_first_offset(chan_id, raw_offset);
}

/* show cur raw info form rawlist, but not move pointer */
STATIC stm_raw_info *stm_raw_show_cur(raw_list_info *raw_list)
{
    if (raw_list == TD_NULL ||
        raw_list->show_id == raw_list->tail) {
        return TD_NULL;
    }

    /* for cpu load do not memcpy raw */
    return raw_list->raw + raw_list->show_id;
}

td_s32 stm_raw_show_head(td_s32 chan_id, stm_raw_info *raw)
{
    td_s32 ret = STM_ERR;
    stm_raw_info *tmp = TD_NULL;
    raw_list_info *raw_list = TD_NULL;

    vfmw_check_ret(raw != TD_NULL, STM_ERR);
    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    raw_list->show_id = raw_list->head;

    tmp = stm_raw_show_cur(raw_list);
    if (tmp != TD_NULL) {
        raw->phy_addr = tmp->phy_addr;
        raw->vir_addr = tmp->vir_addr;
        raw->length = tmp->length;
        ret = STM_OK;
    }

    return ret;
}

stm_raw_info *stm_raw_show_next(raw_list_info *raw_list)
{
    vfmw_check_ret(raw_list != TD_NULL, TD_NULL);

    if (raw_list->show_id == raw_list->tail) {
        return TD_NULL;
    }

    raw_list->show_id = (td_s32)stm_raw_inc((td_u32)raw_list->show_id, MAX_RAW_NUM);

    return stm_raw_show_cur(raw_list);
}

/* check if rawlist is full */
td_s32 stm_raw_is_full(const raw_list_info *raw_list)
{
    td_s32 tail_add1;

    vfmw_check_ret(raw_list != TD_NULL, STM_ERR);
    tail_add1 = (td_s32)stm_raw_inc((td_u32)raw_list->tail, MAX_RAW_NUM);

    return (tail_add1 == raw_list->history) ? STM_OK : STM_ERR;
}

/* insert one raw packet to rawlist */
td_s32 stm_raw_insert(td_s32 chan_id, const stm_raw_info *raw)
{
    td_u32 raw_id;
    raw_list_info *raw_list = TD_NULL;

    vfmw_check_ret(raw != TD_NULL, STM_ERR);
    vfmw_check_ret((raw->length <= MAX_RAW_LEN), STM_ERR);

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;
    if (stm_raw_is_full(raw_list) == STM_OK) {
        return STM_ERR;
    }

    if (raw->pts != VFMW_INVALID_PTS) {
        dprint(PRN_PTS, "raw %d pts %lld len %d\n", raw_list->raw_num_all, raw->pts, raw->length);
    }

    raw_id = (td_u32)raw_list->tail;
    vfmw_check_sec_func(memcpy_s(&(raw_list->raw[raw_id]), sizeof(raw_list->raw[raw_id]), raw, sizeof(stm_raw_info)));
    raw_list->tail = (td_s32)stm_raw_inc((td_u32)raw_list->tail, MAX_RAW_NUM);
    raw_list->fill_new_flag = 1;

    raw_list->fresh_num++;
    raw_list->fresh_size += (td_s32)raw->length;

    raw_list->raw_num_all++;
    raw_list->raw_size_all += raw->length;

#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_cache(uint64_ptr(raw->vir_addr), raw->phy_addr, raw->length);
#endif
    return STM_OK;
}

td_s32 stm_raw_get_raw_num_in_frm(td_s32 chan_id, td_s32 *raw_num, td_s32 *raw_len)
{
    td_s32 ret = STM_ERR;
    stm_chan *chan = TD_NULL;
    raw_list_info *raw_list = TD_NULL;
    td_s32 index;
    td_s32 cnt;
    td_s32 num = 0;
    td_s32 len = 0;

    vfmw_check_ret(raw_num != TD_NULL, STM_ERR);
    vfmw_check_ret(raw_len != TD_NULL, STM_ERR);

    chan = STM_CTRL_GET_CHAN(chan_id);
    raw_list = chan->raw_list;

    for (cnt = 0; cnt < MAX_RAW_NUM; cnt++) {
        index = (raw_list->head + cnt) % MAX_RAW_NUM;
        if (index == raw_list->tail) {
            break;
        }

        num += 1;
        len += (td_s32)raw_list->raw[index].length;
        if (raw_list->raw[index].is_not_last_packet_flag == 0) {
            ret = STM_OK;
            break;
        }
    }

    *raw_num = num;
    *raw_len = len;

    return ret;
}

td_u32 stm_raw_get_bit_rate(td_s32 chan_id)
{
    raw_list_info *raw_list;

    raw_list = STM_CTRL_GET_CHAN(chan_id)->raw_list;

    return raw_list->bitrate_kbps * 1024; /* 1024 is the kilo unit */
}
