/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "vdec_decoder_hw.h"
#include "mpi_vdec_sdk.h"
#include "vdec_hal.h"

INTF STATIC td_s32 decoder_hw_create(td_s32 chan_id)
{
    vdec_print_info("Create vdec hardware decoder chan %d. param %p, size %d\n", chan_id, param, size);
    vdec_mpi_unused(chan_id);

    return EXT_SUCCESS;
}

INTF STATIC td_s32 decoder_hw_destroy(td_s32 chan_id)
{
    vdec_print_info("Destroy vdec hardware decoder chan %d.\n", chan_id);
    vdec_mpi_unused(chan_id);
    return EXT_SUCCESS;
}

INTF STATIC td_s32 decoder_hw_start(td_s32 chan_id)
{
    return vdec_hal_start(chan_id);
}

INTF STATIC td_s32 decoder_hw_stop(td_s32 chan_id)
{
    return vdec_hal_stop(chan_id);
}

INTF STATIC td_s32 decoder_hw_reset(td_s32 chan_id)
{
    return vdec_hal_reset(chan_id);
}

INTF STATIC td_s32 decoder_hw_flush(td_s32 chan_id, const ext_vdec_opt_flush *option)
{
    return vdec_hal_flush(chan_id, option);
}

INTF STATIC td_s32 decoder_hw_decode(td_s32 chan_id, const ext_vdec_opt_decode *option,
    const ext_vdec_stream *stream, ext_vdec_frame *frame)
{
    return vdec_hal_decode(chan_id, option, stream, frame);
}

INTF STATIC td_s32 decoder_hw_get_attr(td_s32 chan_id, ext_vdec_attr *attr)
{
    return vdec_hal_get_attr(chan_id, attr);
}

INTF STATIC td_s32 decoder_hw_set_attr(td_s32 chan_id, td_handle vdec_handle, const ext_vdec_attr *attr)
{
    return vdec_hal_set_attr(chan_id, vdec_handle, attr);
}

INTF STATIC td_s32 decoder_hw_register_event(td_s32 chan_id, td_u64 event_map)
{
    return vdec_hal_register_event(chan_id, event_map);
}

INTF STATIC td_s32 decoder_hw_listen_event(td_s32 chan_id, const ext_vdec_opt_listen *option, ext_vdec_event *event)
{
    return vdec_hal_listen_event(chan_id, option, event);
}

INTF STATIC td_s32 decoder_hw_queue_stream(td_s32 chan_id, const ext_vdec_stream *stream)
{
    return vdec_hal_put_stream(chan_id, stream);
}

INTF STATIC td_s32 decoder_hw_dequeue_stream(td_s32 chan_id, const ext_vdec_opt_get_stream *option,
    ext_vdec_stream *stream)
{
    return vdec_hal_get_stream(chan_id, option, stream);
}

INTF STATIC td_s32 decoder_hw_acquire_frame(td_s32 chan_id, const ext_vdec_opt_acq_frame *option, ext_vdec_frame *frame)
{
    return vdec_hal_acquire_frame(chan_id, option, frame);
}

INTF STATIC td_s32 decoder_hw_release_frame(td_s32 chan_id, const ext_vdec_frame *frame)
{
    return vdec_hal_release_frame(chan_id, frame);
}

INTF STATIC td_s32 decoder_hw_get_param(td_s32 chan_id, ext_vdec_param *param)
{
    return vdec_hal_get_param(chan_id, param);
}

INTF STATIC td_s32 decoder_hw_set_param(td_s32 chan_id, const ext_vdec_param *param)
{
    return vdec_hal_set_param(chan_id, param);
}

INTF STATIC td_s32 decoder_hw_get_feature(td_s32 chan_id, ext_vdec_feature *feature)
{
    return vdec_hal_get_feature(chan_id, feature);
}

INTF STATIC td_s32 decoder_hw_get_status(td_s32 chan_id, ext_vdec_status *status)
{
    return vdec_hal_get_status(chan_id, status);
}

INTF STATIC td_s32 decoder_hw_is_std_support(ext_vdec_opt_is_std_support *support)
{
    return vdec_hal_is_std_support(support);
}

INTF STATIC td_s32 decoder_hw_get_cap(ext_vdec_capability *capability)
{
    return vdec_hal_get_cap(capability);
}

INTF STATIC td_s32 decoder_hw_send_eos(td_s32 chan_id)
{
    return vdec_hal_send_eos(chan_id);
}

INTF STATIC td_s32 decoder_hw_bind_frm_buffer(td_s32 chan_id, const ext_vdec_frame_buf *buffer)
{
    return vdec_hal_bind_frm_buffer(chan_id, buffer);
}

STATIC vdec_decoder g_decoder_hardware = {
    .name = "vdec_decoder_hardware",
    .intf.create = decoder_hw_create,
    .intf.destroy = decoder_hw_destroy,
    .intf.start = decoder_hw_start,
    .intf.stop = decoder_hw_stop,
    .intf.reset = decoder_hw_reset,
    .intf.flush = decoder_hw_flush,
    .intf.decode = decoder_hw_decode,
    .intf.get_attr = decoder_hw_get_attr,
    .intf.set_attr = decoder_hw_set_attr,
    .intf.register_event = decoder_hw_register_event,
    .intf.listen_event = decoder_hw_listen_event,
    .intf.queue_stream = decoder_hw_queue_stream,
    .intf.dequeue_stream = decoder_hw_dequeue_stream,
    .intf.acq_frame = decoder_hw_acquire_frame,
    .intf.rls_frame = decoder_hw_release_frame,
    .intf.get_param = decoder_hw_get_param,
    .intf.set_param = decoder_hw_set_param,
    .intf.get_feature = decoder_hw_get_feature,
    .intf.get_status = decoder_hw_get_status,
    .intf.std_support = decoder_hw_is_std_support,
    .intf.get_cap = decoder_hw_get_cap,
    .intf.send_eos = decoder_hw_send_eos,
    .intf.bind_frm_buf = decoder_hw_bind_frm_buffer,
};

vdec_decoder* vdec_load_decoder_hardware(td_void)
{
    return &g_decoder_hardware;
}

