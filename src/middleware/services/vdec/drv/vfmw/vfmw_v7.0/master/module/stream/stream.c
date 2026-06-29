/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "stream.h"
#include "dbg.h"
#include "vfmw_define.h"
#include "vdec_chr.h"

#define MY_BREAK break

#define STREAM_BUFFER_POOL_SIZE 10
#define STREAM_BUFFER_NAME      "vfmw_es_buf"
#define STREAM_IDR_PTS_THD      40000  /* 40ms */
#define STREAM_INVALID_PTS      0xFFFFFFFF
#define STREAM_NAME_LEN         50

#define stream_find_tail_block(inst, block) do { \
        (block) = os_list_entry((inst)->list.prev, stream_block, node); \
} while (0)

#define stream_find_head_block(inst, block) do { \
        (block) = os_list_entry((inst)->list.next, stream_block, node); \
} while (0)

#define stream_find_first_free_block_(inst, block) do { \
        while (list != &((inst)->list)) { \
            tmp = os_list_entry(list, stream_block, node); \
            if (tmp->status == STREAM_STATUS_FREE) { \
                block = tmp; \
                MY_BREAK; \
            } \
            list = list->next; \
        } \
} while (0)

#define stream_find_first_free_block(inst, block) do { \
        stream_block *tmp = TD_NULL; \
        struct os_list_head *list = (inst)->list.next; \
        stream_find_first_free_block_(inst, block); \
} while (0)

#define stream_find_block_by_addr_(addr, inst, block) do { \
        os_list_for_each_safe(pos, n, &(inst)->list) { \
            tmp = os_list_entry(pos, stream_block, node); \
            if ((addr) == tmp->buf.phy_addr) { \
                block = tmp; \
                MY_BREAK; \
            } \
        } \
} while (0)

#define stream_find_block_by_addr(addr, inst, block) do { \
        struct os_list_head *pos = TD_NULL; \
        struct os_list_head *n = TD_NULL; \
        stream_block *tmp = TD_NULL; \
        stream_find_block_by_addr_(addr, inst, block); \
} while (0)

typedef struct {
    stream_chan chan[VFMW_CHAN_NUM];
    td_bool init;
    fun_vfmw_event_report event_report_vdec;
} stream_entry;

static stream_entry g_stream_entry = {{{0}}, 0, 0};

static stream_entry *stream_get_entry(td_void)
{
    return &g_stream_entry;
}

stream_chan *stream_get_chan(td_s32 chan_id)
{
    stream_entry *entry = stream_get_entry();

    return &entry->chan[chan_id];
}

td_void stream_lock_chan(td_s32 chan_id)
{
    stream_chan *chan = stream_get_chan(chan_id);
    os_sema_down(chan->sema);
}

td_void stream_unlock_chan(td_s32 chan_id)
{
    stream_chan *chan = stream_get_chan(chan_id);
    os_sema_up(chan->sema);
}

STATIC td_void stream_free_list(stream_chan *chan)
{
    stream_block *block = TD_NULL;
    struct os_list_head *pos = TD_NULL;
    struct os_list_head *n = TD_NULL;

    if (!os_list_empty(&chan->list)) {
        os_list_for_each_safe(pos, n, &chan->list) {
            block = os_list_entry(pos, stream_block, node);
            os_list_del(pos);
            block->is_used = TD_FALSE;
        }
    }

    return;
}

STATIC stream_block *stream_alloc_block(stream_chan *chan, td_u32 idx)
{
    td_u32 i;
    td_u32 j;
    td_u32 num;
    td_u32 alloc_num;
    td_u32 mini_num;
    stream_block *block = TD_NULL;
    td_s32 ret;
    td_char name[STREAM_NAME_LEN];

    num = (td_u32)sizeof(chan->blk_mem) / (td_u32)sizeof(chan->blk_mem[0]);
    for (i = 0; i < num; i++) {
        if (chan->blk_mem[i] == TD_NULL) {
            break;
        }
    }

    if (i >= num) {
        return block;
    }

    /* alloc more blocks for alloc less and free faster */
    alloc_num = vfmw_align_up(chan->max_raw_num, num) / num;
    ret = snprintf_s(name, STREAM_NAME_LEN, STREAM_NAME_LEN - 1, "stream_block_%u", i);
    vfmw_check_ret(ret >= 0, TD_NULL);
    vfmw_check_ret(os_alloc_vir != TD_NULL, TD_NULL);
    block = os_alloc_vir(chan->chan_id, name, sizeof(stream_block) * alloc_num);
    if (block != TD_NULL) {
        chan->blk_mem[i] = block;
        mini_num = vfmw_min(alloc_num, chan->max_raw_num - i * alloc_num);
        for (j = 0; j < mini_num; j++) {
            block->is_used = TD_FALSE;
            chan->block[idx + j] = block;
            block++;
        }
        block = chan->block[idx];
    }

    return block;
}

