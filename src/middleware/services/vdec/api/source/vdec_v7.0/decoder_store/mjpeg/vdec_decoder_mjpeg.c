/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "vdec_decoder_mjpeg.h"
#include "mpi_vdec_sdk.h"
#include "mpi_vdec_osal.h"
#include "soc_osal.h"
#include "vdec_jpeg_hal.h"
#include "vdec_hal.h"
#include "securec.h"
#include "vdec_chr.h"

typedef struct {
    jpeg_decompress_ptr jpeg_handle;
    vdec_jpeg_info jpeg_info;
} mjpeg_hal;

typedef struct {
    td_bool start;
    td_bool exit;
    td_bool eos;
    td_bool set_eos;
    td_bool have_stream;
    td_bool have_frame;
    ext_vdec_stream stream;
    vdec_usrdec_frame frame;
} mjpeg_data;

typedef struct {
    td_u32 get_stm_num;
    td_u32 put_stm_num;
    td_u32 get_frm_num;
    td_u32 put_frm_num;
    td_u32 suc_frm_num;
    td_u32 err_frm_num;
} mjpeg_statis;

typedef struct {
    /* not reset in clear_chan */
    td_s32 id;
    td_bool use;
    vdec_mutex mutex;
    pthread_cond_t cond;
    vdec_thread thread;
    /* reset in clear_chan */
    mjpeg_hal hal;
    mjpeg_data data;
    mjpeg_statis statis;
} mjpeg_chan;

static mjpeg_chan *g_mjpeg_chan_ptr[VDEC_MAX_CHAN_NUM] = {TD_NULL};
INTF STATIC td_s32 decoder_mjpeg_dequeue_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option,
    ext_vdec_stream *stream);
INTF STATIC td_s32 decoder_mjpeg_queue_stream(td_s32 chan_id, const ext_vdec_stream *stream);
INTF STATIC td_s32 decoder_mjpeg_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option,
    ext_vdec_frame *frame);

STATIC mjpeg_chan* decoder_mjpeg_get_chan(td_s32 chan_id)
{
    return g_mjpeg_chan_ptr[chan_id];
}

STATIC td_void decoder_mjpeg_init(td_void)
{
    td_s32 i;

    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        g_mjpeg_chan_ptr[i] = TD_NULL;
    }
}

STATIC td_void decoder_mjpeg_clear_chan(mjpeg_chan *chan)
{
    (td_void)memset_s(&chan->hal, sizeof(mjpeg_hal), 0, sizeof(mjpeg_hal));
    (td_void)memset_s(&chan->data, sizeof(mjpeg_data), 0, sizeof(mjpeg_data));
    (td_void)memset_s(&chan->statis, sizeof(mjpeg_statis), 0, sizeof(mjpeg_statis));
}

