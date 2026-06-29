/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2024-10-11
 */

#include "securec.h"
#include "mpi_vdec_sdk.h"
#include "mpi_vdec_osal.h"
#include "vdec_store.h"
#include "soc_vdec_api.h"

/*-------------------------------------- struct info --------------------------------------*/

typedef struct {
    td_s32 id;
    td_bool use;
    ext_vdec_standard standard;
    vdec_mutex mutex;
    vdec_decoder *decoder;
    td_bool decoder_init;
} vdec_chan;

typedef struct {
    td_bool inited;
    vdec_mutex mutex;
    vdec_chan chan[VDEC_MAX_CHAN_NUM];
} vdec_entry;

/*-------------------------------------- global define --------------------------------------*/

static vdec_entry g_vdec_api_entry = {
    .inited = TD_FALSE,
    .mutex = VDEC_MUTEX_INITIALIZER,
};

/*-------------------------------------- static define --------------------------------------*/

static inline vdec_entry *vdec_get_entry(td_void)
{
    return &g_vdec_api_entry;
}

static inline vdec_chan* vdec_get_chan(td_s32 chan_id)
{
    vdec_entry *vdec = vdec_get_entry();

    return &vdec->chan[chan_id];
}

static td_void vdec_lock_entry(td_void)
{
    vdec_entry *vdec = vdec_get_entry();
    vdec_mutex_lock(vdec->mutex);
}

static td_void vdec_unlock_entry(td_void)
{
    vdec_entry *vdec = vdec_get_entry();
    vdec_mutex_unlock(vdec->mutex);
}

static td_void vdec_lock_chan(td_s32 chan_id)
{
    vdec_chan *chan = vdec_get_chan(chan_id);
    vdec_mutex_lock(chan->mutex);
}

static td_void vdec_unlock_chan(td_s32 chan_id)
{
    vdec_chan *chan = vdec_get_chan(chan_id);
    vdec_mutex_unlock(chan->mutex);
}

static inline td_s32 vdec_verify_handle(td_handle handle)
{
    td_s32 module_id = vdec_get_module_id_by_handle(handle);
    td_s32 private_data = vdec_get_private_data_by_handle(handle);
    td_s32 chan_id = vdec_get_chan_id_by_handle(handle);

    vdec_check_ret(module_id == SOC_ID_VDEC);
    vdec_check_ret(private_data == 0);
    vdec_check_ret(chan_id >= 0 && chan_id < VDEC_MAX_CHAN_NUM);

    return EXT_SUCCESS;
}

static td_void vdec_init(td_void)
{
    td_s32 i;
    vdec_chan *chan = TD_NULL;

    for (i = 0; i < VDEC_MAX_CHAN_NUM; i++) {
        chan = vdec_get_chan(i);
        chan->id = i;
        vdec_mutex_init(chan->mutex);
    }

    vdec_print_info("vdec version: %s\n", VDEC_VERSION);
}

static td_void vdec_get_default_attr(ext_vdec_attr *attr)
{
    vdec_check(memset_s(attr, sizeof(ext_vdec_attr), 0, sizeof(ext_vdec_attr)) == EOK);

    attr->standard = EXT_VDEC_STD_UNKNOWN;
    attr->decode_mode = EXT_VDEC_DEC_MODE_IPB;
    attr->frame_buf_mode = EXT_VDEC_FRM_INTERNAL;
    attr->output_order = EXT_VDEC_OUT_ORDER_DISPLAY;
    attr->frame_layout = EXT_VDEC_FRM_LAYOUT_AUTO;
    attr->max_stream_size = 1 * 1024 * 1024; /* 1M : 1 * 1024 * 1024 */
    attr->max_stream_num = VDEC_DFT_MAX_RAW_NUM;
    attr->extra_frame_num = 1;
    attr->error_threshold = 100; /* 100:all frames output */
    attr->max_dec_w_h.is_max_dec_w_h_en = TD_FALSE;
    attr->max_dec_w_h.max_dec_width = 0;
    attr->max_dec_w_h.max_dec_height = 0;
}

