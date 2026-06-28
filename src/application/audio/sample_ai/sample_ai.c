/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: ai sample
 * Author: audio
 * Create: 2019-09-17
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "chip_type.h"
#include "audio_type.h"
#include "osal_list.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_ai.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"
#include "sample_phone.h"
#include "soc_uapi_sea.h"
#ifdef SAP_SEA_SEE_1MIC_SUPPORT
#include "sea_aef.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_char *sample;
    td_char *size;
    td_char *file;
    td_char *port;
} sample_ai_arg;

typedef struct {
    td_char *sample;
    td_char *ai_vol;
    td_char *ai_handle;
    td_char *integer;
    td_char *decimal;
} sample_ai_vol_arg;

typedef struct sample_ai_inst {
    athread_handle task;
    td_bool task_enable;
    td_bool mute;
    td_bool codec_test;

    FILE *h_file;
    td_u32 file_size;
    td_u32 file_limit_size;
    td_handle h_adp;
    td_handle h_ai;
    td_handle h_sea;
    td_handle h_track;
    td_handle h_player;
    uapi_ai_port ai_port;
    uapi_audio_pcm_format pcm_format;
    uapi_ai_gain volume;
    td_u32 mic_num;
    uapi_ai_amic_sel mic_sel[UAPI_AI_MIC_NUM_MAX];
    td_bool sea_enable;
    td_bool ref_enable;
    uapi_sea_lib_id lib_id;

    /* ao */
    uapi_snd snd_id;
    uapi_snd_out_port ao_port;
    uapi_snd_track_mode track_mode;

    td_u32 (*save_frame)(struct sample_ai_inst *inst, const uapi_audio_frame *frame);
    data_info write_data;
    td_bool run_bt_loopback;
    td_bool run_dsp_loopback;
    td_void (*sample_ai_process)(struct sample_ai_inst *inst, const uapi_audio_frame *frame);
    struct osal_list_head node;
} sample_ai_inst;

static OSAL_LIST_HEAD(g_sample_ai_list);

static inline td_bool ai_check_volume_adjust_support(uapi_ai_port ai_port)
{
    if ((ai_port >= UAPI_AI_PORT_ADC0 && ai_port <= UAPI_AI_PORT_ADC3) ||
        (ai_port >= UAPI_AI_PORT_LPADC0 && ai_port <= UAPI_AI_PORT_LPADC1) ||
        (ai_port >= UAPI_AI_PORT_PDM0 && ai_port <= UAPI_AI_PORT_PDM3)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

static sample_ai_inst *sample_ai_alloc_inst(td_void)
{
    sample_ai_inst *inst = (sample_ai_inst *)malloc(sizeof(sample_ai_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_ai_inst));

    osal_list_add_tail(&inst->node, &g_sample_ai_list);
    return inst;
}

static td_void sample_ai_free_inst(sample_ai_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_ai_inst *sample_ai_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_ai_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_ai_list, sample_ai_inst, node);
}

static td_s32 ai_event_proc(td_handle ai, uapi_ai_event_type event, const td_void *param, td_void *context)
{
    sample_ai_inst *test_inst = (sample_ai_inst *)context;

    sap_err_log_h32(event);

    if (event == UAPI_AI_EVENT_VAD_VALID) {
    } else {
    }

    sap_unused(ai);
    sap_unused(param);
    sap_unused(test_inst);
    return EXT_SUCCESS;
}

static td_u32 save_frame_to_buf(sample_ai_inst *inst, const uapi_audio_frame *frame)
{
    circ_buf *cb = &inst->write_data.cb;

    if (circ_buf_query_free(cb) <= frame->bits_bytes) {
        sap_printf("Warning: PCM data enough");
        return 0;
    }

    return circ_buf_write(cb, (const td_u8 *)frame->bits_buffer, frame->bits_bytes);
}

static td_u32 save_frame_to_file(sample_ai_inst *inst, const uapi_audio_frame *frame)
{
    td_u32 ret;
    if (inst->h_file == TD_NULL) {
        return 0;
    }

    ret = sap_fwrite((td_void *)frame->bits_buffer, 1, frame->bits_bytes, inst->h_file);
    return ret;
}

static td_void sample_ai_save_frame(sample_ai_inst *inst, const uapi_audio_frame *frame)
{
    inst->file_size += frame->bits_bytes;
    if (inst->file_size >= inst->file_limit_size) {
        return;
    }

    if (inst->save_frame(inst, frame) == 0) {
        inst->file_size = inst->file_limit_size;
    }
}

static td_void sample_ai_ao_process(sample_ai_inst *inst, const uapi_audio_frame *frame)
{
    td_s32 ret;

    while (inst->task_enable) {
        ret = uapi_adp_send_frame(inst->h_player, frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS); /*   */
            continue;
        }
        break;
    }
}

