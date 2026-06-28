/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stm_chan.h"
#include "stm_base.h"
#include "stm_ctrl.h"
#include "stm_dev.h"
#include "fsp.h"
#include "stm_log.h"
#include "vfmw_define.h"
#include "vdec_chr.h"
#include "pdt_com.h"

typedef struct {
    vfmw_mem_desc seg_mem;
    vfmw_mem_desc eos_mem;
    seg_list *seg_list_info;
    raw_list_info *raw_list;
} stm_mem_info;

td_s32 stm_chan_get_num(stm_chan_type_num *num)
{
    td_s32 id;
    stm_chan *chan = TD_NULL;

    vfmw_check_ret(num != TD_NULL, STM_ERR);

    chan = STM_CTRL_GET_CHAN(0);
    for (id = 0; id < STM_CHAN_NUM; id++) {
        if (chan[id].handle != TD_NULL) {
            if (chan[id].is_sec == 1) {
                num->sec++;
            } else {
                num->no_sec++;
            }
        }
    }

    return STM_OK;
}

STATIC td_s32 stm_chan_alloc_buf(td_s32 chan_id)
{
    td_s32 ret;
    mem_record mem = { 0 };
    td_u32 buf_size;
    stm_chan *chan = TD_NULL;

    ret = STM_ERR;

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle == TD_NULL) {
        dprint(PRN_ERROR, "%s error handle is null \n", __func__);
        goto out;
    }

    if (chan->cfg.seg_bypass == 1 || chan->seg_mem.length != 0) {
        ret = STM_OK;
        goto out;
    }

    if (chan->seg_buf_size <= 0) {
        chan->seg_buf_size = pdt_calc_seg_buf_size(chan_id);
    }
    if (chan->seg_buf_size < (td_s32)stm_seg_get_min_buf_size()) {
        dprint(PRN_ERROR, "seg_buf_size %d out of range!\n", chan->seg_buf_size);
        goto out;
    }

    buf_size = (td_u32)chan->seg_buf_size;
    mem.length = vfmw_align_up(buf_size, 16); /* 16 is aligb len */
    mem.is_cached = 0;
    mem.type = SEG_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    mem.mem_name = "vfmw_seg_buf";
    vfmw_check_goto(os_malloc != TD_NULL, out);
    if (os_malloc(&mem) != OSAL_OK) {
        dprint(PRN_ERROR, "%s alloc segBuf failed!\n", __func__);
        goto out;
    }

    chan->seg_mem.dma_buf = mem.dma_buf;
    chan->seg_mem.length = mem.length;
    chan->seg_mem.vir_addr = ptr_uint64(mem.vir_addr);
    chan->seg_mem.phy_addr = mem.phy_addr;
    stm_seg_config_buf(chan_id, &(chan->seg_mem));
    stm_ctrl_get_entry()->hal_size += buf_size;
    ret = STM_OK;

out:

    return ret;
}

STATIC td_void stm_chan_clear_seg_buf(stm_entry *entry, stm_chan *chan, td_bool need_free)
{
    mem_record mem = { 0 };

    if (need_free == TD_TRUE) {
        mem.dma_buf = chan->seg_mem.dma_buf;
        mem.length = chan->seg_mem.length;
        mem.phy_addr = chan->seg_mem.phy_addr;
        mem.vir_addr = uint64_ptr(chan->seg_mem.vir_addr);
        mem.is_cached = 0;
        os_free(&mem);
    }
    entry->hal_size -= (td_u32)chan->seg_mem.length;
    vfmw_check_sec_func(memset_s(&chan->seg_mem, sizeof(chan->seg_mem), 0, sizeof(vfmw_mem_desc)));

    return;
}

STATIC td_void stm_chan_clear_raw_ctx(stm_entry *entry, stm_chan *chan, td_bool need_free)
{
    mem_record mem = { 0 };

    if (need_free == TD_TRUE) {
        mem.vir_addr = (td_u8 *)(chan->raw_list);
        mem.is_cached = 0;
        os_free(&mem);
    }
    chan->raw_list = TD_NULL;
    entry->ctx_size -= (td_u32)sizeof(raw_list_info);

    return;
}

STATIC td_void stm_chan_clear_seg_ctx(stm_entry *entry, stm_chan *chan, td_bool need_free)
{
    mem_record mem = { 0 };

    if (need_free == TD_TRUE) {
        mem.vir_addr = (td_u8 *)(chan->seg_list_info);
        mem.is_cached = 0;
        os_free(&mem);
    }
    chan->seg_list_info = TD_NULL;
    entry->ctx_size -= (td_u32)sizeof(seg_list);

    return;
}