STATIC td_s32 decoder_mjpeg_get_stream(mjpeg_chan *chan, ext_vdec_stream *stream)
{
    td_s32 ret;

    ret = vdec_hal_command(chan->id, (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_GET_STREAM, stream, sizeof(ext_vdec_stream));
    if (ret != EXT_SUCCESS) {
        if (chan->data.set_eos == TD_TRUE && chan->data.eos == TD_FALSE) {
            chan->data.eos = TD_TRUE;
        }

        vdec_print_dbg("mjpeg chan %d get stream failed.\n", chan->id);

        return EXT_FAILURE;
    }

    chan->statis.get_stm_num++;

    return EXT_SUCCESS;
}

STATIC td_void decoder_mjpeg_put_stream(mjpeg_chan *chan, ext_vdec_stream *stream)
{
    td_s32 ret;

    chan->statis.put_stm_num++;

    ret = vdec_hal_command(chan->id, (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_PUT_STREAM, stream, sizeof(ext_vdec_stream));
    if (ret != EXT_SUCCESS) {
        vdec_print_info("mjpeg put stream failed!\n");
    }

    return;
}

STATIC td_void decoder_mjpeg_init_frame(const vdec_jpeg_info *info, const ext_vdec_stream *stream,
    vdec_usrdec_frame *frame)
{
    (td_void)memset_s(frame, sizeof(vdec_usrdec_frame), 0, sizeof(vdec_usrdec_frame));

    frame->pts = stream->pts;
    frame->disp_time = (td_u64)stream->pts;
    frame->width = info->width;
    frame->height = info->height;
    frame->luma_stride = info->luma_stride;
    frame->chrom_stride = info->chrom_stride;
    frame->chrom_offset = info->luma_size;
    frame->mem_size = info->luma_size + info->chrom_size;
    frame->fmt = info->fmt;
}

STATIC td_s32 decoder_mjpeg_get_frame(mjpeg_chan *chan, vdec_usrdec_frame *frame)
{
    td_s32 ret;

    ret = vdec_hal_command(chan->id, (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_GET_FRAME, frame, sizeof(vdec_usrdec_frame));
    if (ret != EXT_SUCCESS) {
        vdec_print_dbg("mjpeg chan %d get frame failed.\n", chan->id);
        return EXT_FAILURE;
    }

    chan->statis.get_frm_num++;

    return EXT_SUCCESS;
}

STATIC td_void decoder_mjpeg_put_frame(mjpeg_chan *chan, vdec_usrdec_frame *frame)
{
    td_s32 ret;

    chan->statis.put_frm_num++;

    ret = vdec_hal_command(chan->id, (ext_vdec_cmd_id)VDEC_PRIVATE_CMD_PUT_FRAME, frame, sizeof(vdec_usrdec_frame));
    if (ret != EXT_SUCCESS) {
        vdec_print_info("mjpeg put frame failed!\n");
    }

    return;
}

STATIC td_void decoder_mjpeg_release_resource(mjpeg_chan *chan)
{
    mjpeg_hal *hal = &chan->hal;

    if (chan->data.have_stream == TD_TRUE) {
        decoder_mjpeg_put_stream(chan, &chan->data.stream);
        if (hal->jpeg_handle != TD_NULL) {
            vdec_jpeg_hal_exit(hal->jpeg_handle);
            hal->jpeg_handle = TD_NULL;
        }

        chan->data.have_stream = TD_FALSE;
    }

    if (chan->data.have_frame == TD_TRUE) {
        decoder_mjpeg_put_frame(chan, &chan->data.frame);
        chan->data.have_frame = TD_FALSE;
    }

    return;
}

STATIC td_void decoder_mjpeg_flush_resource(mjpeg_chan *chan, const ext_vdec_opt_flush *option)
{
    mjpeg_hal *hal = &chan->hal;
    td_bool keep_stm, keep_frm;

    keep_stm = (option->type == EXT_VDEC_FLUSH_IN  || option->type == EXT_VDEC_FLUSH_ALL) ? TD_FALSE : TD_TRUE;
    keep_frm = (option->type == EXT_VDEC_FLUSH_OUT || option->type == EXT_VDEC_FLUSH_ALL) ? TD_FALSE : TD_TRUE;

    if (keep_stm == TD_FALSE) {
        if (chan->data.have_stream == TD_TRUE) {
            decoder_mjpeg_put_stream(chan, &chan->data.stream);
            if (hal->jpeg_handle != TD_NULL) {
                vdec_jpeg_hal_exit(hal->jpeg_handle);
                hal->jpeg_handle = TD_NULL;
            }
            chan->data.have_stream = TD_FALSE;
        }
    }

    // for mjpeg frame associates to stream, if release stream, release frame as well
    if (keep_stm == TD_FALSE || keep_frm == TD_FALSE) {
        if (chan->data.have_frame == TD_TRUE) {
            decoder_mjpeg_put_frame(chan, &chan->data.frame);
            chan->data.have_frame = TD_FALSE;
        }
    }
}

STATIC td_bool decoder_mjpeg_process(mjpeg_chan *chan)
{
    td_s32 ret;
    mjpeg_hal *hal = &chan->hal;
    mjpeg_data *data = &chan->data;

    if (data->have_stream == TD_FALSE) {
        ret = decoder_mjpeg_get_stream(chan, &data->stream);
        if (ret != EXT_SUCCESS) {
            return TD_FALSE;
        }

        hal->jpeg_handle = vdec_jpeg_hal_init(chan->id, &data->stream, &hal->jpeg_info);
        if (hal->jpeg_handle == TD_NULL) {
            vdec_print_err("mjpeg chan %d init jpeg component failed.\n", chan->id);
            (td_void)decoder_mjpeg_put_stream(chan, &data->stream);
            return TD_TRUE;
        }
        data->have_stream = TD_TRUE;
        decoder_mjpeg_init_frame(&hal->jpeg_info, &data->stream, &data->frame);
    }

    if (data->have_stream == TD_TRUE && data->have_frame == TD_FALSE) {
        ret = decoder_mjpeg_get_frame(chan, &data->frame);
        if (ret != EXT_SUCCESS) {
            return TD_FALSE;
        }
        data->have_frame = TD_TRUE;
    }

    ret = vdec_jpeg_hal_decode(hal->jpeg_handle, &hal->jpeg_info, &data->frame);
    if (ret != EXT_SUCCESS) {
        chan->statis.err_frm_num++;
        data->frame.is_frame_valid = TD_FALSE;
        vdec_print_err("mjpeg chan %d decode frame failed.\n", chan->id);
    } else {
        chan->statis.suc_frm_num++;
        data->frame.is_frame_valid = TD_TRUE;
        vdec_print_dbg("mjpeg chan %d decode frame(pts %lld) success: suc frm num %d.\n",
            chan->id, data->frame.pts, chan->statis.suc_frm_num);
    }
    vdec_jpeg_hal_exit(hal->jpeg_handle);
    hal->jpeg_handle = TD_NULL;

    decoder_mjpeg_release_resource(chan);

    return TD_TRUE;
}

STATIC td_void decoder_mjpeg_wait_stop(mjpeg_chan *chan)
{
    vdec_mutex_lock(chan->mutex);
    while (chan->data.start == TD_FALSE && chan->data.exit != TD_TRUE && chan->use == TD_TRUE) {
        vdec_cond_wait(chan->cond, chan->mutex);
    }
    vdec_mutex_unlock(chan->mutex);
}

STATIC td_void* decoder_mjpeg_thread(td_void *args)
{
    td_s32 ret;
    td_bool need_retry;
    mjpeg_chan *chan = TD_NULL;
    td_s32 chan_id = *((td_s32 *)args);

    ret = pthread_setname_np(pthread_self(), "vdec_mjp_thread");
    if (ret != 0) {
        vdec_print_err("%s, %d Error (pthread_setname_np): %s\n", __func__, __LINE__, strerror(ret));
    }

    if (chan_id >= VDEC_MAX_CHAN_NUM) {
        vdec_print_err("mjpeg thread chan id %d invalid.\n", chan_id);
        return TD_NULL;
    }

    vdec_print_info("mjpeg chan %d thread enter.\n", chan_id);
    chan = decoder_mjpeg_get_chan(chan_id);

    do {
        need_retry = TD_FALSE;

        vdec_mutex_lock(chan->mutex);
        if (chan->data.exit == TD_TRUE || chan->use == TD_FALSE) {
            decoder_mjpeg_release_resource(chan);
            vdec_mutex_unlock(chan->mutex);
            break;
        }

        vdec_print_dbg("mjpeg chan %d thread running, start %d.\n", chan_id, chan->data.start);

        if (chan->data.start == TD_TRUE) {
            need_retry = decoder_mjpeg_process(chan);
        }

        vdec_mutex_unlock(chan->mutex);

        decoder_mjpeg_wait_stop(chan);

        if (need_retry == TD_FALSE) {
            vdec_msleep(10);  // 10: sleep time in ms
        }
    } while (TD_TRUE);

    vdec_print_info("mjpeg chan %d thread exit.\n", chan_id);

    return TD_NULL;
}

INTF STATIC td_s32 decoder_mjpeg_create(td_s32 chan_id)
{
    td_s32 ret;
    mjpeg_chan *chan = TD_NULL;
    pthread_attr_t attr = { 0 };

    if (g_mjpeg_chan_ptr[chan_id] != TD_NULL) {
        vdec_print_alert("mjpeg chan %d has been used!\n", chan_id);
        return EXT_FAILURE;
    }

    chan = (mjpeg_chan *)osal_vmalloc(sizeof(mjpeg_chan));
    if (chan == TD_NULL) {
        vdec_print_err("mjpeg chan %d alloc failed!\n", chan_id);
        return EXT_FAILURE;
    }
    (td_void)memset_s(chan, sizeof(mjpeg_chan), 0, sizeof(mjpeg_chan));

    chan->id = chan_id;
    chan->use = TD_FALSE;
    vdec_mutex_init(chan->mutex);
    vdec_cond_init(chan->cond);

    g_mjpeg_chan_ptr[chan_id] = chan;

    decoder_mjpeg_clear_chan(chan);

    vdec_mutex_lock(chan->mutex);

    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 8 * 1024); /* 8 * 1024 stack */
    ret = vdec_thread_create(&chan->thread, &attr, decoder_mjpeg_thread, &chan->id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("mjpeg chan %d create thread failed!\n", chan_id);
        vdec_mutex_unlock(chan->mutex);
        vdec_mutex_destroy(chan->mutex);
        vdec_cond_destroy(chan->cond);
        osal_vfree(chan);
        g_mjpeg_chan_ptr[chan_id] = TD_NULL;
        return EXT_FAILURE;
    }

    chan->use = TD_TRUE;

    vdec_mutex_unlock(chan->mutex);

    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_CREATE_MJPEG, MT_VDEC_INFO_3_DEFAULT);

    vdec_print_info("mjpeg create chan %d.\n", chan_id);

    return EXT_SUCCESS;
}

INTF STATIC td_s32 decoder_mjpeg_destroy(td_s32 chan_id)
{
    td_s32 ret;
    mjpeg_chan *chan = g_mjpeg_chan_ptr[chan_id];

    if (chan == TD_NULL || chan->use == TD_FALSE) {
        vdec_print_alert("mjpeg chan %d has not been create!\n", chan_id);
        return EXT_SUCCESS;
    }

    vdec_mutex_lock(chan->mutex);

    chan->data.exit = TD_TRUE;
    vdec_cond_broadcast(chan->cond);
    vdec_mutex_unlock(chan->mutex);

    ret = vdec_thread_join(chan->thread, TD_NULL);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("mjpeg chan %d thread join failed!\n", chan_id);
    }

    vdec_mutex_lock(chan->mutex);

    decoder_mjpeg_clear_chan(chan);
    chan->use = TD_FALSE;

    vdec_mutex_unlock(chan->mutex);

    vdec_mutex_destroy(chan->mutex);
    vdec_cond_destroy(chan->cond);

    osal_vfree(chan);
    g_mjpeg_chan_ptr[chan_id] = TD_NULL;

    vdec_print_info("mjpeg destroy chan %d.\n", chan_id);

    return EXT_SUCCESS;
}