static td_void sample_ai_thread(td_void *args)
{
    td_s32 ret;
    uapi_audio_frame frame;
    sample_ai_inst *inst = (sample_ai_inst *)args;

    if (inst == TD_NULL) {
        sap_err_log_info("ai_inst is null");
        return;
    }

    while (inst->task_enable) {
        ret = uapi_adp_acquire_frame(inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        if (frame.bit_depth != UAPI_AUDIO_BIT_DEPTH_16) {
            frame.bit_depth = UAPI_AUDIO_BIT_DEPTH_24;
        }

        if (inst->sample_ai_process != NULL) {
            inst->sample_ai_process(inst, &frame);
        }

        ret = uapi_adp_release_frame(inst->h_adp, &frame);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_frame, ret);
        }
    }

    athread_set_exit(inst->task, TD_TRUE);
}

static td_s32 sample_ai_sea_init(sample_ai_inst *inst)
{
    td_s32 ret;

    if (inst->sea_enable == TD_FALSE) {
        return EXT_SUCCESS;
    }

    ret = uapi_sea_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_init, ret);
        return ret;
    }

    ret = uapi_sea_load_engine(inst->lib_id, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_load_engine, ret);
        (td_void)uapi_sea_deinit();
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_ai_sea_deinit(const sample_ai_inst *inst)
{
    td_s32 ret;
    if (inst->sea_enable == TD_FALSE) {
        return;
    }

    ret = uapi_sea_unload_engine(inst->lib_id, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_unload_engine, ret);
    }

    ret = uapi_sea_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_deinit, ret);
    }
}

static td_s32 sample_ai_sys_init(sample_ai_inst *inst)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_ai_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_init, ret);
        goto out0;
    }

    ret = sample_ai_sea_init(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_sea_init, ret);
        goto out1;
    }

    return EXT_SUCCESS;
out1:
    (td_void)uapi_ai_deinit();