STATIC td_void stm_chan_clear_eos_mem(stm_entry *entry, stm_chan *chan, td_bool need_free)
{
    mem_record mem = { 0 };

    if (need_free == TD_TRUE) {
        mem.dma_buf = chan->eos_mem.dma_buf;
        mem.length = chan->eos_mem.length;
        mem.phy_addr = chan->eos_mem.phy_addr;
        mem.vir_addr = uint64_ptr(chan->eos_mem.vir_addr);
        mem.is_cached = 0;
        os_free(&mem);
    }
    chan->eos_mem.vir_addr = 0;
    entry->ctx_size -= (td_u32)chan->eos_mem.length;
}

STATIC td_void stm_chan_init_chan(stm_chan *chan, td_s32 chan_id, const stm_create_info *param)
{
    vfmw_check(os_get_unid != TD_NULL);

    chan->unid = (td_u32)os_get_unid();
    chan->id = chan_id;
    chan->handle = chan;
    chan->state = STM_CHAN_STATE_STOP;
    chan->eos = STM_EOS_WAIT;
    chan->eop_flag = TD_FALSE;
    chan->is_sec = param->is_sec;
    chan->seg_buf_size = param->seg_buf_size;
    chan->pts_state = STM_FIRST_SECOND_NOT_GET;
    chan->block_time = 0;
    chan->dev_id = 0;
    chan->threshold = 0;
    chan->tick = 0;
    chan->std_err = TD_FALSE;

    vfmw_check_sec_func(memset_s(&chan->chk_param, sizeof(chan->chk_param), 0, sizeof(stm_chk_param)));

    /* chan cfg */
    vfmw_check_sec_func(memset_s(&chan->cfg, sizeof(chan->cfg), 0, sizeof(stm_cfg_info)));

    /* seg mem */
    vfmw_check_sec_func(memset_s(&chan->seg_mem, sizeof(chan->seg_mem), 0, sizeof(vfmw_mem_desc)));
    vfmw_check_sec_func(memset_s(&chan->eos_mem, sizeof(chan->eos_mem), 0, sizeof(vfmw_mem_desc)));
    chan->eos_mem.length = STM_EOS_SIZE;

    return;
}

STATIC td_void stm_chan_clear_chan(stm_chan *chan)
{
    vfmw_stream_intf *intf = &chan->raw_intf;

    vfmw_check_sec_func(memset_s(intf, sizeof(chan->raw_intf), 0, sizeof(vfmw_stream_intf)));
    vfmw_check_sec_func(memset_s(&chan->cfg, sizeof(chan->cfg), 0, sizeof(stm_cfg_info)));

    chan->id = 0;
    chan->unid = 0;
    chan->handle = TD_NULL;
    chan->state = STM_CHAN_STATE_NULL;
    chan->es_valid = 0;

    return;
}