STATIC stream_block *stream_get_manger_block(stream_chan *chan)
{
    td_u32 i = 0;
    td_u32 j = 0;
    stream_block *block = TD_NULL;

    vfmw_check_ret(chan->max_raw_num != 0, TD_NULL);

    if (chan->raw_num >= chan->max_raw_num) {
        dprint(PRN_ERROR, "chan->raw_num(%d) >= chan->max_raw_num(%d)\n", chan->raw_num, chan->max_raw_num);
        return TD_NULL;
    }

    if (chan->block == TD_NULL) {
        vfmw_check_ret(os_alloc_vir != TD_NULL, TD_NULL);
        chan->block = (stream_block **)os_alloc_vir(chan->chan_id, "stream_block_pool",
            sizeof(stream_block *) * chan->max_raw_num);
        if (chan->block == TD_NULL) {
            return TD_NULL;
        }
        vfmw_check_sec_func(memset_s(chan->block, sizeof(stream_block *) * chan->max_raw_num,
                                     0, sizeof(stream_block *) * chan->max_raw_num));
    }

    for (j = 0; j < chan->max_raw_num; j++) {
        i = (chan->blk_next_idx + j) % chan->max_raw_num;
        if (chan->block[i] == TD_NULL) {
            break;
        }

        if (chan->block[i]->is_used == TD_FALSE) {
            block = chan->block[i];
            break;
        }
    }

    if (block == TD_NULL && j < chan->max_raw_num) {
        block = stream_alloc_block(chan, i);
    }

    if (block != TD_NULL) {
        block->is_used = TD_TRUE;
        chan->raw_num++;
        chan->blk_next_idx = (i + 1) % chan->max_raw_num;
    }

    return block;
}

STATIC td_void stream_put_manger_block(stream_chan *chan, stream_block *block)
{
    block->is_used = TD_FALSE;
    chan->raw_num--;
}

STATIC td_void stream_free_manger_block(stream_chan *chan)
{
    /* if free 4096 mem cost 800us, now 16 mem 20us */
    td_u32 i;
    td_u32 num;

    if (chan->block != TD_NULL) {
        os_free_vir(chan->block);
        chan->block = TD_NULL;
    }

    num = (td_u32)sizeof(chan->blk_mem) / (td_u32)sizeof(chan->blk_mem[0]);
    for (i = 0; i < num; i++) {
        if (chan->blk_mem[i]  == TD_NULL) {
            break;
        }

        os_free_vir(chan->blk_mem[i]);
        chan->blk_mem[i] = TD_NULL;
    }

    chan->raw_num = 0;
}

STATIC td_s32 stream_deq_buf_prepare(stream_chan *chan,
    const vfmw_stream_buf *buffer, stream_block **block_out)
{
    stream_block *block = TD_NULL;

    if (chan->is_used == TD_FALSE || chan->start_phy_addr == 0) {
        dprint(PRN_ERROR, "%s %d chan is not create!\n", __func__, __LINE__);
        return EXT_FAILURE;
    }

    block = stream_get_manger_block(chan);
    if (block == TD_NULL) {
        dprint(PRN_ERROR, "alloc stream block fail! chan->raw_num=%d\n", chan->raw_num);
        return EXT_FAILURE;
    }

    if (chan->free_len < buffer->length) {
        stream_put_manger_block(chan, block);
        return EXT_FAILURE;
    }

    *block_out = block;

    return EXT_SUCCESS;
}

STATIC td_s32 stream_deq_buf_check_state(stream_chan *chan,
    vfmw_stream_buf *buffer, stream_block *block, td_u32 *write_offset, td_u32 *read_offset)
{
    stream_block *block_tail = TD_NULL;
    stream_block *block_head = TD_NULL;

    if (os_list_empty(&chan->list) == TD_TRUE) {
        *read_offset = *write_offset = 0;
    } else {
        stream_find_tail_block(chan, block_tail);
        if (block_tail->status == STREAM_STATUS_WRITE) {
            vfmw_check_sec_func(memcpy_s(buffer, sizeof(vfmw_stream_buf), &block_tail->buf, sizeof(block_tail->buf)));
            stream_put_manger_block(chan, block);
            return EXT_SUCCESS;
        }
        *write_offset = (td_u32)(block_tail->buf.phy_addr + block_tail->buf.length  - chan->start_phy_addr);

        stream_find_head_block(chan, block_head);
        *read_offset = (td_u32)(block_head->buf.phy_addr - chan->start_phy_addr);
    }

    return EXT_FAILURE;
}