out0:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_ai_sys_deinit(sample_ai_inst *inst)
{
    td_s32 ret;

    sample_ai_sea_deinit(inst);

    ret = uapi_ai_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_ai_close_adp(sample_ai_inst *inst)
{
    if (inst->h_adp != 0) {
        uapi_adp_destroy(inst->h_adp);
        inst->h_adp = 0;
    }
}

static td_s32 sample_ai_open_adp(sample_ai_inst *inst)
{
    td_s32 ret;
    uapi_adp_attr adp_attr;

    inst->h_adp = 0;

    if (inst->run_dsp_loopback) {
        return EXT_SUCCESS;
    }

    ret = uapi_adp_get_def_attr(&adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_get_def_attr, ret);
        return ret;
    }

    ret = uapi_adp_create(&inst->h_adp, &adp_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_create, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_ai_close_ai(sample_ai_inst *inst)
{
    td_s32 ret;

    if (inst->h_ai == 0) {
        return;
    }

    if (inst->h_sea != 0) {
        ret = uapi_ai_detach_output(inst->h_ai, inst->h_sea);
    } else if (inst->h_adp != 0) {
        ret = uapi_ai_detach_output(inst->h_ai, inst->h_adp);
    } else {
        ret = uapi_ai_detach_output(inst->h_ai, inst->h_track);
    }

    ret = uapi_ai_close(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_close, ret);
    }

    inst->h_ai = 0;
}

static td_void sample_ai_set_adc_mic_sel(const sample_ai_inst *inst, uapi_ai_attr *ai_attr)
{
    td_u32 i;

    if (inst->mic_num == 0) {
        return;
    }

    /* 命令行传入的值，可选 */
    for (i = 0; i < UAPI_AI_MIC_NUM_MAX; i++) {
        ai_attr->port_attr.adc.mic_sel[i] = inst->mic_sel[i];
    }
}

static td_void sample_ai_set_port_attr(const sample_ai_inst *inst, uapi_ai_attr *ai_attr)
{
    td_s32 ret;
    td_bool local_out_port;

    ret = memcpy_s(&ai_attr->pcm_attr, sizeof(ai_attr->pcm_attr), &inst->pcm_format, sizeof(inst->pcm_format));
    if (ret != EOK) {
        return;
    }

    if (inst->ref_enable == TD_TRUE) {
        ai_attr->ref_attr.enable = inst->ref_enable;
        ai_attr->ref_attr.port = SND_OUT_PORT_DEFAULT;
    } else {
        ai_attr->ref_attr.enable = TD_FALSE;
    }

    switch (inst->ai_port) {
        case UAPI_AI_PORT_PDM0:
            ai_attr->port_attr.pdm.rx_type = UAPI_AI_RX_DEFAULT;
            ai_attr->port_attr.pdm.i2s_attr.channels = inst->pcm_format.channels;
            ai_attr->port_attr.pdm.i2s_attr.bit_depth = inst->pcm_format.bit_depth;
            break;

        case UAPI_AI_PORT_ADC0:
        case UAPI_AI_PORT_LPADC0:
        case UAPI_AI_PORT_LPADC1:
        case UAPI_AI_PORT_ANC0:
            ai_attr->port_attr.adc.rx_type = UAPI_AI_RX_DEFAULT;
            sample_ai_set_adc_mic_sel(inst, ai_attr);
            break;

        case UAPI_AI_PORT_I2S0:
        case UAPI_AI_PORT_I2S1:
        case UAPI_AI_PORT_I2S2:
            local_out_port = (inst->ai_port != CAT1_OUT_PORT_DEFAULT) ? TD_TRUE : TD_FALSE;
            get_i2s_attr(&ai_attr->port_attr.i2s.i2s_attr, (const uapi_audio_pcm_format *)&inst->pcm_format, local_out_port);
            break;

        default:
            break;
    }
}

static td_s32 sample_ai_open_ai(sample_ai_inst *inst)
{
    td_s32 ret;
    uapi_ai_attr ai_attr;

    ret = uapi_ai_get_default_attr(inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_get_default_attr, ret);
        return ret;
    }

    sample_ai_set_port_attr(inst, &ai_attr);

    ret = uapi_ai_open(&inst->h_ai, inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_open, ret);
        return ret;
    }

    /* Register AI event process callback */
    ret = uapi_ai_register_event_proc(inst->h_ai, ai_event_proc, (td_void *)inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_register_event_proc, ret);
        goto out;
    }

    if (ai_check_volume_adjust_support(inst->ai_port) == TD_TRUE) {
        ret = uapi_ai_set_volume(inst->h_ai, &inst->volume);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_ai_set_volume, ret);
            goto out;
        }

        ret = uapi_ai_set_mute(inst->h_ai, inst->mute);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_ai_set_mute, ret);
            goto out;
        }
    }

    if (inst->h_sea != 0) {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_sea);
    } else if (inst->h_adp != 0) {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_adp);
    } else {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_track);
    }
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_ai_close(inst->h_ai);
    return ret;
}

static td_s32 sample_ai_set_sea_param(sample_ai_inst *inst)
{
    td_s32 ret;
    td_u32 params_size;
    td_u8 *params;

#ifndef SAP_SEA_SEE_1MIC_V7_SUPPORT
    audio_unused(inst);
    return EXT_SUCCESS;
#endif

    params_size = 288; /* 288: 1MIC_V7 param size */
    params = (td_u8 *)malloc(params_size);
    if (params == TD_NULL) {
        sap_printf("malloc failed\n");
        return EXT_FAILURE;
    }

    ret = uapi_sea_get_param(inst->h_sea, UAPI_SEA_AFE_SEE_RW_PARAM_CMD, params, params_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_param, ret);
        free(params);
        return ret;
    }
    ret = uapi_sea_set_param(inst->h_sea, UAPI_SEA_AFE_SEE_RW_PARAM_CMD, params, params_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_param, ret);
        free(params);
        return ret;
    }

    free(params);
    return EXT_SUCCESS;
}