STATIC td_s32 stm_chan_alloc_raw_list(stm_entry *entry, stm_chan *chan)
{
    mem_record mem = { 0 };

    /* "RAW_List" */
    mem.mem_name = "raw_list_info";
    mem.length = vfmw_align_up((td_u32)sizeof(raw_list_info), VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan->id;
    vfmw_check_ret(os_malloc != TD_NULL, STM_ERR);
    os_malloc(&mem);
    chan->raw_list = (raw_list_info *)(mem.vir_addr);
    if (chan->raw_list == TD_NULL) {
        dprint(PRN_ERROR, "stm alloc raw ctx failed !\n");
        return STM_ERR;
    }

    stm_raw_init(chan->id);
    entry->ctx_size += (td_u32)sizeof(raw_list_info);

    return STM_OK;
}

STATIC td_s32 stm_chan_alloc_seg_ctx(stm_entry *entry, stm_chan *chan)
{
    mem_record mem = { 0 };

    /* seg context */
    mem.mem_name = "seg_list";
    mem.length = vfmw_align_up((td_u32)sizeof(seg_list), VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan->id;
    vfmw_check_ret(os_malloc != TD_NULL, STM_ERR);
    os_malloc(&mem);
    chan->seg_list_info = (seg_list *)(mem.vir_addr);
    if (chan->seg_list_info == TD_NULL) {
        dprint(PRN_ERROR, "stm alloc seg ctx failed !\n");
        return STM_ERR;
    }

    stm_seg_init(chan->id, 1);
    entry->ctx_size += (td_u32)sizeof(seg_list);

    return STM_OK;
}

STATIC td_s32 stm_chan_alloc_eos_mem(stm_entry *entry, stm_chan *chan)
{
    td_u32 half_len;
    mem_record mem = { 0 };

    /* eos */
    mem.length = chan->eos_mem.length;
    mem.is_cached = 0;
    mem.type = NO_CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan->id;
    mem.mem_name = "vfmw_eos_buf";
    vfmw_check_ret(os_malloc != TD_NULL, STM_ERR);
    if (os_malloc(&mem) != OSAL_OK) {
        dprint(PRN_ERROR, "%s alloc eos_buf failed!\n", __func__);
        return STM_ERR;
    }
    chan->eos_mem.dma_buf = mem.dma_buf;
    chan->eos_mem.phy_addr = mem.phy_addr;
    chan->eos_mem.vir_addr = ptr_uint64(mem.vir_addr);

    half_len = mem.length / 2; /* 2: eos + eop */
    chan->eop_mem.length = half_len;
    chan->eop_mem.phy_addr = mem.phy_addr + half_len;
    chan->eop_mem.vir_addr = ptr_uint64(mem.vir_addr) + half_len;

    entry->ctx_size += chan->eos_mem.length;

    return STM_OK;
}

td_s32 stm_chan_create(td_s32 chan_id, const td_void *args)
{
    stm_chan *chan = TD_NULL;
    stm_entry *entry = TD_NULL;
    const stm_create_info *param = (stm_create_info *)args;

    vfmw_check_ret(args != TD_NULL, STM_ERR);

    entry = stm_ctrl_get_entry();
    chan = STM_CTRL_GET_CHAN(chan_id);

    stm_chan_init_chan(chan, chan_id, param);

    if (stm_chan_alloc_raw_list(entry, chan) != STM_OK) {
        goto out;
    }

    if (stm_chan_alloc_seg_ctx(entry, chan) != STM_OK) {
        goto out;
    }

    if (stm_chan_alloc_eos_mem(entry, chan) != STM_OK) {
        goto out;
    }

    chan->es_valid = 1;

    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_STM_OK);

    return STM_OK;

out:

    /* Free RawCtx */
    if (chan->raw_list != TD_NULL) {
        stm_chan_clear_raw_ctx(entry, chan, TD_TRUE);
    }

    /* Free SegCtx */
    if (chan->seg_list_info != TD_NULL) {
        stm_chan_clear_seg_ctx(entry, chan, TD_TRUE);
    }

    /* Free Eos */
    if (chan->eos_mem.vir_addr != 0) {
        stm_chan_clear_eos_mem(entry, chan, TD_TRUE);
    }

    stm_chan_clear_chan(chan);

    return STM_ERR;
}

STATIC td_void stm_chan_free_mem(const stm_mem_info *mem_info)
{
    mem_record mem = { 0 };

    vfmw_alert(mem_info != TD_NULL);

    if (mem_info->seg_mem.length != 0) {
        mem.length = mem_info->seg_mem.length;
        mem.phy_addr = mem_info->seg_mem.phy_addr;
        mem.vir_addr = uint64_ptr(mem_info->seg_mem.vir_addr);
        mem.is_cached = 0;
        os_free(&mem);
        vfmw_check_sec_func(memset_s(&mem, sizeof(mem), 0, sizeof(mem_record)));
    }

    if (mem_info->raw_list != TD_NULL) {
        mem.vir_addr = (td_u8 *)(mem_info->raw_list);
        mem.is_cached = 0;
        os_free(&mem);
        vfmw_check_sec_func(memset_s(&mem, sizeof(mem), 0, sizeof(mem_record)));
    }

    if (mem_info->seg_list_info != TD_NULL) {
        mem.vir_addr = (td_u8 *)(mem_info->seg_list_info);
        mem.is_cached = 0;
        os_free(&mem);
        vfmw_check_sec_func(memset_s(&mem, sizeof(mem), 0, sizeof(mem_record)));
    }

    if (mem_info->eos_mem.vir_addr != 0) {
        mem.length = mem_info->eos_mem.length;
        mem.phy_addr = mem_info->eos_mem.phy_addr;
        mem.vir_addr = uint64_ptr(mem_info->eos_mem.vir_addr);
        mem.is_cached = 0;
        os_free(&mem);
    }
}

td_s32 stm_chan_destroy(td_s32 chan_id)
{
    stm_chan *chan = TD_NULL;
    stm_entry *entry = TD_NULL;
    stm_mem_info mem;
    td_ulong lock_flag;

    vfmw_check_sec_func(memset_s(&mem, sizeof(mem), 0, sizeof(stm_mem_info)));

    entry = stm_ctrl_get_entry();
    stm_chan_lock(chan_id, &lock_flag);
    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle == TD_NULL) {
        goto out;
    }

    /* Free SegBuf */
    if (chan->seg_mem.length != 0) {
        vfmw_check_sec_func(memcpy_s(&mem.seg_mem, sizeof(mem.seg_mem), &chan->seg_mem, sizeof(chan->seg_mem)));
        stm_chan_clear_seg_buf(entry, chan, TD_FALSE);
    }

    /* Free RawCtx */
    if (chan->raw_list != TD_NULL) {
        mem.raw_list = chan->raw_list;
        stm_chan_clear_raw_ctx(entry, chan, TD_FALSE);
    }

    /* Free SegCtx */
    if (chan->seg_list_info != TD_NULL) {
        mem.seg_list_info = chan->seg_list_info;
        stm_chan_clear_seg_ctx(entry, chan, TD_FALSE);
    }

    /* Free Eos */
    if (chan->eos_mem.vir_addr != 0) {
        vfmw_check_sec_func(memcpy_s(&mem.eos_mem, sizeof(mem.eos_mem), &chan->eos_mem, sizeof(chan->eos_mem)));
        stm_chan_clear_eos_mem(entry, chan, TD_FALSE);
    }

    stm_chan_clear_chan(chan);

    stm_chan_unlock(chan_id, &lock_flag);

    stm_chan_free_mem(&mem);
    vdec_record_chr_event(chan_id, MT_VDEC_DESTROY, MT_VDEC_STORE_CLOSE, MT_VDEC_VFMW_STM_DESTROY_OK);

    return STM_OK;
out:
    stm_chan_unlock(chan_id, &lock_flag);

    return STM_ERR;
}

