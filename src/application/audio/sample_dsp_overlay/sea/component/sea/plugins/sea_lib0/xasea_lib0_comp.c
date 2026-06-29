/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: source for sea extend algorithm
 * Author: audio
 */

#include <stdio.h>
#include <stdlib.h>
#include "component.h"
#include "plugin_errno.h"
#include "audio_alg.h"
#include "audio_sea.h"
#include "audio_sea_struct.h"
#include "audio_type.h"
#include "sea_alg0_api.h"
#include "log_overlay.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SEE_SAMPLES_PER_FRAME 240

typedef struct {
    td_s32 frame_buf[SEE_SAMPLES_PER_FRAME];
    td_void *handle;
    td_bool enable;
    audio_alg_component *entry;

    td_void *data_context;
    td_void *event_context;
    sea_data_output data_output;
    sea_event_report event_report;
} sea_lib_inst;

static td_s32 sea_lib_get_eng_caps(sea_eng_sel *eng_sel)
{
    eng_sel->eng_ind[SEA_ECH_AFE_IND].type = SEA_AFE_SEE;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_get_def_pcm_attr(sea_pcm_attr *attr)
{
    attr->buf_dur_ms = 0;
    attr->in_pcm.channels = AUDIO_CH_1;
    attr->in_pcm.bit_depth = BIT_DEPTH_32;
    attr->in_pcm.sample_rate = SAMPLE_RATE_48K;
    attr->in_pcm.samples_per_frame = SEE_SAMPLES_PER_FRAME;
    attr->ref_pcm.channels = AUDIO_CH_1;
    attr->ref_pcm.bit_depth = BIT_DEPTH_32;
    attr->ref_pcm.sample_rate = SAMPLE_RATE_48K;
    attr->out_pcm[SEA_OUTPUT_KWS_SRC].channels = AUDIO_CH_1;
    attr->out_pcm[SEA_OUTPUT_KWS_SRC].bit_depth = BIT_DEPTH_32;
    attr->out_pcm[SEA_OUTPUT_KWS_SRC].sample_rate = SAMPLE_RATE_48K;
    attr->out_pcm[SEA_OUTPUT_KWS_SRC].samples_per_frame = SEE_SAMPLES_PER_FRAME;
    attr->out_pcm[SEA_OUTPUT_ASR_SRC].channels = AUDIO_CH_1;
    attr->out_pcm[SEA_OUTPUT_ASR_SRC].bit_depth = BIT_DEPTH_32;
    attr->out_pcm[SEA_OUTPUT_ASR_SRC].sample_rate = SAMPLE_RATE_48K;
    attr->out_pcm[SEA_OUTPUT_ASR_SRC].samples_per_frame = SEE_SAMPLES_PER_FRAME;
    return AUDIO_SUCCESS;
}

static td_s32 sea_lib_get_eng_attr(sea_handle handle, td_u8 type, td_void *attr)
{
    sea_eng_attr *eng_attr = (sea_eng_attr *)attr;

    eng_attr->type = SEA_AFE_SEE;
    eng_attr->mode = SEA_NORM_MODE;

    audio_unused(handle);
    audio_unused(type);
    return AUDIO_SUCCESS;
}

static td_s32 sea_lib_set_eng_attr(sea_handle handle, td_u8 type, const td_void *attr)
{
    audio_unused(handle);
    audio_unused(type);
    audio_unused(attr);
    return AUDIO_SUCCESS;
}

static td_s32 sea_lib_get_parameter(sea_handle handle, td_u32 cmd_id, td_u8 *parameter)
{
    audio_log2(LOG_LEVEL_WARNING,
        "sea_lib_get_parameter enter, handle = %p, cmd_id = 0x%x",
        handle, cmd_id);

    audio_unused(parameter);
    return AUDIO_SUCCESS;
}

static td_s32 sea_lib_set_parameter(sea_handle handle, td_u32 cmd_id, const td_u8 *parameter)
{
    audio_log2(LOG_LEVEL_WARNING,
        "sea_lib_set_parameter enter, handle = %p, cmd_id = 0x%x",
        handle, cmd_id);

    audio_unused(parameter);
    return AUDIO_SUCCESS;
}

static td_s32 sea_lib_create(sea_handle *handle, const sea_eng_sel *eng_sel,
    const sea_pcm_attr *attr, const sea_cfg *cfg)
{
    td_s32 ret;
    td_u32 api_size = 0;
    sea_lib_inst *lib_inst = TD_NULL;
    audio_alg_component *lib_entry = TD_NULL;

    audio_log1(LOG_LEVEL_WARNING, "sea_lib_create enter, handle = %p", handle);

    lib_entry = sea_alg0_report_entry();
    lib_entry->get_config(TD_NULL, ALG_COMMON_CMD_GET_API_SIZE, (td_void *)&api_size);
    lib_inst = (sea_lib_inst *)calloc(1, (sizeof(sea_lib_inst) + api_size));
    if (lib_inst == TD_NULL) {
        return PLG_ERR_MEM_ALLOC;
    }
    lib_inst->handle = (td_void *)lib_inst + sizeof(sea_lib_inst);

    (td_void)lib_entry->set_config(lib_inst->handle, ALG_COMMON_CMD_API_INIT, TD_NULL);
    ret = lib_entry->create(lib_inst->handle, attr);
    if (ret != PLUGIN_SUCCESS) {
        free((td_void *)lib_inst);
        return ret;
    }

    lib_inst->entry = lib_entry;
    *handle = (sea_handle)lib_inst;
    audio_unused(eng_sel);
    audio_unused(cfg);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_destroy(sea_handle handle)
{
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    if (lib_inst->handle != TD_NULL) {
        lib_inst->entry->destroy(lib_inst->handle);
        lib_inst->handle = TD_NULL;
    }

    free((td_void *)lib_inst);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_proc_frame(sea_handle handle, sea_pcm_frame *frame)
{
    td_s32 ret;
    sea_pcm_frame out_frm = *frame;
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    if (lib_inst->enable == TD_FALSE) {
        return PLG_ERR_INST_STOP;
    }

    out_frm.data = lib_inst->frame_buf;

    ret = lib_inst->entry->process(lib_inst->handle, (td_void *)frame, &out_frm);
    if (ret != PLUGIN_SUCCESS) {
        return ret;
    }

    if (lib_inst->data_output != TD_NULL) {
        out_frm.type = SEA_OUTPUT_ASR_SRC;
        lib_inst->data_output(handle, &out_frm, lib_inst->data_context);
        out_frm.type = SEA_OUTPUT_KWS_SRC;
        lib_inst->data_output(handle, &out_frm, lib_inst->data_context);
        out_frm.type = SEA_OUTPUT_REC_SRC;
        lib_inst->data_output(handle, &out_frm, lib_inst->data_context);
    }

    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_set_enable(sea_handle handle, td_bool enable)
{
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    if (enable == TD_FALSE) {
        lib_inst->entry->reset(lib_inst->handle);
        lib_inst->enable = TD_FALSE;
    }

    lib_inst->enable = enable;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_set_scr_buf(sea_handle handle, td_void *scr_buf, td_u32 scr_size)
{
    td_u32 size = 0;
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    (td_void)lib_inst->entry->get_config(lib_inst->handle, SEA_ALG_CMD_GET_SCR_BUF_SIZE, (td_void *)&size);
    if ((scr_buf == TD_NULL) || (scr_size < size)) {
        return PLG_ERR_BUF_OVERFLOW;
    }

    (td_void)lib_inst->entry->set_config(lib_inst->handle, SEA_ALG_CMD_SET_SCR_BUF, scr_buf);
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_reg_data_output(sea_handle handle, sea_data_output data_output, td_void *context)
{
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    lib_inst->data_context = context;
    lib_inst->data_output = data_output;
    return PLUGIN_SUCCESS;
}

static td_s32 sea_lib_reg_event_report(sea_handle handle, sea_event_report event_report, td_void *context)
{
    sea_lib_inst *lib_inst = (sea_lib_inst *)handle;

    lib_inst->event_context = context;
    lib_inst->event_report = event_report;
    return PLUGIN_SUCCESS;
}

static sea_entry ha_sea_lib_entry = {
    .name = (const td_char *)"sea_lib0",
    .sea_id = SEA_LIB_ID_SEE,
    .version.u32 = 0x04000001,
    .description = (const td_char *)"Sea lib0 developed by XXX",
    .create = sea_lib_create,
    .destroy = sea_lib_destroy,
    .proc_frame = sea_lib_proc_frame,
    .set_enable = sea_lib_set_enable,
    .set_scr_buf = sea_lib_set_scr_buf,
    .get_eng_caps = sea_lib_get_eng_caps,
    .get_def_pcm_attr = sea_lib_get_def_pcm_attr,
    .get_item_sets = TD_NULL,
    .get_eng_attr = sea_lib_get_eng_attr,
    .set_eng_attr = sea_lib_set_eng_attr,
    .get_parameter = sea_lib_get_parameter,
    .set_parameter = sea_lib_set_parameter,
    .set_input_attr = TD_NULL,
    .set_output_attr = TD_NULL,
    .reg_data_output = sea_lib_reg_data_output,
    .reg_event_report = sea_lib_reg_event_report
};

define_component(SEA, ha_sea_lib_entry);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