static td_s32 sample_ai_open_sea(sample_ai_inst *inst)
{
    td_s32 ret;
    uapi_sea_attr sea_attr;
    uapi_sea_eng_sel sea_eng;

    inst->h_sea = 0;
    if (!inst->sea_enable) {
        return EXT_SUCCESS;
    }

    /* Create SEA instance */
    (td_void)memset_s(&sea_eng, sizeof(sea_eng), 0, sizeof(sea_eng));
    sea_eng.afe.type = UAPI_SEA_AFE_SEE;
    sea_eng.afe.lib_id = inst->lib_id;
    ret = uapi_sea_get_default_attr(&sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_default_attr, ret);
        return ret;
    }

    sea_attr.in_pcm.bit_depth = inst->pcm_format.bit_depth;
    sea_attr.in_pcm.channels = inst->pcm_format.channels;
    sea_attr.in_pcm.sample_rate = inst->pcm_format.sample_rate;
    sea_attr.in_pcm.sample_per_frame = inst->pcm_format.sample_per_frame;
    sea_attr.ref_pcm.channels = ((inst->ref_enable == TD_TRUE) ? 1 : 0);
    ret = uapi_sea_create(&inst->h_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        return ret;
    }

    ret = sample_ai_set_sea_param(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_set_sea_param, ret);
        goto out;
    }

    if (inst->h_adp != 0) {
        ret = uapi_sea_attach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_adp);
    } else {
        ret = uapi_sea_attach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_track);
    }
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;

out:
    (td_void)uapi_sea_destroy(inst->h_sea);
    return ret;
}

static td_void sample_ai_close_sea(sample_ai_inst *inst)
{
    td_s32 ret;

    if (inst->h_sea == 0) {
        return;
    }

    if (inst->h_adp != 0) {
        ret = uapi_sea_detach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_adp);
    } else {
        ret = uapi_sea_detach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->h_track);
    }
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_detach_output, ret);
    }

    ret = uapi_sea_destroy(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_destroy, ret);
    }

    inst->h_sea = 0;
}

static td_void sample_ai_close_file(sample_ai_inst *inst)
{
    if (inst->h_file != TD_NULL) {
        sap_fclose(inst->h_file);
        inst->h_file = TD_NULL;
    }
}

static td_s32 sample_ai_open_file(sample_ai_inst *inst, const td_char *file)
{
    if (file == TD_NULL || strcmp("null", file) == 0) {
        inst->h_file = TD_NULL;
        data_info_init(&inst->write_data, FPGA_SAVE_DATA_ADDR, FPGA_SAVE_DATA_SIZE);
        inst->save_frame = save_frame_to_buf;
    } else {
        td_s32 ret;
        td_char file_path[FILE_PATH_LEN];
        ret = snprintf_s(file_path, FILE_PATH_LEN - 1, FILE_PATH_LEN - 1, "%s_%uk_%ubit_%uch.pcm", file,
            inst->pcm_format.sample_rate / 1000, /* 1000 Hz to kHz */
            inst->pcm_format.bit_depth, inst->pcm_format.channels);
        if (ret < 0) {
            sap_err_log_fun(snprintf_s, ret);
            return EXT_FAILURE;
        }

        inst->h_file = sap_fopen(file_path, "wb");
        if (inst->h_file == TD_NULL) {
            sap_printf("open %s failed", file_path);
            return EXT_FAILURE;
        }
        inst->save_frame = save_frame_to_file;
    }

    return EXT_SUCCESS;
}

static td_void sample_ai_inst_deinit(sample_ai_inst *inst)
{
    sample_ai_close_file(inst);
}