STATIC td_void stream_deq_buf_fill_info(stream_chan *chan,
    vfmw_stream_buf *buffer, stream_block *block, td_u32 write_offset)
{
    buffer->phy_addr = block->buf.phy_addr = chan->start_phy_addr + write_offset;
    buffer->kern_vir_addr = block->buf.kern_vir_addr = (chan->start_kern_vir_addr == 0) ?
                                                       0 : chan->start_kern_vir_addr + write_offset;
    buffer->usr_vir_addr = block->buf.usr_vir_addr = (chan->start_usr_vir_addr == 0) ?
                                                     0 : (chan->start_usr_vir_addr + write_offset);
    buffer->length = block->buf.length = buffer->length;
    buffer->pts = block->buf.pts = VFMW_INVALID_PTS;
    buffer->is_end_of_frm = block->buf.is_end_of_frm = 0;
    buffer->is_dual_layer = block->buf.is_dual_layer = 0;
    buffer->offset = block->buf.offset = write_offset;
    buffer->handle = block->buf.handle = chan->mem_fd;
}

STATIC td_s32 stream_deq_buffer_internal(stream_chan *chan, vfmw_stream_buf *buffer)
{
    td_u32 read_offset;
    td_u32 write_offset = 0;
    stream_block *block = TD_NULL;
    td_bool alloc = TD_FALSE;
    td_u32 tail_free;
    td_u32 head_free;

    if (stream_deq_buf_prepare(chan, buffer, &block) != EXT_SUCCESS) {
        return EXT_FAILURE;
    }

    if (stream_deq_buf_check_state(chan, buffer, block, &write_offset, &read_offset) == EXT_SUCCESS) {
        dprint(PRN_ERROR, "double deq same buf!\n");
        return EXT_SUCCESS;
    }

    if (write_offset > chan->length) {
        stream_put_manger_block(chan, block);
        dprint(PRN_ERROR, "write_offset(%d) > chan->length(%d)\n", write_offset, chan->length);
        return EXT_FAILURE;
    }

    if (write_offset <= read_offset) {
        alloc = TD_TRUE;
    } else {
        tail_free = chan->length - write_offset;
        head_free = chan->free_len - tail_free;

        if (tail_free >= buffer->length) {
            alloc = TD_TRUE;
        } else if (head_free >= buffer->length) {
            alloc = TD_TRUE;
            write_offset = 0;
            chan->freeze_size = tail_free;
            chan->free_len -= tail_free;
        } else {
            dprint(PRN_ERROR, "head free:%x tail free:%x raw len:%x\n", head_free, tail_free, buffer->length);
        }
    }

    chan->write_offst = write_offset;
    chan->read_offst = read_offset;

    if (alloc != TD_TRUE) {
        stream_put_manger_block(chan, block);
        dprint(PRN_ERROR, "not enough buf!\n");
        return EXT_FAILURE;
    }

    stream_deq_buf_fill_info(chan, buffer, block, write_offset);

    block->status = STREAM_STATUS_WRITE;

    os_list_add_tail(&block->node, &chan->list);

    return EXT_SUCCESS;
}

STATIC td_s32 stream_deq_buffer_outer(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *head_block = TD_NULL;

    if (chan->is_used == TD_FALSE || os_list_empty(&chan->list) == TD_TRUE) {
        return EXT_FAILURE;
    }

    stream_find_head_block(chan, head_block);
    if (head_block->status != STREAM_STATUS_WRITE) {
        return EXT_FAILURE;
    }

    vfmw_check_sec_func(memcpy_s(buffer, sizeof(vfmw_stream_buf), &head_block->buf, sizeof(head_block->buf)));

    os_list_del(&head_block->node);
    stream_put_manger_block(chan, head_block);

    return EXT_SUCCESS;
}