td_s32 stm_chan_config(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;
    stm_chan *chan = TD_NULL;
    const stm_cfg_info *cfg = TD_NULL;

    vfmw_check_ret(args != TD_NULL, STM_ERR);

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle == TD_NULL) {
        ret = STM_ERR;
        goto out;
    }

    cfg = (stm_cfg_info *)args;

    vfmw_check_sec_func(memcpy_s(&chan->cfg, sizeof(chan->cfg), cfg, sizeof(stm_cfg_info)));

    chan->cfg.scd_bypass = stm_dev_is_scd_bypass(chan_id);
    chan->cfg.seg_bypass = stm_dev_is_scd_bypass(chan_id);

#ifdef VT_SUPPORT
    chan->rls_raw_wait = os_get_rand() & 1;
#endif

    ret = STM_OK;

out:

    return ret;
}

td_s32 stm_chan_cfg_ext_intf(td_s32 chan_id, const td_void *args)
{
    stm_chan *chan = TD_NULL;
    const vfmw_stream_intf *ext_intf = TD_NULL;

    vfmw_check_ret(args != TD_NULL, STM_ERR);

    ext_intf = (vfmw_stream_intf *)args;

    vfmw_check_ret(ext_intf->read_stream != TD_NULL, STM_ERR);
    vfmw_check_ret(ext_intf->release_stream != TD_NULL, STM_ERR);

    chan = STM_CTRL_GET_CHAN(chan_id);
    chan->raw_intf.stream_provider_inst_id = ext_intf->stream_provider_inst_id;
    chan->raw_intf.read_stream = ext_intf->read_stream;
    chan->raw_intf.release_stream = ext_intf->release_stream;

    return STM_OK;
}

td_s32 stm_chan_start(td_s32 chan_id)
{
    stm_chan *chan = TD_NULL;

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle != TD_NULL && chan->state != STM_CHAN_STATE_NULL) {
        chan->state = STM_CHAN_STATE_RUN;
        chan->thres_state = STM_RAW_NO_NEED_THRESHOLD;
        chan->get_last_packet = STM_RAW_NOT_GET_LASTPACKET;

        return STM_OK;
    }

    return STM_ERR;
}