static td_s32 sample_ai_inst_init(const sample_ai_arg *arg, sample_ai_inst *inst)
{
    td_s32 ret;
    td_u32 size;

    if (arg->port != TD_NULL) {
        inst->ai_port = sample_audio_get_ai_port(arg->port);
    } else {
        inst->ai_port = AI_PORT_DEFAULT;
    }

    size = (td_u32)strtoul(arg->size, TD_NULL, 0);
    if (size == 0 || size > 100) { /* 100 max file size */
        sap_err_log_ret(size);
        return EXT_FAILURE;
    }

    inst->file_limit_size = (size << 20); /* 20: bytes to MB */

    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    inst->volume.integer = AI_VOLUME_DEFAULT;
    inst->volume.decimal = 0;

    ret = sample_ai_open_file(inst, arg->file);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_file, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_ai_close_inst(sample_ai_inst *inst)
{
    sample_ai_close_ai(inst);
    sample_ai_close_sea(inst);
    sample_ai_close_adp(inst);
    sample_ai_sys_deinit(inst);
}

static td_s32 sample_ai_open_inst(sample_ai_inst *inst)
{
    td_s32 ret;

    ret = sample_ai_sys_init(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 ai --> adp
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_ai_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_adp, ret);
        goto out0;
    }

    ret = sample_ai_open_sea(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_aenc_open_sea, ret);
        goto out1;
    }

    ret = sample_ai_open_ai(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_ai, ret);
        goto out2;
    }

    return EXT_SUCCESS;

out2:
    sample_ai_close_sea(inst);
out1:
    sample_ai_close_adp(inst);
out0:
    sample_ai_sys_deinit(inst);
    return ret;
}

static td_void sample_ai_stop_inst(sample_ai_inst *inst)
{
    td_s32 ret;

    if (inst->task != TD_NULL) {
        /* Destory Task to stop ADP instance reading pcm data */
        inst->task_enable = TD_FALSE;
        athread_exit(inst->task);
        inst->task = TD_NULL;
    }

    if (inst->sea_enable) {
        ret = uapi_sea_stop(inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_stop, ret);
        }
        sample_gui_aef_delete_handle(inst->h_sea);
    }

    /* Stop AI instance output pcm data */
    ret = uapi_ai_stop(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_stop, ret);
    }
}

static td_s32 sample_ai_start_inst(sample_ai_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    /* Start AI instance to write data into ADP instance */
    ret = uapi_ai_start(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
        return ret;
    }

    inst->sample_ai_process = sample_ai_save_frame;
    inst->task_enable = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_ai";
    ret = athread_create(&inst->task, sample_ai_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_ai_stop(inst->h_ai);
    return ret;
}

static td_void sample_ai_usage(const td_char *name)
{
    sap_printf(" usage: %s size(MB) file(*.pcm)\n", name);
    sap_printf("set ai high precision gain\n");
    sap_printf("  %s ai_vol ai_handle vol_integer vol_decimal\n", name);
    sap_printf(" examples:\n");
    sap_printf(" %s 100 /mnt/ai.pcm\n", name);
    sap_printf(" %s 100 null\n", name);
    sap_printf("set ai gain to 6.5dB\n");
    sap_printf("  %s ai_vol 0x6d000008 6 500\n", name);
    sap_printf("set ai gain to 12dB\n");
    sap_printf("  %s ai_vol 0x6d000008 12 0\n", name);
}

static td_s32 sample_ai_vol_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_handle ai;
    uapi_ai_gain gain = {0, 0};
    sample_ai_vol_arg *arg = (sample_ai_vol_arg *)argv;
    if (argc != (sizeof(sample_ai_vol_arg) / sizeof(td_char *))) {
        sample_ai_usage(argv[0]);
        return EXT_FAILURE;
    }

    ai = (td_u32)strtol(arg->ai_handle, TD_NULL, 16); /* 16进制 */
    gain.integer = (td_s32)strtol(arg->integer, TD_NULL, 0);
    gain.decimal = (td_s32)strtol(arg->decimal, TD_NULL, 0);

    ret = uapi_ai_set_volume(ai, &gain);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_set_volume, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ai_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_ai_arg *arg = (sample_ai_arg *)argv;
    sample_ai_inst *inst = sample_ai_get_inst();
    if (inst != TD_NULL) {
        sap_printf("%s is already running\n", argv[0]);
        return EXT_FAILURE;
    }

    inst = sample_ai_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_ai_alloc_inst failed\n");
        return EXT_FAILURE;
    }

    ret = sample_ai_inst_init(arg, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_inst_init, ret);
        goto out0;
    }

    ret = sample_ai_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_inst, ret);
        goto out1;
    }

    ret = sample_ai_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_start_inst, ret);
        goto out2;
    }

    sap_printf("sample_rate: %u, channels: %u, bit_depth: %u", inst->pcm_format.sample_rate,
        inst->pcm_format.channels, inst->pcm_format.bit_depth);

    return EXT_SUCCESS;