INTF STATIC td_s32 decoder_mjpeg_start(td_s32 chan_id)
{
    td_s32 ret;
    mjpeg_chan *chan = decoder_mjpeg_get_chan(chan_id);

    vdec_mutex_lock(chan->mutex);

    ret = vdec_hal_start(chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("mjpeg chan %d start failed!\n", chan_id);
        vdec_mutex_unlock(chan->mutex);
        return EXT_FAILURE;
    }

    chan->data.start = TD_TRUE;
    vdec_cond_broadcast(chan->cond);

    vdec_record_chr_event(chan_id, MT_VDEC_START, MT_VDEC_MJPEG_START, MT_VDEC_INFO_3_DEFAULT);

    vdec_mutex_unlock(chan->mutex);

    vdec_print_info("mjpeg start chan %d.\n", chan_id);

    return EXT_SUCCESS;
}

INTF STATIC td_s32 decoder_mjpeg_stop(td_s32 chan_id)
{
    td_s32 ret;
    td_bool err_flag = TD_FALSE;
    mjpeg_chan *chan = decoder_mjpeg_get_chan(chan_id);

    vdec_mutex_lock(chan->mutex);

    chan->data.start = TD_FALSE;

    ret = vdec_hal_stop(chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("mjpeg chan %d stop failed!\n", chan_id);
        err_flag = TD_TRUE;
    }

    vdec_mutex_unlock(chan->mutex);
    vdec_record_chr_event(chan_id, MT_VDEC_STOP, MT_VDEC_MJPEG_STOP, MT_VDEC_INFO_3_DEFAULT);

    vdec_print_info("mjpeg stop chan %d.\n", chan_id);

    return (err_flag == TD_FALSE) ? EXT_SUCCESS : EXT_FAILURE;
}