td_s32 stm_chan_stop(td_s32 chan_id)
{
    stm_chan *chan = TD_NULL;
    td_s32 wait = 1000;

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle != TD_NULL && chan->state != STM_CHAN_STATE_NULL) {
        chan->state = STM_CHAN_STATE_STOP;

        if (stm_dev_cancel(chan->dev_id, chan_id) == STM_OK) {
            return STM_OK;
        }
        while (((wait--) != 0) && (chan->raw_list->is_cuting != 0)) {
            os_msleep(1);
        }

        if (chan->raw_list->is_cuting != 0) {
            dprint(PRN_ERROR, "stm stop chan(%d) fail!\n", chan_id);
            return STM_ERR;
        }
        return STM_OK;
    }

    return STM_ERR;
}

td_s32 stm_chan_reset(td_s32 chan_id)
{
    vfmw_mem_desc *mem = TD_NULL;
    stm_chan *chan = TD_NULL;

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan->handle == TD_NULL) {
        goto out;
    }

    if (chan->state == STM_CHAN_STATE_RUN) {
        chan->state = STM_CHAN_STATE_STOP;
    }
    chan->eos = STM_EOS_WAIT;
    chan->eop_flag = TD_FALSE;
    chan->pts_state = STM_FIRST_SECOND_NOT_GET;

    stm_raw_deleta_all(chan_id);

    stm_raw_init(chan_id);

    stm_seg_init(chan_id, 0);

    mem = &chan->seg_mem;
    stm_seg_config_buf(chan_id, mem);

    return STM_OK;

out:

    return STM_ERR;
}

STATIC td_s32 stm_chan_read_is_full(const stm_chan *chan)
{
    if (stm_raw_is_full(chan->raw_list) == STM_OK) {
        return STM_ERR;
    }

    if (chan->cfg.scd_bypass == 0 &&
        chan->raw_list->cfg_dev_cnt == 0 &&
        (chan->raw_list->fresh_num > RAW_START_NUM ||
         chan->raw_list->fresh_size > RAW_START_SIZE)) {
        return STM_ERR;
    }

    return STM_OK;
}

STATIC td_s32 stm_chan_need_read(td_s32 chan_id)
{
    stm_chan *chan = TD_NULL;

    chan = STM_CTRL_GET_CHAN(chan_id);
    vfmw_check_ret_no_print(chan != TD_NULL, STM_ERR);
    vfmw_check_ret_no_print(chan->handle != TD_NULL, STM_ERR);

    if (chan->get_last_packet != STM_RAW_GET_LASTPACKET) {
        goto STATE_DECIDE;
    }

STATE_DECIDE:

    if (stm_chan_read_is_full(chan) == STM_ERR) {
        return STM_ERR;
    }

    return STM_OK;
}

STATIC td_void stm_chan_raw_count(td_s32 chan_id, const stm_raw_info *raw)
{
    td_u32 cur_time_ms;
    td_u32 time_period;
    stm_chan *chan = TD_NULL;
    raw_list_info *raw_list = TD_NULL;

    chan = STM_CTRL_GET_CHAN(chan_id);
    if (chan == TD_NULL || chan->state != STM_CHAN_STATE_RUN) {
        return;
    }
    vfmw_check(os_get_time_ms != TD_NULL);

    raw_list = chan->raw_list;
    raw_list->raw_num_cnt++;
    raw_list->raw_size_cnt += raw->length;

    cur_time_ms = os_get_time_ms();
    if (raw_list->last_time_ms == 0) {
        raw_list->last_time_ms = cur_time_ms;
    } else {
        time_period = cur_time_ms - raw_list->last_time_ms;
        if (time_period > 2000) { /* 2000 is time period */
            raw_list->num_rate_ops = raw_list->raw_num_cnt * 1000 / time_period; /* 1000 is time unit */
            raw_list->bitrate_kbps = raw_list->raw_size_cnt * 8 / time_period * 1000 / 1024; /* 8,1000,1024 is unit */
            if (raw_list->bitrate_kbps == 0 && raw_list->raw_size_cnt != 0) {
                raw_list->bitrate_kbps = 1;
            }
            raw_list->raw_num_cnt = 0;
            raw_list->raw_size_cnt = 0;
            raw_list->last_time_ms = cur_time_ms;
        }
    }
}