static td_s32 vdec_get_decoder(td_s32 chan_id, const ext_vdec_attr *attr)
{
    td_s32 ret;
    vdec_decoder *decoder = TD_NULL;
    vdec_chan *chan = vdec_get_chan(chan_id);

    ret = vdec_store_query_decoder(attr->standard, &decoder);
    vdec_check_ret(ret == EXT_SUCCESS && decoder != TD_NULL);

    if (chan->decoder == decoder) {
        vdec_print_info("decoder has been got!\n");
        chan->decoder_init = TD_TRUE;
        return EXT_SUCCESS;
    }

    if (chan->decoder != TD_NULL) {
        ret = chan->decoder->intf.destroy(chan_id);
        if (ret != EXT_SUCCESS) {
            vdec_print_err("decoder destroy fail!\n");
            return EXT_FAILURE;
        }
    }

    chan->decoder = decoder;
    chan->decoder_init = TD_TRUE;

    ret = chan->decoder->intf.create(chan_id);
    vdec_check_ret(ret == EXT_SUCCESS);

    return EXT_SUCCESS;
}

static td_bool vdec_is_entry_inited(td_void)
{
    td_bool inited;
    vdec_entry *vdec = vdec_get_entry();

    vdec_lock_entry();
    inited = vdec->inited;
    vdec_unlock_entry();

    return inited;
}