STATIC td_s32 stream_que_buffer_internal(stream_chan *chan, const vfmw_stream_buf *buffer)
{
    stream_block *block_tail = TD_NULL;
    UADDR phy_addr = buffer->phy_addr;

    if (chan->is_used == TD_FALSE) {
        dprint(PRN_ERROR, "%s %d return err\r\n", __func__, __LINE__);
        return EXT_FAILURE;
    }

    stream_find_tail_block(chan, block_tail);
    if (block_tail->buf.length < buffer->length) {
        dprint(PRN_ERROR, "%s %d return err [%d < %d]\r\n", __func__, __LINE__, block_tail->buf.length,
            buffer->length);
        return EXT_FAILURE;
    }

    if (block_tail->status == STREAM_STATUS_WRITE &&
       (block_tail->buf.phy_addr == phy_addr || block_tail->buf.offset == buffer->offset)) {
        if (buffer->length == 0) {
            os_list_del(&block_tail->node);
            stream_put_manger_block(chan, block_tail);
            return EXT_SUCCESS;
        }

        block_tail->buf.is_dual_layer = buffer->is_dual_layer;
        block_tail->buf.is_end_of_frm = buffer->is_end_of_frm;
        block_tail->buf.pts = buffer->pts;
        block_tail->buf.length = buffer->length;
        block_tail->buf.disp_time = buffer->disp_time;
        block_tail->status = STREAM_STATUS_FREE;
    } else {
        dprint(PRN_ERROR, "%s %d return err\r\n", __func__, __LINE__);
        return EXT_FAILURE;
    }

    chan->free_len -= buffer->length;
    chan->used_len += buffer->length;

    return EXT_SUCCESS;
}

STATIC td_s32 stream_que_buffer_outer(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block = TD_NULL;

    block = stream_get_manger_block(chan);
    if (block == TD_NULL) {
        return EXT_FAILURE;
    }

    vfmw_check_sec_func(memcpy_s(&block->buf, sizeof(block->buf), buffer, sizeof(vfmw_stream_buf)));

    block->status = STREAM_STATUS_FREE;

    if (chan->is_used == TD_FALSE) {
        stream_put_manger_block(chan, block);
        return EXT_FAILURE;
    }

    block->buf.phy_addr = buffer->phy_addr;

    os_list_add_tail(&block->node, &chan->list);

    return EXT_SUCCESS;
}

STATIC td_s32 stream_read_internal(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block = TD_NULL;

    if (chan->is_used == TD_FALSE || chan->used_len == 0 || os_list_empty(&chan->list)) {
        return EXT_FAILURE;
    }

    stream_find_first_free_block(chan, block);
    if (block == TD_NULL) {
        return EXT_FAILURE;
    }

    buffer->phy_addr = block->buf.phy_addr;
    buffer->kern_vir_addr = block->buf.kern_vir_addr;
    buffer->usr_vir_addr = block->buf.usr_vir_addr;

    buffer->pts = block->buf.pts;
    buffer->length = block->buf.length;
    buffer->marker = block->buf.marker;
    buffer->is_dual_layer = block->buf.is_dual_layer;
    buffer->is_end_of_frm = block->buf.is_end_of_frm;
    buffer->handle = block->buf.handle;
    buffer->offset = block->buf.offset;
    buffer->disp_time = block->buf.disp_time;

    block->status = STREAM_STATUS_READ;

    return EXT_SUCCESS;
}

STATIC td_s32 stream_read_outer(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block = TD_NULL;

    if (chan->is_used == TD_FALSE || os_list_empty(&chan->list) == TD_TRUE) {
        return EXT_FAILURE;
    }

    stream_find_first_free_block(chan, block);
    if (block == TD_NULL) {
        return EXT_FAILURE;
    }

    buffer->phy_addr = block->buf.phy_addr;
    buffer->kern_vir_addr = block->buf.kern_vir_addr;

    buffer->pts = block->buf.pts;
    buffer->length = block->buf.length;
    buffer->marker = block->buf.marker;
    buffer->is_dual_layer = block->buf.is_dual_layer;
    buffer->is_end_of_frm = block->buf.is_end_of_frm;
    buffer->disp_enable_flag = block->buf.disp_enable_flag;
    buffer->disp_frm_distance = block->buf.disp_frm_distance;
    buffer->disp_time = block->buf.disp_time;
    buffer->distance_before_first_frame = block->buf.distance_before_first_frame;
    buffer->gop_num = block->buf.gop_num;

    block->status = STREAM_STATUS_READ;

    return EXT_SUCCESS;
}

STATIC td_s32 stream_release_internal(stream_chan *chan, const vfmw_stream_buf *buffer)
{
    stream_block *block = TD_NULL;
    UADDR phy_addr;

    if (chan->is_used == TD_FALSE) {
        return EXT_FAILURE;
    }

    phy_addr = buffer->phy_addr;

    stream_find_block_by_addr(phy_addr, chan, block);
    if (block == TD_NULL) {
        dprint(PRN_ERROR, "can not find block!\n");
        return EXT_FAILURE;
    }

    if (chan->freeze_size != 0 &&
        block->buf.phy_addr + block->buf.length == chan->start_phy_addr + chan->length - chan->freeze_size) {
        chan->free_len += chan->freeze_size;
        chan->freeze_size = 0;
    }

    if (block->status != STREAM_STATUS_READ) {
        dprint(PRN_ERROR, "block state is %d!\n", block->status);
        return EXT_FAILURE;
    }

    chan->used_len -= block->buf.length;
    chan->free_len += block->buf.length;

    os_list_del(&block->node);
    stream_put_manger_block(chan, block);

    return EXT_SUCCESS;
}

