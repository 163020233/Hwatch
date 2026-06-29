/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: source for sea extend algorithm
 * Author: audio
 */

#include <stdio.h>
#include <stdlib.h>
#include "securec.h"
#include "component.h"
#include "plugin_errno.h"
#include "audio_alg.h"
#include "audio_sea.h"
#include "audio_sea_struct.h"
#include "audio_type.h"
#include "aai_alg0_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define AAI_LIB_CHANNELS         1
#define AAI_LIB_BIT_DEPTH        16
#define AAI_LIB_SAMPLE_RATE      16000
#define AAI_LIB_FRAME_PER_SECOND 50

/* defines the bit depth during audio sampling. */
typedef enum {
    BIT_DEPTH_NULL = 0, /* unknown */
    BIT_DEPTH_8 = 8,    /* 8-bit depth */
    BIT_DEPTH_16X2_LH = 15,  /* 16-bit depth */
    BIT_DEPTH_16 = 16,  /* 16-bit depth */
    BIT_DEPTH_18 = 18,  /* 18-bit depth */
    BIT_DEPTH_20 = 20,  /* 20-bit depth */
    BIT_DEPTH_24 = 24,  /* 24-bit depth */
    BIT_DEPTH_32 = 32,  /* 32-bit depth */
    BIT_DEPTH_64 = 64,  /* 64-bit depth */

    BIT_DEPTH_MAX
} aai_bit_depth;

typedef struct {
    td_void *handle;
    td_bool enable;
    sea_frame_buf frame_buf;
    audio_alg_component *entry;

    td_void *data_context;
    td_void *event_context;
    sea_data_output data_output;
    sea_event_report event_report;
} aai_lib_inst;

static inline td_u32 aai_min(td_u32 x, td_u32 y)
{
    return (x <= y) ? x : y;
}

static td_u32 calc_sample_size(td_u32 channels, td_u32 bit_depth)
{
    td_u32 depth_size;

    if (bit_depth > BIT_DEPTH_NULL && bit_depth <= BIT_DEPTH_8) {
        depth_size = sizeof(td_u8);
    } else if (bit_depth > BIT_DEPTH_8 && bit_depth <= BIT_DEPTH_16) {
        depth_size = sizeof(td_u16);
    } else if (bit_depth > BIT_DEPTH_16 && bit_depth <= BIT_DEPTH_32) {
        depth_size = sizeof(td_u32);
    } else {
        return 0;
    }

    return (td_u32)(depth_size * channels);
}


/* how many samples per frame */
static td_u32 calc_samples_per_frame(td_u32 frame_size, td_u32 channels, td_u32 bit_depth)
{
    td_u32 sample_size = calc_sample_size(channels, bit_depth);
    if (sample_size == 0) {
        return 0;
    }

    return (frame_size / sample_size);
}

static td_void write_frame_buf(sea_frame_buf *frame_buf, const td_void *pcm_buf, td_u32 pcm_size, td_bool *is_full)
{
    td_void *dst_buf = TD_NULL;
    td_u32 copy_size = 0;
    td_u32 free_size = 0;

    if (frame_buf->used >= frame_buf->size) {
        frame_buf->used = 0;
    }

    free_size = frame_buf->size - frame_buf->used;
    copy_size = aai_min(pcm_size, free_size);
    dst_buf = frame_buf->addr + frame_buf->used;
    (td_void)memcpy_s(dst_buf, copy_size, pcm_buf, copy_size);
    frame_buf->used += copy_size;

    *is_full = (frame_buf->used >= frame_buf->size ? TD_TRUE : TD_FALSE);
}

static td_s32 aai_lib_get_eng_attr(sea_handle handle, td_u8 type, td_void *attr)
{
    sea_eng_attr *eng_attr = (sea_eng_attr *)attr;

    if (type != SEA_AAI_KWS) {
        return ERR_SEA_NOT_SUPPORTED;
    }

    eng_attr->type = SEA_AAI_KWS;
    eng_attr->mode = SEA_NORM_MODE;

    audio_unused(handle);
    return AUDIO_SUCCESS;
}