out2:
    sample_ai_close_inst(inst);
out1:
    sample_ai_inst_deinit(inst);
out0:
    sample_ai_free_inst(inst);

    audio_unused(argc);
    return ret;
}

static td_s32 sample_ai_exit(td_void)
{
    sample_ai_inst *inst = sample_ai_get_inst();

    if (inst != NULL) {
        sample_ai_stop_inst(inst);
        sample_ai_close_inst(inst);
        sample_ai_inst_deinit(inst);
        sample_ai_free_inst(inst);
    }

    return EXT_SUCCESS;
}

td_s32 sample_ai(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ai_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ai_exit();
    } else if (strcmp(argv[1], "ai_vol") == 0) {
        return sample_ai_vol_entry(argc, argv);
    } else {
        return sample_ai_entry(argc, argv);
    }
}

static td_void sample_ai_ao_usage(const td_char *name)
{
    sap_printf(" usage: %s -p [port] -c [channels] -b [bit_depth] -s [sample_rate] -v [volume]\n", name);
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("        -p     specify ai port(mad0/i2s0/i2s1/adc0/lpadc0)\n"
           "        -c     channels of ai port(default: 2)\n"
           "        -b     bit_depth of ai port(default: 16)\n"
           "        -s     samplerate of ai port(default: 48000)\n"
           "        -v     volume of ai port(default: 0)\n"
           "        -m     mute ai port(default: false)\n"
           "        -t     sound track mode(default: 0) range[0,7]\n"
           "        -l     specify a long parameter(bt_loopback/mic0/mic1)\n"
           "\n");
    sap_printf(" examples:\n");
    sap_printf(" %s -p mad0 -c 1 -s 16000\n", name);
    sap_printf(" %s -p mad0 -c 2 -s 48000\n", name);
    sap_printf(" %s -p i2s1\n", name);
    sap_printf(" %s -p i2s1 -c 1\n", name);
    sap_printf(" %s -p adc0 -c 2 -s 48000 -v 30\n", name);
    sap_printf(" %s -p adc0 -c 2 -s 48000 -t 1\n", name);
    sap_printf("\n");
}

static td_void sample_ai_ao_parse_snd_id(const td_char *snd_name, sample_ai_inst *inst)
{
    td_u32 i;
    const struct {
        const td_char *name;
        uapi_snd snd_id;
    } ao_snd_name[] = {
        {"snd0", UAPI_SND_0},
        {"snd1", UAPI_SND_1},
        {"snd2", UAPI_SND_2},
    };

    for (i = 0; i < sizeof(ao_snd_name) / sizeof(ao_snd_name[0]); i++) {
        if (strcmp(ao_snd_name[i].name, snd_name) == 0) {
            inst->snd_id = ao_snd_name[i].snd_id;
            return;
        }
    }

    inst->snd_id = UAPI_SND_0;
}

static td_void sample_ai_ao_parse_mic(const td_char *opt_arg, sample_ai_inst *inst)
{
    if (inst->mic_num >= UAPI_AI_MIC_NUM_MAX) {
        return;
    }

    if (strcmp(opt_arg, "mic0") == 0) {
        inst->mic_sel[inst->mic_num++] = UAPI_AI_AMIC0;
        return;
    }

    if (strcmp(opt_arg, "mic1") == 0) {
        inst->mic_sel[inst->mic_num++] = UAPI_AI_AMIC1;
        return;
    }

    if (strcmp(opt_arg, "mic2") == 0) {
        inst->mic_sel[inst->mic_num++] = UAPI_AI_AMIC2;
        return;
    }
}

static td_s32 sample_ai_ao_parse_snd_track_mode(const td_char *opt_arg, sample_ai_inst *inst)
{
    td_u32 track_mode;

    track_mode = (td_u32)strtoul(opt_arg, TD_NULL, 0);
    if (track_mode < UAPI_SND_TRACK_STEREO || track_mode >= UAPI_SND_TRACK_MODE_MAX) {
        sap_printf("invalid sound track mode, range[0,7]\n");
        return EXT_FAILURE;
    } else {
        inst->track_mode = (uapi_snd_track_mode)track_mode;
    }
    return EXT_SUCCESS;
}