INTF STATIC td_s32 decoder_mjpeg_reset(td_s32 chan_id)
{
    td_s32 ret;
    mjpeg_chan *chan = decoder_mjpeg_get_chan(chan_id);

    vdec_mutex_lock(chan->mutex);

    decoder_mjpeg_release_resource(chan);

    ret = vdec_hal_reset(chan_id);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("mjpeg chan %d reset failed!\n", chan_id);
    }

    vdec_mutex_unlock(chan->mutex);
    vdec_record_chr_event(chan_id, MT_VDEC_RESET, MT_VDEC_MJPEG_RESET, MT_VDEC_INFO_3_DEFAULT);

    vdec_print_info("mjpeg reset chan %d.\n", chan_id);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_flush(td_s32 chan_id, const ext_vdec_opt_flush *option)
{
    td_s32 ret;
    mjpeg_chan *chan = decoder_mjpeg_get_chan(chan_id);

    vdec_mutex_lock(chan->mutex);

    decoder_mjpeg_flush_resource(chan, option);

    ret = vdec_hal_flush(chan_id, option);
    if (ret != EXT_SUCCESS) {
        vdec_print_alert("mjpeg chan %d flush failed!\n", chan_id);
    }

    vdec_mutex_unlock(chan->mutex);
    vdec_record_chr_event(chan_id, MT_VDEC_FLUSH, MT_VDEC_MJPEG_FLUSH, MT_VDEC_INFO_3_DEFAULT);

    vdec_print_info("mjpeg flush chan %d.\n", chan_id);

    return ret;
}