static td_s32 aai_lib_set_eng_attr(sea_handle handle, td_u8 type, const td_void *attr)
{
    if (type != SEA_AAI_KWS) {
        return ERR_SEA_NOT_SUPPORTED;
    }

    audio_unused(handle);
    audio_unused(attr);
    return AUDIO_SUCCESS;
}

static td_s32 aai_lib_create(sea_handle *handle, const sea_eng_sel *eng_sel, const sea_pcm_attr *attr)
{
    td_s32 ret;
    td_u32 api_size = 0;
    td_u32 buf_size = 0;
    aai_lib_inst *lib_inst = TD_NULL;
    audio_alg_component *lib_entry = TD_NULL;

    lib_entry = aai_alg0_report_entry();
    lib_entry->get_config(TD_NULL, ALG_COMMON_CMD_GET_API_SIZE, (td_void *)&api_size);
    lib_inst = (aai_lib_inst *)calloc(1, (sizeof(aai_lib_inst) + api_size + buf_size));
    if (lib_inst == TD_NULL) {
        return PLG_ERR_MEM_ALLOC;
    }
    lib_inst->handle = (td_void *)lib_inst + sizeof(aai_lib_inst);
    lib_inst->frame_buf.addr = lib_inst->handle + api_size;
    lib_inst->frame_buf.size = buf_size;
    lib_inst->frame_buf.samples = calc_samples_per_frame(buf_size, attr->in_pcm.channels, attr->in_pcm.bit_depth);

    (td_void)lib_entry->set_config(lib_inst->handle, ALG_COMMON_CMD_API_INIT, TD_NULL);
    ret = lib_entry->create(lib_inst->handle, attr);
    if (ret != PLUGIN_SUCCESS) {
        free((td_void *)lib_inst);
        return ret;
    }

    lib_inst->entry = lib_entry;
    *handle = (sea_handle)lib_inst;
    audio_unused(eng_sel);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_lib_destroy(sea_handle handle)
{
    aai_lib_inst *lib_inst = (aai_lib_inst *)handle;

    if (lib_inst->handle != TD_NULL) {
        lib_inst->entry->destroy(lib_inst->handle);
        lib_inst->handle = TD_NULL;
    }

    free((td_void *)lib_inst);
    return PLUGIN_SUCCESS;
}

static td_void aai_lib_wakeup_proc(sea_handle handle, td_u32 cmd_id, td_u32 score)
{
    sea_event_param kws_ret;
    aai_lib_inst *lib_inst = (aai_lib_inst *)handle;

    kws_ret.kws.match_score = score;
    kws_ret.kws.start_offset = 0;
    kws_ret.kws.end_offset = 0;
    kws_ret.kws.target_angle = 0;
    kws_ret.kws.phrase_id = cmd_id;
    if (lib_inst->event_report != TD_NULL) {
        lib_inst->event_report(handle, SEA_EVENT_KWS_MATCH, (td_void *)&kws_ret, lib_inst->event_context);
    }
}

static td_s32 aai_lib_proc_frame(sea_handle handle, sea_pcm_frame *frame)
{
    td_s32 ret;
    td_bool is_full = TD_FALSE;
    audio_kws_out out_frm;
    aai_lib_inst *lib_inst = (aai_lib_inst *)handle;

    if (lib_inst->enable == TD_FALSE) {
        return PLG_ERR_INST_STOP;
    }

    if (frame->size > lib_inst->frame_buf.size) {
    }

    write_frame_buf(&lib_inst->frame_buf, (td_void *)frame->data, frame->size, &is_full);
    if (is_full == TD_FALSE) {
        return PLUGIN_SUCCESS;
    }

    ret = lib_inst->entry->process(lib_inst->handle, (td_void *)frame, &out_frm);
    if (ret != PLUGIN_SUCCESS) {
        return ret;
    }

    if (out_frm.iswakeup == TD_TRUE) {
        aai_lib_wakeup_proc(handle, out_frm.index, out_frm.score[out_frm.index]);
    }

    return PLUGIN_SUCCESS;
}

static td_s32 aai_lib_set_enable(sea_handle handle, td_bool enable)
{
    aai_lib_inst *lib_inst = (aai_lib_inst *)handle;

    if (enable == TD_FALSE) {
        lib_inst->entry->reset(lib_inst->handle);
    }

    lib_inst->enable = enable;
    return PLUGIN_SUCCESS;
}

static td_s32 aai_lib_set_scr_buf(sea_handle handle, td_void *scr_buf, td_u32 scr_size)
{
    td_u32 size = 0;
    aai_lib_inst *lib_inst = (aai_lib_inst *)handle;

    (td_void)lib_inst->entry->get_config(lib_inst->handle, AAI_ALG_CMD_GET_SCR_BUF_SIZE, (td_void *)&size);
    if ((scr_buf == TD_NULL) || (scr_size < size)) {
        return PLG_ERR_BUF_OVERFLOW;
    }

    (td_void)lib_inst->entry->set_config(lib_inst->handle, AAI_ALG_CMD_SET_SCR_BUF, scr_buf);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_lib_get_eng_caps(sea_eng_sel *eng_sel)
{
    eng_sel->eng_ind[SEA_ECH_AAI_IND].type = SEA_AAI_KWS;
    eng_sel->eng_ind[SEA_ECH_AAI_IND].lib_id = SEA_LIB_ID_KWS;
    return AUDIO_SUCCESS;
}

static td_s32 aai_lib_get_def_pcm_attr(sea_pcm_attr *attr)
{
    attr->in_pcm.channels = AAI_LIB_CHANNELS;
    attr->in_pcm.bit_depth = AAI_LIB_BIT_DEPTH;
    attr->in_pcm.sample_rate = AAI_LIB_SAMPLE_RATE;
    attr->in_pcm.samples_per_frame = (attr->in_pcm.sample_rate / AAI_LIB_FRAME_PER_SECOND);
    return AUDIO_SUCCESS;
}

static td_s32 aai_lib_get_item_sets(sea_handle handle, sea_item_type type, td_void *sets)
{
    sea_item_sets *item_sets = (sea_item_sets *)sets;

    item_sets->total_count = KEYWORDNUM;
    item_sets->output_count = KEYWORDNUM;

    audio_unused(handle);
    audio_unused(type);
    return PLUGIN_SUCCESS;
}

static td_s32 aai_lib_reg_data_output(sea_handle handle, sea_data_output data_output, td_void *context)
{
    aai_lib_inst *lib_inst = (aai_lib_inst *)handle;

    lib_inst->data_context = context;
    lib_inst->data_output = data_output;
    return PLUGIN_SUCCESS;
}

static td_s32 aai_lib_reg_event_report(sea_handle handle, sea_event_report event_report, td_void *context)
{
    aai_lib_inst *lib_inst = (aai_lib_inst *)handle;

    lib_inst->event_context = context;
    lib_inst->event_report = event_report;
    return PLUGIN_SUCCESS;
}

static sea_entry ha_aai_lib0_entry = {
    .name = (const td_char *)"aai_lib1",
    .sea_id = SEA_LIB_ID_KWS,
    .version.u32 = 0x10000001,
    .description = (const td_char *)"Sea lib1 developed by XXX",
    .create = aai_lib_create,
    .destroy = aai_lib_destroy,
    .proc_frame = aai_lib_proc_frame,
    .set_enable = aai_lib_set_enable,
    .set_scr_buf = aai_lib_set_scr_buf,
    .get_eng_caps = aai_lib_get_eng_caps,
    .get_def_pcm_attr = aai_lib_get_def_pcm_attr,
    .get_item_sets = aai_lib_get_item_sets,
    .get_eng_attr = aai_lib_get_eng_attr,
    .set_eng_attr = aai_lib_set_eng_attr,
    .get_parameter = TD_NULL,
    .set_parameter = TD_NULL,
    .set_input_attr = TD_NULL,
    .set_output_attr = TD_NULL,
    .reg_data_output = aai_lib_reg_data_output,
    .reg_event_report = aai_lib_reg_event_report
};

define_component(AAI, ha_aai_lib0_entry);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