STATIC td_void stm_chan_fill_raw(td_s32 chan_id)
{
    stm_chan *chan = TD_NULL;
    stm_raw_info *raw_packet = TD_NULL;

    chan = STM_CTRL_GET_CHAN(chan_id);
    raw_packet = &(chan->raw_list->raw_tmp);
    raw_packet->pts = VFMW_INVALID_PTS;

    while (1) {
        if (stm_chan_need_read(chan_id) != STM_OK) {
            break;
        }

        if (stm_raw_read_ext(chan_id, raw_packet) != STM_OK) {
            break;
        }

        stm_chan_raw_count(chan_id, raw_packet);

        if (stm_raw_insert(chan_id, raw_packet) != STM_OK) {
            break;
        }
#ifdef VDEC_PROC_SUPPORT
        stm_log_save_raw(chan_id, raw_packet);
#endif
        if (chan->cfg.is_android_test == 1) {
            break;
        }
    }

    return;
}

td_s32 stm_chan_run_proc(td_void)
{
    td_s32 cnt;
    td_s32 ret = STM_ERR;
    td_s32 chan_id;
    stm_chan *chan = TD_NULL;
    stm_entry *entry = TD_NULL;
    stm_chan *chan_array = TD_NULL;

    entry = stm_ctrl_get_entry();
    chan_array = STM_CTRL_GET_CHAN(0);

    /* delete prev start chan raw */
    chan_id = entry->start_chan;
    chan = chan_array + chan_id;
    if (chan->handle != TD_NULL && chan->state == STM_CHAN_STATE_RUN) {
        if (chan->cfg.scd_bypass == 0 || chan->cfg.seg_bypass == 1 || chan->raw_list->need_release == 1) {
            stm_raw_delete(chan_id);
            if (chan->raw_list->need_release == 1) {
                chan->raw_list->need_release = 0;
            }
        }
    }

    chan_id = entry->this_chan;
    for (cnt = 0; cnt < STM_CHAN_NUM; cnt++) {
        chan_id = (chan_id + 1) % STM_CHAN_NUM;
        chan = chan_array + chan_id;

        /* for cpu load only proc one chan */
        if (chan->handle != TD_NULL && chan->state == STM_CHAN_STATE_RUN) {
            if (chan->cfg.scd_bypass == 0 || chan->cfg.seg_bypass == 1) {
                stm_raw_delete(chan_id);
            }

            ret = stm_chan_alloc_buf(chan_id);
            stm_chan_fill_raw(chan_id);
            entry->this_chan = chan_id;
            break;
        }
    }

    return ret;
}

td_void stm_chan_print_block(td_s32 chan_id, const td_char *info)
{
    td_u32 cur_time;
    stm_chan *chan = STM_CTRL_GET_CHAN(chan_id);

    vfmw_check(info != TD_NULL);
    vfmw_check(os_get_time_ms != TD_NULL);

    cur_time = os_get_time_ms();
    if (cur_time - chan->block_time < 300) { /* 300 is time period */
        return;
    }

    chan->block_time = cur_time;

    dprint(PRN_STM_BLK, "chan %d stm block of %s\n", chan->id, info);
}

td_void stm_chan_lock(td_s32 chan_id, td_ulong *lock_flag)
{
    vfmw_check(lock_flag != TD_NULL);

    os_spin_lock(stm_ctrl_get_entry()->chan[chan_id].lock, lock_flag);
}

td_void stm_chan_unlock(td_s32 chan_id, td_ulong *lock_flag)
{
    vfmw_check(lock_flag != TD_NULL);

    os_spin_unlock(stm_ctrl_get_entry()->chan[chan_id].lock, lock_flag);
}

td_s32 stm_chan_set_es_invalid(td_s32 chan_id)
{
    stm_chan *chan = STM_CTRL_GET_CHAN(chan_id);

    chan->es_valid = TD_FALSE;

    return STM_OK;
}

td_s32 stm_chan_need_lowldy(td_void)
{
    td_s32 id;
    stm_chan *chan = TD_NULL;
    stm_chan *chan_array = TD_NULL;

    chan_array = STM_CTRL_GET_CHAN(0);
    for (id = 0; id < STM_CHAN_NUM; id++) {
        chan = chan_array + id;
        if (chan->handle == TD_NULL ||
            chan->cfg.fill_eop_en == 0 ||
            chan->state != STM_CHAN_STATE_RUN ||
            chan->raw_list == TD_NULL ||
            chan->seg_list_info == TD_NULL) {
            continue;
        }

        if (stm_raw_get_fresh_num(id) +
            stm_seg_get_fresh_num(id) < 0x2) {
            return STM_OK;
        }
    }

    return STM_ERR;
}