static td_void sample_ai_ao_parse_long_opt(td_char opt, const td_char *opt_arg, sample_ai_inst *inst)
{
    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return;
    }

    if (strcmp(opt_arg, "bt_loopback") == 0) {
        inst->run_bt_loopback = TD_TRUE;
        return;
    }

    if (strcmp(opt_arg, "codec_test") == 0) {
        inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_2;
        inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_32;
        inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_48K;
        inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */
        inst->codec_test = TD_TRUE;
        return;
    }

    if (strcmp(opt_arg, "dsp_loopback") == 0) {
        inst->run_dsp_loopback = TD_TRUE;
        return;
    }

    if (strcmp(opt_arg, "ref") == 0) {
        inst->ref_enable = TD_TRUE;
        return;
    }

    // 通过指定-lahe，指定加载UAPI_SEA_LIB_AHE
    if (strcmp(opt_arg, "ahe") == 0) {
        inst->lib_id = UAPI_SEA_LIB_AHE;
        return;
    }

    sample_ai_ao_parse_mic(opt_arg, inst);
}

static td_s32 sample_ai_ao_parse_opt(td_char opt, const td_char *opt_arg, sample_ai_inst *inst)
{
    if (opt == 'h') {
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 's': {
            inst->pcm_format.sample_rate = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */
            break;
        }
        case 'c':
            inst->pcm_format.channels = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'b':
            inst->pcm_format.bit_depth = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'n':
            inst->pcm_format.sample_per_frame = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'p':
            inst->ai_port = sample_audio_get_ai_port(opt_arg);
            break;
        case 'o':
            inst->ao_port = sample_audio_get_ao_port(opt_arg);
            break;
        case 'd':
            sample_ai_ao_parse_snd_id(opt_arg, inst);
            break;
        case 'v':
            inst->volume.integer = (td_s32)strtol(opt_arg, TD_NULL, 0);
            break;
        case 'm':
            inst->mute = TD_TRUE;
            break;
        case 't':
            return sample_ai_ao_parse_snd_track_mode(opt_arg, inst);
        case 'l':
            sample_ai_ao_parse_long_opt(opt, opt_arg, inst);
            break;
        default:
            sap_printf("invalid command line\n");
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ai_ao_parse_cmd_line(td_s32 argc, td_char *argv[], sample_ai_inst *inst)
{
    td_s32 ret;
    td_char opt;
    parg_state ps;

    if (argv == TD_NULL) {
        return EXT_FAILURE;
    }

    parg_init(&ps);

    while (1) {
        ret = parg_getopt(&ps, argc, argv, "t:s:c:b:n:p:v:o:d:hml:");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;
        if (sample_ai_ao_parse_opt(opt, ps.optarg, inst) != EXT_SUCCESS) {
            sample_ai_ao_usage(argv[0]);
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ai_ao_start_inst(sample_ai_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    /* Start AI instance to write data into ADP instance */
    ret = uapi_ai_start(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
        return ret;
    }

    if (inst->sea_enable) {
        ret = uapi_sea_start(inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_start, ret);
            goto out0;
        }

        sample_gui_aef_add_handle(GUI_AEF_SEA, inst->h_sea);
    }

    if (inst->h_adp == 0) {
        return EXT_SUCCESS;
    }

    /* Create task for ADP instance to read pcm data */
    inst->task_enable = TD_TRUE;
    inst->sample_ai_process = sample_ai_ao_process;

    if (inst->run_bt_loopback != TD_TRUE) {
        attr.priority = ATHREAD_PRIORITY_NORMAL;
        attr.stack_size = 0x1000; /* 4k */
        attr.name = "sample_ai_ao";
        ret = athread_create(&inst->task, sample_ai_thread, (td_void *)inst, &attr);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(athread_create, ret);
            goto out1;
        }
    } else {
        adp_handle_share_set(inst->h_adp, inst->h_player, 0);
        sap_printf("[audio] ul handle = x%x, dl handle = x%x\n", inst->h_adp, inst->h_player);
    }

    return EXT_SUCCESS;
out1:
    if (inst->sea_enable) {
        (td_void)uapi_sea_stop(inst->h_sea);
        sample_gui_aef_delete_handle(inst->h_sea);
    }
out0:
    (td_void)uapi_ai_stop(inst->h_ai);
    return ret;
}

static td_void sample_ai_ao_inst_init(sample_ai_inst *inst)
{
    td_u32 i;

    inst->ai_port = AI_PORT_DEFAULT;

    inst->snd_id = UAPI_SND_0;
    inst->ao_port = SND_OUT_PORT_DEFAULT;
    inst->codec_test = TD_FALSE;
    inst->track_mode = TRACK_MODE_DEFAULT;
    inst->lib_id = UAPI_SEA_LIB_SEE;

    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    for (i = 0; i < UAPI_AI_MIC_NUM_MAX; i++) {
        inst->mic_sel[i] = UAPI_AI_AMIC_MAX;
    }
}

static td_s32 sample_ai_player_open(sample_ai_inst *inst)
{
    td_s32 ret;
    td_handle h_snd;
    sample_acodec_arg in_arg;
    const uapi_snd_attr snd_attr = {
        .port_num = 1,
        .port_attr[0].out_port = inst->ao_port,
        .channels = SND_OUT_CHANNEL_DEF,
        .bit_depth = SND_OUT_BIT_DEPTH_DEF,
        .sample_rate = SND_OUT_SAMPLE_RATE_DEF,
    };

    sample_escast_arg escast_arg = {
        .player = &inst->h_player,
        .track = &inst->h_track,
        .in_arg = &in_arg,
        .snd = &h_snd,
        .snd_attr = &snd_attr,
        .audio_scene = UAPI_AUDIO_SCENE_UNKNOWN,
        .in_file = TD_NULL,
        .in_data_addr = 0,
        .in_data_size = 0,
        .track_mode = inst->track_mode,
        .run_dsp_loopback = inst->run_dsp_loopback,
    };

    /* set track input pcm format matching ai output pcm format */
    ret = memcpy_s(&in_arg.pcm_format, sizeof(in_arg.pcm_format), &inst->pcm_format, sizeof(inst->pcm_format));
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return ret;
    }

    return sample_ao_player_open(&escast_arg, inst->snd_id, inst->codec_test);
}

static td_s32 sample_ai_ao_entry(td_s32 argc, td_char *argv[], td_bool sea_enable)
{
    td_s32 ret;
    sample_ai_inst *inst = TD_NULL;

    inst = sample_ai_alloc_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ai_ao_alloc_inst failed");
        return EXT_FAILURE;
    }

    sample_ai_ao_inst_init(inst);
    inst->sea_enable = sea_enable;

    ret = sample_ai_ao_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_ai_player_open(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_player_open, ret);
        goto out0;
    }

    ret = sample_ai_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_open_inst, ret);
        goto out1;
    }

    ret = sample_ai_ao_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ai_start_inst, ret);
        goto out2;
    }

    sap_alert_log_u32(inst->pcm_format.sample_rate);
    sap_alert_log_u32(inst->pcm_format.channels);
    sap_alert_log_u32(inst->pcm_format.bit_depth);

    return EXT_SUCCESS;
out2:
    sample_ai_close_inst(inst);
out1:
    sample_ao_player_close();
out0:
    sample_ai_free_inst(inst);
    return ret;
}

static td_s32 sample_ai_ao_exit(td_void)
{
    sample_ai_inst *inst = sample_ai_get_inst();

    if (inst != NULL) {
        sample_ai_stop_inst(inst);
        sample_ai_close_inst(inst);
        sample_ao_player_close();
        sample_ai_free_inst(inst);
    }

    return EXT_SUCCESS;
}

td_s32 sample_ai_ao(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ai_ao_usage(argv[0]);
        return EXT_FAILURE;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ai_ao_exit();
    } else {
        return sample_ai_ao_entry(argc, argv, TD_FALSE);
    }
}

td_s32 sample_ai_sea_ao(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ai_ao_usage(argv[0]);
        return EXT_FAILURE;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ai_ao_exit();
    } else {
        return sample_ai_ao_entry(argc, argv, TD_TRUE);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