STATIC td_s32 decoder_mjpeg_send_eos(td_s32 chan_id)
{
    mjpeg_chan *chan = decoder_mjpeg_get_chan(chan_id);

    chan->data.set_eos = TD_TRUE;

    return vdec_hal_send_eos(chan_id);
}

INTF STATIC td_s32 decoder_mjpeg_decode(td_s32 chan_id, const ext_vdec_opt_decode *option,
    const ext_vdec_stream *stream, ext_vdec_frame *frame)
{
    td_s32 ret;
    ext_vdec_opt_get_stream dec_opt = {0};
    ext_vdec_opt_acq_frame acq_opt = {0};
    ext_vdec_stream stream_info = {0};
    td_u32 start_time = 0;
    td_u32 end_time;

    mjpeg_chan *chan = decoder_mjpeg_get_chan(chan_id);

    vdec_check_ret(option != TD_NULL && stream != TD_NULL && frame != TD_NULL && chan != TD_NULL);
    vdec_check_ret(stream->vir_addr != TD_NULL);

    ret = decoder_mjpeg_start(chan_id);
    vdec_check_ret(ret == EXT_SUCCESS);
    vdec_record_chr_event(chan_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_MJPEG_I_DECODE, MT_VDEC_MJPEG_I_START_OK);

    dec_opt.expect_size = stream->length;
    ret = decoder_mjpeg_dequeue_stream(chan_id, &dec_opt, &stream_info);
    vdec_check_goto(ret == EXT_SUCCESS, exit2);
    vdec_record_chr_event(chan_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_MJPEG_I_DECODE, MT_VDEC_MJPEG_I_DEQUEUE_OK);

    stream_info.vir_addr = (td_u8 *)vdec_mmap(stream_info.start_addr, stream_info.length);
    vdec_check_goto(stream_info.vir_addr != TD_NULL, exit2);

    (td_void)memcpy_s(stream_info.vir_addr, stream_info.length, stream->vir_addr, stream->length);

    stream_info.is_end_of_frame = TD_TRUE;
    ret = decoder_mjpeg_queue_stream(chan_id, &stream_info);
    vdec_check_goto(ret == EXT_SUCCESS, exit1);
    vdec_record_chr_event(chan_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_MJPEG_I_DECODE, MT_VDEC_MJPEG_I_PUT_STM_OK);

    ret = vdec_hal_send_eos(chan_id);
    vdec_check_goto(ret == EXT_SUCCESS, exit1);
    vdec_record_chr_event(chan_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_MJPEG_I_DECODE, MT_VDEC_MJPEG_I_SEND_EOS_OK);

    vdec_get_time(&start_time);
    end_time = start_time;
    ret = decoder_mjpeg_acquire_frame(chan_id, &acq_opt, frame);
    while ((ret != EXT_SUCCESS) && (end_time >= start_time) && (end_time - start_time <= (td_u32)option->time_out)) {
        vdec_msleep(2); // 2: ms
        ret = decoder_mjpeg_acquire_frame(chan_id, &acq_opt, frame);
        vdec_get_time(&end_time);
    }
    if (ret == EXT_SUCCESS) {
        vdec_record_chr_event(chan_id, MT_VDEC_I_FRAME_DECODE, MT_VDEC_MJPEG_I_DECODE, MT_VDEC_MJPEG_I_RECIEVE_OK);
    }

exit1:
    ret = vdec_umap(stream_info.vir_addr, stream_info.length);

exit2:
    ret = decoder_mjpeg_stop(chan_id);
    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_get_attr(td_s32 chan_id, ext_vdec_attr *attr)
{
    td_s32 ret;

    ret = vdec_hal_get_attr(chan_id, attr);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_set_attr(td_s32 chan_id, td_handle vdec_handle, const ext_vdec_attr *attr)
{
    td_s32 ret;
    ext_vdec_attr new_attr;

    if (attr->standard != EXT_VDEC_STD_MJPEG) {
        vdec_print_alert("set attr standard %d invalid, should be mjpeg.\n", attr->standard);
        return EXT_FAILURE;
    }

    (td_void)memcpy_s(&new_attr, sizeof(new_attr), attr, sizeof(ext_vdec_attr));

    ret = vdec_hal_set_attr(chan_id, vdec_handle, &new_attr);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_register_event(td_s32 chan_id, td_u64 event_map)
{
    td_s32 ret;

    ret = vdec_hal_register_event(chan_id, event_map);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_listen_event(td_s32 chan_id, const ext_vdec_opt_listen *option, ext_vdec_event *event)
{
    td_s32 ret;

    ret = vdec_hal_listen_event(chan_id, option, event);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_queue_stream(td_s32 chan_id, const ext_vdec_stream *stream)
{
    td_s32 ret;

    ret = vdec_hal_put_stream(chan_id, stream);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_dequeue_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option,
    ext_vdec_stream *stream)
{
    td_s32 ret;

    ret = vdec_hal_get_stream(chan_id, option, stream);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option,
    ext_vdec_frame *frame)
{
    td_s32 ret;

    ret = vdec_hal_acquire_frame(chan_id, option, frame);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_release_frame(td_s32 chan_id, const ext_vdec_frame *frame)
{
    td_s32 ret;

    ret = vdec_hal_release_frame(chan_id, frame);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_get_param(td_s32 chan_id, ext_vdec_param *param)
{
    td_s32 ret;

    ret = vdec_hal_get_param(chan_id, param);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_set_param(td_s32 chan_id, const ext_vdec_param *param)
{
    td_s32 ret;

    ret = vdec_hal_set_param(chan_id, param);

    return ret;
}

INTF STATIC td_s32 decoder_mjpeg_get_feature(td_s32 chan_id, ext_vdec_feature *feature)
{
    return vdec_hal_get_feature(chan_id, feature);
}

INTF STATIC td_s32 decoder_mjpeg_get_status(td_s32 chan_id, ext_vdec_status *status)
{
    return vdec_hal_get_status(chan_id, status);
}

INTF STATIC td_s32 decoder_mjpeg_is_std_support(ext_vdec_opt_is_std_support *support)
{
    return vdec_hal_is_std_support(support);
}

INTF STATIC td_s32 decoder_mjpeg_get_cap(ext_vdec_capability *capability)
{
    return vdec_hal_get_cap(capability);
}

INTF STATIC td_s32 decoder_mjpeg_bind_frm_buffer(td_s32 chan_id, const ext_vdec_frame_buf *buffer)
{
    return vdec_hal_bind_frm_buffer(chan_id, buffer);
}

STATIC vdec_decoder g_decoder_mjpeg = {
    .name = "vdec_decoder_mjpeg",
    .intf.create = decoder_mjpeg_create,
    .intf.destroy = decoder_mjpeg_destroy,
    .intf.start = decoder_mjpeg_start,
    .intf.stop = decoder_mjpeg_stop,
    .intf.reset = decoder_mjpeg_reset,
    .intf.flush = decoder_mjpeg_flush,
    .intf.decode = decoder_mjpeg_decode,
    .intf.get_attr = decoder_mjpeg_get_attr,
    .intf.set_attr = decoder_mjpeg_set_attr,
    .intf.register_event = decoder_mjpeg_register_event,
    .intf.listen_event = decoder_mjpeg_listen_event,
    .intf.queue_stream = decoder_mjpeg_queue_stream,
    .intf.dequeue_stream = decoder_mjpeg_dequeue_stream,
    .intf.acq_frame = decoder_mjpeg_acquire_frame,
    .intf.rls_frame = decoder_mjpeg_release_frame,
    .intf.get_param = decoder_mjpeg_get_param,
    .intf.set_param = decoder_mjpeg_set_param,
    .intf.get_feature = decoder_mjpeg_get_feature,
    .intf.get_status = decoder_mjpeg_get_status,
    .intf.std_support = decoder_mjpeg_is_std_support,
    .intf.get_cap = decoder_mjpeg_get_cap,
    .intf.send_eos = decoder_mjpeg_send_eos,
    .intf.bind_frm_buf = decoder_mjpeg_bind_frm_buffer,
};

vdec_decoder* vdec_load_decoder_mjpeg(td_void)
{
    decoder_mjpeg_init();

    return &g_decoder_mjpeg;
}