STATIC td_s32 stream_release_outer(stream_chan *chan, const vfmw_stream_buf *buffer)
{
    stream_block *block = TD_NULL;
    UADDR phy_addr;

    phy_addr = buffer->phy_addr;

    if (chan->is_used == TD_FALSE) {
        return EXT_FAILURE;
    }

    stream_find_block_by_addr(phy_addr, chan, block);
    if (block == TD_NULL) {
        return EXT_FAILURE;
    }

    if (block->status != STREAM_STATUS_READ) {
        return EXT_FAILURE;
    }

    block->status = STREAM_STATUS_WRITE;

    return EXT_SUCCESS;
}

STATIC td_void stream_release_report(stream_chan *chan, const vfmw_stream_buf *buffer)
{
    td_s32 evt_ret = EXT_SUCCESS;
    stream_entry *entry = stream_get_entry();

    if (chan->adpt_type == ADPT_TYPE_VDEC && entry->event_report_vdec != TD_NULL &&
        (chan->evt_map & EVNT_RLS_STREAM) != 0) {
        evt_ret = entry->event_report_vdec(chan->chan_id, EVNT_RLS_STREAM, buffer, sizeof(vfmw_stream_buf));
    }

    if (evt_ret != EXT_SUCCESS) {
        dprint(PRN_ERROR, "%s chan %d call event call baclk fail!\n", __func__, chan->chan_id);
    }
}

STATIC td_bool stream_is_frm_mode(td_s32 vid_std, const vfmw_std_ext *std_ext)
{
    vfmw_unused(std_ext);
    if (vid_std == VFMW_H264 || vid_std == VFMW_HEVC) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

STATIC td_void stream_save_raw(td_s32 chan_id, vfmw_stream_buf *raw_buf)
{
    td_s32 write_len;
    const td_u32 ivf_file_head = 32;
    const td_u32 ivf_frm_head = 12;
    const td_u32 raw_frm_head = 4;
    td_u8 *raw_vir = TD_NULL;
    stream_chan *chan = TD_NULL;
    mem_record mem_rec = {0};
    const td_char *ivf_signature = "DKIF";

    chan = stream_get_chan(chan_id);
    if (chan->file == TD_NULL || chan->is_sec) {
        return;
    }

    mem_rec.phy_addr = raw_buf->phy_addr;
    mem_rec.vir_addr = os_map_mem(&mem_rec);
    vfmw_check(mem_rec.vir_addr != TD_NULL);

    raw_vir = mem_rec.vir_addr;

    if (chan->frm_mode && (chan->vid_std != VFMW_MJPEG)) {
        if (chan->vid_std == VFMW_VP9 || chan->vid_std == VFMW_AV1) {
            if (chan->queue_ok_cnt == 1) {
                os_fwrite(ivf_signature, ivf_file_head, chan->file);
            }
            os_fwrite((char *)(&raw_buf->length), ivf_frm_head, chan->file);
        } else {
            os_fwrite((char *)(&raw_buf->length), raw_frm_head, chan->file);
        }
    }

    write_len = os_fwrite((char *)raw_vir, raw_buf->length, chan->file);
    if ((td_u32)write_len < raw_buf->length) {
        dprint(PRN_ERROR, "%s %d error !\n", __func__, __LINE__);
    }
    os_fsync(chan->file);

    if (mem_rec.vir_addr != TD_NULL) {
        os_unmap_mem(&mem_rec);
    }
}

td_void stream_open1(td_void)
{
    td_s32 i;
    stream_chan *chan = TD_NULL;

    for (i = 0; i < VFMW_CHAN_NUM; i++) {
        chan = stream_get_chan(i);
        os_sema_init(&chan->sema);
    }
}

td_void stream_close(td_void)
{
    td_s32 i;
    stream_chan *chan = TD_NULL;

    for (i = 0; i < VFMW_CHAN_NUM; i++) {
        chan = stream_get_chan(i);
        os_sema_exit(chan->sema);
    }
}

td_s32 stream_set_callback(vfmw_adpt_type type, fun_vfmw_event_report fp_event_report)
{
    stream_entry *entry = stream_get_entry();
    vfmw_check_ret(fp_event_report != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(entry != TD_NULL, EXT_FAILURE);

    if (type == ADPT_TYPE_VDEC) {
        entry->event_report_vdec = fp_event_report;
    }
    return EXT_SUCCESS;
}

td_void stream_init(const vfmw_init_param *param)
{
    stream_entry *entry = stream_get_entry();
    td_s32 ret;

    vfmw_check(param != TD_NULL);
    if (entry->init == TD_FALSE) {
        entry->init = TD_TRUE;
    }

    ret = stream_set_callback(param->adpt_type, param->fn_callback);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_ERROR, "%s %d stream_set_callback failed!\n", __func__, __LINE__);
    }
}