static td_bool vdec_is_chan_ready(const vdec_chan *chan)
{
    if (chan->use == TD_FALSE || chan->decoder == TD_NULL) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

static td_void vdec_clear_chan(vdec_chan *chan)
{
    chan->id = -1;
    chan->use = TD_FALSE;
    chan->decoder = TD_NULL;
    chan->standard = EXT_VDEC_STD_UNKNOWN;
    chan->decoder_init = TD_FALSE;
}

static td_s32 vdec_check_capability_input(const ext_vdec_capability *capability)
{
    vdec_check_ret(capability != TD_NULL);

    if (capability->standard <= EXT_VDEC_STD_UNKNOWN || capability->standard >= EXT_VDEC_STD_MAX) {
        vdec_print_err("invalid capability standard %d.\n", capability->standard);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_event_input(const ext_vdec_event *event)
{
    vdec_check_ret(event != TD_NULL);

    if (event->event_id <= EXT_VDEC_EVT_UNKNOWN) {
        vdec_print_err("invalid event_id %d.\n", event->event_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_event_option(const ext_vdec_opt_listen *option)
{
    vdec_check_ret(option != TD_NULL);

    if (option->time_out < -1) {
        vdec_print_err("invalid time_out %d.\n", option->time_out);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_flush_option(const ext_vdec_opt_flush *option)
{
    vdec_check_ret(option != TD_NULL);

    if (option->type < EXT_VDEC_FLUSH_IN || option->type >= EXT_VDEC_FLUSH_MAX) {
        vdec_print_err("invalid option type %d.\n", option->type);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_decode_option(const ext_vdec_opt_decode *option)
{
    vdec_check_ret(option != TD_NULL);

    if (option->time_out < -1) {
        vdec_print_err("invalid time_out %d.\n", option->time_out);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_stream_option(const ext_vdec_opt_get_stream *option)
{
    vdec_check_ret(option != TD_NULL);

    if (option->expect_size == 0) {
        vdec_print_err("invalid expect_size %u.\n", option->expect_size);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_frame_option(const ext_vdec_opt_acq_frame *option)
{
    vdec_check_ret(option != TD_NULL);

    if (option->time_out < -1) {
        vdec_print_err("invalid time_out %d.\n", option->time_out);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_stream(const ext_vdec_stream *stream)
{
    vdec_check_ret(stream != TD_NULL);

    if (stream->start_addr == 0) {
        vdec_print_err("start_addr = 0 invalid.\n");
        return EXT_FAILURE;
    }

    if (stream->vir_addr == TD_NULL) {
        vdec_print_err("vir_addr = NULL invalid.\n");
        return EXT_FAILURE;
    }

    if (stream->length == 0) {
        vdec_print_err("length = 0 invalid.\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_frame(const ext_vdec_frame *frame)
{
    vdec_check_ret(frame != TD_NULL);

    if (frame->frame_addr.start_addr == 0) {
        vdec_print_err("frame_addr.start_addr = 0 invalid.\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_attr(const ext_vdec_attr *attr)
{
    vdec_check_ret(attr != TD_NULL);

    if (attr->standard <= EXT_VDEC_STD_UNKNOWN || attr->standard >= EXT_VDEC_STD_MAX) {
        vdec_print_err("invalid attr standard %d.\n", attr->standard);
        return EXT_FAILURE;
    }

    if (attr->decode_mode < EXT_VDEC_DEC_MODE_IPB || attr->decode_mode >= EXT_VDEC_DEC_MODE_MAX) {
        vdec_print_err("invalid attr decode_mode %d.\n", attr->decode_mode);
        return EXT_FAILURE;
    }

    if (attr->output_order < EXT_VDEC_OUT_ORDER_DISPLAY || attr->output_order >= EXT_VDEC_OUT_ORDER_MAX) {
        vdec_print_err("invalid attr output_order %d.\n", attr->output_order);
        return EXT_FAILURE;
    }

    if (attr->frame_layout < EXT_VDEC_FRM_LAYOUT_AUTO || attr->frame_layout >= EXT_VDEC_FRM_LAYOUT_MAX) {
        vdec_print_err("invalid attr frame_layout %d.\n", attr->frame_layout);
        return EXT_FAILURE;
    }

    if (attr->frame_buf_mode < EXT_VDEC_FRM_INTERNAL || attr->frame_buf_mode >= EXT_VDEC_FRM_MAX) {
        vdec_print_err("invalid attr frame_buf_mode %d.\n", attr->frame_buf_mode);
        return EXT_FAILURE;
    }

    if (attr->max_stream_size == 0) {
        vdec_print_err("max_stream_size = 0 invalid.\n");
        return EXT_FAILURE;
    }

    if (attr->max_stream_num == 0) {
        vdec_print_err("max_stream_num = 0 invalid.\n");
        return EXT_FAILURE;
    }

    if (attr->extra_frame_num == 0) {
        vdec_print_err("extra_frame_num = 0 invalid.\n");
        return EXT_FAILURE;
    }

    if (attr->error_threshold > EXT_VDEC_MAX_ERR_THRESHOLD) {
        vdec_print_err("error_threshold %u, out of range[0,%d].\n", attr->error_threshold, EXT_VDEC_MAX_ERR_THRESHOLD);
        return EXT_FAILURE;
    }

    if (attr->max_dec_w_h.is_max_dec_w_h_en == TD_TRUE &&
        ((attr->max_dec_w_h.max_dec_width == 0 || attr->max_dec_w_h.max_dec_width > EXT_VDEC_MAX_DEC_WIDTH) ||
        (attr->max_dec_w_h.max_dec_height == 0 || attr->max_dec_w_h.max_dec_height > EXT_VDEC_MAX_DEC_HEIGHT))) {
        vdec_print_err("max_width %u range[0,%d] max_height %u range[0,%d]\n", attr->max_dec_w_h.max_dec_width,
            EXT_VDEC_MAX_DEC_WIDTH, attr->max_dec_w_h.max_dec_height, EXT_VDEC_MAX_DEC_HEIGHT);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_param(const ext_vdec_param *param)
{
    vdec_check_ret(param != TD_NULL);

    if (param->low_delay != TD_FALSE && param->low_delay != TD_TRUE) {
        vdec_print_err("invalid low_delay %u.\n", param->low_delay);
        return EXT_FAILURE;
    }

    if (param->frame_rate.type < EXT_VDEC_FR_TYPE_PTS || param->frame_rate.type >= EXT_VDEC_FR_TYPE_MAX) {
        vdec_print_err("invalid frame_rate.type %d.\n", param->frame_rate.type);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 vdec_check_bind_buffer(const ext_vdec_frame_buf *buffer)
{
    vdec_check_ret(buffer != TD_NULL);

    if (buffer->phy_addr == 0) {
        vdec_print_err("phy_addr = 0 invalid.\n");
        return EXT_FAILURE;
    }

    if (buffer->frame_size == 0) {
        vdec_print_err("frame_size = 0 invalid.\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

/*-------------------------------------- func release -------------------------------------*/

td_s32 uapi_vdec_get_capability(ext_vdec_capability *capability)
{
    vdec_check_ret(vdec_check_capability_input(capability) == EXT_SUCCESS);

    return vdec_store_get_cap(capability);
}

td_s32 uapi_vdec_is_standard_support(ext_vdec_standard standard, td_bool *is_support)
{
    td_s32 ret;
    vdec_decoder *decoder = TD_NULL;

    vdec_check_ret(is_support != TD_NULL);
    vdec_check_ret(standard > EXT_VDEC_STD_UNKNOWN && standard < EXT_VDEC_STD_MAX);

    *is_support = TD_FALSE;

    ret = vdec_store_query_decoder(standard, &decoder);
    vdec_check_ret(ret == EXT_SUCCESS && decoder != TD_NULL);

    *is_support = TD_TRUE;

    return EXT_SUCCESS;
}

td_s32 uapi_vdec_create(td_handle *handle)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    vdec_entry *vdec = vdec_get_entry();
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(handle != TD_NULL);

    begin_time = end_time = 0;
    vdec_get_time(&begin_time);

    vdec_lock_entry();

    if (vdec->inited == TD_FALSE) {
        vdec_init();
        vdec->inited = TD_TRUE;
    }

    ret = vdec_store_open(&chan_id);
    if (ret != EXT_SUCCESS || chan_id < 0 || chan_id >= VDEC_MAX_CHAN_NUM) {
        vdec_unlock_entry();
        vdec_print_alert("store open fail!\n");
        return EXT_FAILURE;
    }

    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_clear_chan(chan);

    vdec_store_get_default_decoder(&chan->decoder);

    chan->use = TD_TRUE;

    vdec_unlock_chan(chan_id);
    vdec_unlock_entry();

    *handle = vdec_make_handle((td_u32)chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("create chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
}

td_s32 uapi_vdec_destroy(td_handle handle)
{
    td_s32 ret = EXT_FAILURE;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    vdec_lock_entry();

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    if (chan->decoder != TD_NULL) {
        ret = chan->decoder->intf.destroy(chan_id);
        vdec_check_goto(ret == EXT_SUCCESS, exit);
    }

    ret = vdec_store_close(chan_id);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_clear_chan(chan);

    ret = EXT_SUCCESS;

exit:
    vdec_unlock_chan(chan_id);
    vdec_unlock_entry();

    vdec_get_time(&end_time);

    vdec_print_dbg("destroy chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return ret;
}

td_s32 uapi_vdec_start(td_handle handle)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.start(chan_id);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("start chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_stop(td_handle handle)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.stop(chan_id);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("stop chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_reset(td_handle handle)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.reset(chan_id);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("reset chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_flush(td_handle handle, const ext_vdec_opt_flush *option)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_flush_option(option) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.flush(chan_id, option);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("flush chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_decode(td_handle handle, const ext_vdec_stream *stream, ext_vdec_frame *frame,
    const ext_vdec_opt_decode *option)
{
    td_s32 ret;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;
    vdec_chan *chan = TD_NULL;

    vdec_check_ret(frame != TD_NULL);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_stream(stream) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_decode_option(option) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.decode(chan_id, option, stream, frame);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_get_attr(td_handle handle, ext_vdec_attr *attr)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(attr != TD_NULL);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(chan->use == TD_TRUE, exit);

    if (chan->decoder == TD_NULL || chan->decoder_init == TD_FALSE) {
        vdec_get_default_attr(attr);
    } else {
        ret = chan->decoder->intf.get_attr(chan_id, attr);
        vdec_check_goto(ret == EXT_SUCCESS, exit);
    }

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("get attr chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_set_attr(td_handle handle, const ext_vdec_attr *attr)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_attr(attr) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(chan->use == TD_TRUE, exit);

    ret = vdec_get_decoder(chan_id, attr);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    ret = chan->decoder->intf.set_attr(chan_id, handle, attr);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    chan->standard = attr->standard;

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("set attr chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_get_param(td_handle handle, ext_vdec_param *param)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(param != TD_NULL);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.get_param(chan_id, param);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("get param chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_set_param(td_handle handle, const ext_vdec_param *param)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(param != TD_NULL);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_param(param) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    if (param->low_delay == TD_TRUE) {
        vdec_print_err("low_delay is not support!\r\n");
        goto exit;
    }

    ret = chan->decoder->intf.set_param(chan_id, param);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("set param chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_get_feature(td_handle handle, ext_vdec_feature *feature)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(feature != TD_NULL);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.get_feature(chan_id, feature);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("get feature chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_get_status(td_handle handle, ext_vdec_status *status)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(status != TD_NULL);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.get_status(chan_id, status);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("get status chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_get_stream(td_handle handle, ext_vdec_stream *stream, const ext_vdec_opt_get_stream *option)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(stream != TD_NULL);
    vdec_check_ret(vdec_check_stream_option(option) == EXT_SUCCESS);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.dequeue_stream(chan_id, option, stream);
    if (ret != EXT_SUCCESS) {
        goto exit;
    }

    vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_put_stream(td_handle handle, const ext_vdec_stream *stream)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_stream(stream) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.queue_stream(chan_id, stream);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_acquire_frame(td_handle handle, ext_vdec_frame *frame, const ext_vdec_opt_acq_frame *option)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    fn_decoder_acquire_frame acq_frame = TD_NULL;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(frame != TD_NULL);
    vdec_check_ret(vdec_check_frame_option(option) == EXT_SUCCESS);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    if (option->time_out == 0) {
        ret = chan->decoder->intf.acq_frame(chan_id, option, frame);
        if (ret != EXT_SUCCESS) {
            goto exit;
        }
    } else {
        acq_frame = chan->decoder->intf.acq_frame;
    }

    vdec_unlock_chan(chan_id);

    if (acq_frame != TD_NULL) {
        ret = acq_frame(chan_id, option, frame);
        if (ret != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_release_frame(td_handle handle, const ext_vdec_frame *frame)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_check_frame(frame) == EXT_SUCCESS);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.rls_frame(chan_id, frame);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_register_event(td_handle handle, td_u64 event_map)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.register_event(chan_id, event_map);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_listen_event(td_handle handle, ext_vdec_event *event, const ext_vdec_opt_listen *option)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;
    fn_decoder_listen_event listen_event = TD_NULL;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_event_input(event) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_event_option(option) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    listen_event = chan->decoder->intf.listen_event;
    vdec_unlock_chan(chan_id);

    if (listen_event == TD_NULL) {
        vdec_print_err("chan %d listen_event is null!\n", chan_id);
        return EXT_FAILURE;
    }

    ret = listen_event(chan_id, option, event);
    if (ret != EXT_SUCCESS) {
        vdec_print_warn("chan %d listen event fail!\n", chan_id);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_send_eos(td_handle handle)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.send_eos(chan_id);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("send eos chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}

td_s32 uapi_vdec_bind_frame_buffer(td_handle handle, const ext_vdec_frame_buf *buffer)
{
    td_s32 ret;
    vdec_chan *chan = TD_NULL;
    td_u32 begin_time, end_time;
    td_s32 chan_id = EXT_VDEC_INVALID_HANDLE;

    vdec_check_ret(buffer != TD_NULL);
    vdec_check_ret(vdec_verify_handle(handle) == EXT_SUCCESS);
    vdec_check_ret(vdec_check_bind_buffer(buffer) == EXT_SUCCESS);
    vdec_check_ret(vdec_is_entry_inited() == TD_TRUE);

    vdec_get_time(&begin_time);

    chan_id = vdec_get_chan_id_by_handle(handle);
    vdec_lock_chan(chan_id);

    chan = vdec_get_chan(chan_id);
    vdec_check_goto(vdec_is_chan_ready(chan) == TD_TRUE, exit);

    ret = chan->decoder->intf.bind_frm_buf(chan_id, buffer);
    vdec_check_goto(ret == EXT_SUCCESS, exit);

    vdec_unlock_chan(chan_id);

    vdec_get_time(&end_time);

    vdec_print_dbg("bind frame buffer chan %d cost time %d ms.\n", chan_id, end_time - begin_time);

    return EXT_SUCCESS;
exit:
    vdec_unlock_chan(chan_id);

    return EXT_FAILURE;
}