td_void stream_exit(td_void)
{
    stream_entry *entry = stream_get_entry();

    if (entry->init == TD_TRUE) {
        entry->init = TD_FALSE;
    }
}

td_s32 stream_create_chan(td_s32 chan_id, const vfmw_chan_option *option)
{
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(option != TD_NULL, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    if (chan->is_used == TD_TRUE) {
        stream_unlock_chan(chan_id);
        dprint(PRN_FATAL, "%s %d chan %d is busy!\n", __func__, __LINE__, chan_id);
        return EXT_FAILURE;
    }

    chan->is_used = TD_TRUE;
    chan->chan_id = chan_id;
    chan->adpt_type = option->adpt_type;
    chan->mode = STREAM_BUF_ALLOC_OUTER;
    chan->freeze_size = 0;
    chan->free_len = 0;
    chan->length = 0;
    chan->used_len = 0;
    chan->start_phy_addr = 0;
    chan->start_kern_vir_addr = 0;
    chan->start_usr_vir_addr = 0;
    chan->is_sec = option->is_sec_mode;
    chan->vdec_handle = option->vdec_handle;
    chan->ssm_handle = option->ssm_handle;
    chan->try_read_cnt = 0;
    chan->try_release_cnt = 0;
    chan->read_ok_cnt = 0;
    chan->release_ok_cnt = 0;
    chan->try_dequeue_cnt = 0;
    chan->dequeue_ok_cnt = 0;
    chan->try_queue_cnt = 0;
    chan->queue_ok_cnt = 0;
    chan->eos_flag = TD_FALSE;
    chan->que_eos_cnt = 0;
    chan->raw_num = 0;
    chan->blk_next_idx = 0;
    chan->block = TD_NULL;
    chan->en_idr_pts = 0;
    chan->idr_pts = 0;
    vfmw_check_sec_func(memset_s(chan->blk_mem, sizeof(td_void *) * 0x10, 0, sizeof(td_void *) * 0x10));

    os_list_init_head(&chan->list);

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_void stream_destroy_chan(td_s32 chan_id)
{
    stream_chan *chan = TD_NULL;
    vfmw_mem_desc mem = {0};

    vfmw_check(chan_id >= 0 && chan_id < VFMW_CHAN_NUM);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->is_used = TD_FALSE;
    stream_free_list(chan);
    stream_free_manger_block(chan);

    if (chan->start_phy_addr != 0) {
        mem.phy_addr = chan->start_phy_addr;
        mem.vir_addr = chan->start_kern_vir_addr;
        mem.length = chan->length;
        mem.mode = chan->is_sec ? MEM_MMU_SEC : MEM_MMU_MMU;
        os_kmem_free(&mem);
        chan->start_phy_addr = 0;
        chan->start_kern_vir_addr = 0;
        chan->start_usr_vir_addr = 0;
        chan->length = 0;
    }

    stream_unlock_chan(chan_id);
}

td_s32 stream_alloc_buffer(td_s32 chan_id, td_u32 *size)
{
    td_s32 ret;
    vfmw_mem_desc mem = {0};
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(size != TD_NULL, EXT_FAILURE);
    vfmw_check_ret((*size) > 0, EXT_FAILURE);
    vfmw_check_ret(os_kmem_alloc != TD_NULL, EXT_FAILURE);

    chan = stream_get_chan(chan_id);

    mem.vdec_handle = chan->vdec_handle;
    mem.unid = (td_u32)chan_id;
    ret = os_kmem_alloc(STREAM_BUFFER_NAME, *size, 16, 0, &mem); /* 16 align */
    if (ret != OSAL_OK) {
        dprint(PRN_ERROR, "%s %d alloc es buffer fail!\n", __func__, __LINE__);
        return EXT_FAILURE;
    }

    stream_lock_chan(chan_id);

    if (chan->is_used == TD_FALSE || chan->start_phy_addr != 0) {
        stream_unlock_chan(chan_id);
        os_kmem_free(&mem);
        dprint(PRN_ERROR, "%s %d chan %d is not create!\n", __func__, __LINE__, chan_id);
        return EXT_FAILURE;
    }

    chan->start_phy_addr = mem.phy_addr;
    chan->start_kern_vir_addr = mem.vir_addr;
    chan->length = mem.length;
    chan->write_offst = 0;
    chan->read_offst = 0;
    chan->free_len = mem.length;
    chan->used_len = 0;
    chan->freeze_size = 0;
    chan->mode = STREAM_BUF_ALLOC_INNER;
    chan->eos_flag = TD_FALSE;
    (*size) = mem.phy_addr;

    stream_unlock_chan(chan_id);
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_INIT_INPUT_BUF_OK);

    return EXT_SUCCESS;
}

td_s32 stream_free_buffer(td_s32 chan_id)
{
    stream_chan *chan = TD_NULL;
    vfmw_mem_desc mem = {0};

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    if (chan->is_used == TD_FALSE) {
        stream_unlock_chan(chan_id);
        dprint(PRN_ERROR, "%s %d chan %d is not create!\n", __func__, __LINE__, chan_id);
        return EXT_FAILURE;
    }
    if (chan->start_phy_addr != 0) {
        mem.phy_addr = chan->start_phy_addr;
        mem.vir_addr = chan->start_kern_vir_addr;
        mem.length = chan->length;
        mem.mode = chan->is_sec ? MEM_MMU_SEC : MEM_MMU_MMU;
        os_kmem_free(&mem);
        chan->start_phy_addr = 0;
        chan->start_kern_vir_addr = 0;
        chan->start_usr_vir_addr = 0;
        chan->length = 0;
    }

    stream_free_list(chan);
    stream_free_manger_block(chan);
    chan->mode = STREAM_BUF_ALLOC_OUTER; /* for IFrm to DVB */

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 stream_bind_usr_addr(td_s32 chan_id, td_u64 usr_addr)
{
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    if (chan->is_used == TD_FALSE) {
        stream_unlock_chan(chan_id);
        dprint(PRN_ERROR, "%s %d chan %d is not create!\n", __func__, __LINE__, chan_id);
        return EXT_FAILURE;
    }

    chan->start_usr_vir_addr = usr_addr;

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 stream_dequeue_buffer(td_s32 chan_id, vfmw_stream_buf *raw_buf)
{
    td_s32 ret;
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(raw_buf != TD_NULL, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->try_dequeue_cnt++;

    if (chan->mode == STREAM_BUF_ALLOC_INNER) {
        ret = stream_deq_buffer_internal(chan, raw_buf);
    } else {
        ret = stream_deq_buffer_outer(chan, raw_buf);
    }

    if (ret == EXT_SUCCESS) {
        chan->dequeue_ok_cnt++;
    }

    stream_unlock_chan(chan_id);

    return ret;
}

td_s32 stream_queue_buffer(td_s32 chan_id, vfmw_stream_buf *raw_buf)
{
    td_s32 ret;
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(raw_buf != TD_NULL, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->try_queue_cnt++;

    if (chan->mode == STREAM_BUF_ALLOC_INNER) {
        ret = stream_que_buffer_internal(chan, raw_buf);
    } else {
        ret = stream_que_buffer_outer(chan, raw_buf);
    }

    if (ret == EXT_SUCCESS) {
        chan->queue_ok_cnt++;
        stream_save_raw((td_s32)chan_id, raw_buf);
    }

    stream_unlock_chan(chan_id);

    return ret;
}

td_s32 stream_read_buffer(td_s32 chan_id, vfmw_stream_buf *buffer)
{
    td_s32 ret;
    stream_chan *chan = TD_NULL;
    td_s64 target_pts;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(buffer != TD_NULL, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->try_read_cnt++;
    if (chan->mode == STREAM_BUF_ALLOC_INNER) {
        ret = stream_read_internal(chan, buffer);
    } else {
        ret = stream_read_outer(chan, buffer);
    }

    if (ret != EXT_SUCCESS) {
        if (chan->eos_flag == TD_TRUE) {
            chan->eos_flag = TD_FALSE;
            chan->read_ok_cnt++;
            stream_unlock_chan(chan_id);
            return STREAM_EOS;
        }
        stream_unlock_chan(chan_id);
        return EXT_FAILURE;
    }

    if (chan->en_idr_pts == TD_TRUE) {
        target_pts = chan->idr_pts > STREAM_IDR_PTS_THD ? chan->idr_pts - STREAM_IDR_PTS_THD : chan->idr_pts;
        if (buffer->pts >= target_pts && buffer->pts != VFMW_INVALID_PTS) {
            chan->en_idr_pts = TD_FALSE;
        } else {
            if (chan->mode == STREAM_BUF_ALLOC_OUTER) {
                ret = stream_release_outer(chan, buffer);
            } else {
                ret = stream_release_internal(chan, buffer);
            }

            if (ret == EXT_SUCCESS) {
                stream_release_report(chan, buffer);
                stream_unlock_chan(chan_id);
                return EXT_FAILURE;
            }
        }
    }

    chan->read_ok_cnt++;

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 stream_release_buffer(td_s32 chan_id, vfmw_stream_buf *buffer)
{
    td_s32 ret;

    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(buffer != TD_NULL, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->try_release_cnt++;

    if (chan->mode == STREAM_BUF_ALLOC_INNER) {
        ret = stream_release_internal(chan, buffer);
    } else {
        ret = stream_release_outer(chan, buffer);
    }

    if (ret == EXT_SUCCESS) {
        chan->release_ok_cnt++;
        stream_release_report(chan, buffer);
    }

    stream_unlock_chan(chan_id);

    return ret;
}

td_s32 stream_send_eos(td_s32 chan_id)
{
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);
    chan->eos_flag = TD_TRUE;
    chan->que_eos_cnt++;
    stream_unlock_chan(chan_id);
    vdec_record_chr_event(chan_id, MT_VDEC_SEND_EOS, MT_VDEC_HW_SEND_EOS, MT_VDEC_SET_HW_SEND_EOS_OK);

    return EXT_SUCCESS;
}

td_s32 stream_reset(td_s32 chan_id)
{
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);
    stream_free_list(chan);
    chan->eos_flag = TD_FALSE;
    chan->freeze_size = 0;
    chan->free_len = chan->length;
    chan->used_len = 0;
    chan->read_offst = 0;
    chan->write_offst = 0;
    chan->try_read_cnt = 0;
    chan->try_release_cnt = 0;
    chan->read_ok_cnt = 0;
    chan->release_ok_cnt = 0;
    chan->raw_num = 0;
    chan->blk_next_idx = 0;
    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 stream_get_es_info(td_s32 chan_id, vfmw_stream_buf *raw_buf)
{
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(raw_buf != TD_NULL, EXT_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);

    raw_buf->phy_addr = chan->start_phy_addr;
    raw_buf->length = chan->length;

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 stream_attach_es_buf(td_s32 chan_id, vfmw_stream_buf *raw_buf)
{
#ifdef VFMW_SEC_SUPPORT
    td_s32 ret;
    stream_chan *chan = TD_NULL;
    ext_drv_ssm_buf_attach_info attach_info;
    td_u64 sec_info_addr;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(raw_buf != TD_NULL, EXT_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);

    attach_info.module_handle = chan->vdec_handle;
    attach_info.session_handle = chan->ssm_handle;
    stream_unlock_chan(chan_id);

    attach_info.buf_id = EXT_SSM_BUFFER_ID_DMX_VID_ES_BUF;

    ret = ext_drv_ssm_attach_buffer(attach_info, &sec_info_addr);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_FATAL, "vfmw attach esbuf fail\n");
    }
#else
    vfmw_unused(chan_id);
    vfmw_unused(raw_buf);
#endif

    return EXT_SUCCESS;
}

td_s32 stream_set_idr_pts(td_s32 chan_id, td_s64 idr_pts)
{
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);
    chan->en_idr_pts = TD_TRUE;
    chan->idr_pts = idr_pts;

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 stream_get_chan_status(td_s32 chan_id, vfmw_chan_info *status)
{
    stream_chan *chan = TD_NULL;
    stream_block *block = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(status != TD_NULL, EXT_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);

    status->is_end_of_stm = chan->eos_flag;
    status->free_stm_buf_size = chan->free_len;
    status->total_stm_buf_size = chan->length;
    status->used_stm_buf_size = chan->used_len;
    status->undecode_stm_num = 0;
    status->undecode_stm_size = 0;

    if (os_list_empty(&chan->list) != TD_TRUE) {
        os_list_for_each_entry(block, &chan->list, node) {
            if (block->status == STREAM_STATUS_FREE || block->status == STREAM_STATUS_READ) {
                status->undecode_stm_num++;
                status->undecode_stm_size += block->buf.length;
            }
        }
    }

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 stream_set_evt_map(td_s32 chan_id, td_u64 evt_map)
{
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->evt_map = evt_map;

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}

td_s32 stream_config(td_s32 chan_id, vfmw_chan_cfg *cfg)
{
    stream_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, EXT_FAILURE);
    vfmw_check_ret(cfg != TD_NULL, EXT_FAILURE);
    vfmw_check_ret(cfg->max_raw_packet_num != 0, EXT_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->max_raw_num = (td_u32)cfg->max_raw_packet_num;
    chan->vid_std = cfg->vid_std;
    chan->frm_mode = stream_is_frm_mode(cfg->vid_std, &cfg->std_ext);

    stream_unlock_chan(chan_id);

    return EXT_SUCCESS;
}
