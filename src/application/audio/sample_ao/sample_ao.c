/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: ao sample
 * Author: audio
 * Create: 2019-09-17
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "thread_os.h"
#include "chip_type.h"
#include "osal_list.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_adec.h"
#include "soc_uapi_sound.h"
#include "soc_uapi_audio_sys.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_MIXER_NUM 4
#define I2S_TDM_TEST_MAX_ARGC 7
#define I2S_TDM_TEST_INDEX 1
#define I2S_TDM_TEST_I2S_ID UAPI_SND_OUT_PORT_I2S1

typedef struct {
    td_char *in_stream;
    td_char *volume;
    td_char *sample_rate;
    td_char *bit_depth;
    td_char *channels;
} sample_ao_track_arg;

typedef struct {
    td_char *sample;
    sample_ao_track_arg track_arg[MAX_MIXER_NUM];
} sample_ao_arg;

typedef struct {
    td_char *sample;
    td_char *ao_vol;
    td_char *integer;
    td_char *decimal;
} sample_ao_vol_arg;

typedef struct {
    td_char *sample;
    td_char *track_vol;
    td_char *track;
    td_char *integer;
    td_char *decimal;
} sample_ao_track_vol_arg;

typedef struct sample_ao_track_inst {
    athread_handle task;
    td_bool task_active;
    td_bool run_dsp_loopback;

    td_char *in_stream;
    FILE *h_file;
    td_u8 *frame_buf;
    td_u32 frame_size;

    td_handle h_adp;
    td_handle h_track;

    data_info data_info;
    td_u32 (*read_frame)(struct sample_ao_track_inst *inst, uapi_audio_frame *frame);
    td_s32 volume;
    uapi_audio_pcm_format pcm_format;
} sample_ao_track_inst;

typedef struct {
    uapi_snd snd_id;
    td_handle h_snd;
    uapi_snd_attr snd_attr;
    uapi_snd_aef_profile aef_profile;
    td_bool thread_enable;
    td_u32 track_num;
    sample_ao_track_inst track_inst[MAX_MIXER_NUM];
    uapi_snd_track_mode track_mode;
    struct osal_list_head node;
} sample_ao_inst;

static td_u32 g_vector_read = 0;

static OSAL_LIST_HEAD(g_sample_ao_list);

static td_void sample_ao_show_inst(td_void)
{
    td_u32 cnt = 0;
    sample_ao_inst *inst = TD_NULL;

    if (osal_list_empty(&g_sample_ao_list) != TD_FALSE) {
        sap_alert_log_info("sample_ao all exit!");
        return;
    }

    osal_list_for_each_entry(inst, &g_sample_ao_list, node) {
        cnt++;
        sap_trace_log_u32(cnt);
        sap_trace_log_u32(inst->snd_id);
        sap_trace_log_u32(inst->track_num);
    }
    sap_unused(cnt);
}

static sample_ao_inst *sample_ao_alloc_inst(td_void)
{
    sample_ao_inst *inst = (sample_ao_inst *)malloc(sizeof(sample_ao_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    memset_s(inst, sizeof(*inst), 0, sizeof(sample_ao_inst));

    osal_list_add_tail(&inst->node, &g_sample_ao_list);
    return inst;
}

static td_void sample_ao_free_inst(sample_ao_inst *inst)
{
    if (inst == TD_NULL) {
        return;
    }

    osal_list_del(&inst->node);
    free(inst);
}

static sample_ao_inst *sample_ao_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_ao_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_ao_list, sample_ao_inst, node);
}

static td_u32 sample_ao_track_read_file(sample_ao_track_inst *inst, uapi_audio_frame *frame)
{
    td_u32 read_len;

    if (inst->h_file == TD_NULL) {
        return 0;
    }

    read_len = sap_fread(frame->bits_buffer, 1, frame->bits_bytes, inst->h_file);
    if (read_len == 0) {
        sap_printf("reset read\n");
        sap_rewind(inst->h_file);
        return 0;
    }

    return read_len;
}

static td_u32 sample_ao_track_read_flash(sample_ao_track_inst *inst, uapi_audio_frame *frame)
{
    td_u32 len = inst->frame_size;
    circ_buf *cb = &inst->data_info.cb;

    if (cb == TD_NULL) {
        sap_err_log_info("Fatal Error: circ_buf pointer is null");
        return 0;
    }

    if (len > cb->size) {
        sap_err_log_info("Fatal Error: data not enough");
        return 0;
    }

    if (circ_buf_query_busy(cb) < len) {
        sap_err_log_info("reset read");
        circ_buf_flush(cb);
        circ_buf_update_write_pos(cb, cb->size - sizeof(td_s16));
    }

    if (circ_buf_read(cb, (td_u8 *)frame->bits_buffer, len) != len) {
        sap_err_log_info("Fatal Error: circ_buf_read error");
        return 0;
    }

    circ_buf_update_write_pos(cb, len);
    return len;
}

static td_u32 sample_ao_track_read_array(sample_ao_track_inst *inst, uapi_audio_frame *frame)
{
    td_u32 i;
    td_u32 ch;
    td_u16 *buf = (td_u16 *)(td_uintptr_t)frame->bits_buffer;
    const td_u16 *vector_16k_1ch_16bit = sample_get_vector_16k_1ch_16bit();
    const td_u32 verctor_size = sample_get_vector_size();

    for (i = 0; i < inst->pcm_format.sample_per_frame; i++) {
        for (ch = 0; ch < inst->pcm_format.channels; ch++) {
            *buf++ = vector_16k_1ch_16bit[g_vector_read];
        }

        g_vector_read = saturate_add(g_vector_read, 1, verctor_size);
    }

    return inst->frame_size;
}

static td_void sample_ao_track_build_frame(const sample_ao_track_inst *inst, uapi_audio_frame *frame)
{
    clear_obj(frame);

    frame->interleaved = TD_TRUE;
    frame->channels = inst->pcm_format.channels;
    frame->bit_depth = inst->pcm_format.bit_depth;
    frame->sample_rate = inst->pcm_format.sample_rate;
    frame->pcm_samples = inst->pcm_format.sample_per_frame;
    frame->bits_buffer = (td_s32 *)inst->frame_buf;
    frame->bits_bytes = inst->frame_size;
    frame->pkg_loss = TD_FALSE;
}

static td_void sample_ao_track_thread(td_void *args)
{
    td_s32 ret;
    td_u32 len;
    uapi_audio_frame frame;
    sample_ao_track_inst *inst = (sample_ao_track_inst *)args;

    inst->frame_buf = (td_u8 *)malloc(inst->frame_size);
    if (inst->frame_buf == TD_NULL) {
        sap_err_log_u32(inst->frame_size);
        sap_err_log_info("malloc buffer failed");
        return;
    }

    sample_ao_track_build_frame(inst, &frame);
    while (inst->task_active) {
        uapi_adp_query_free(inst->h_adp, &len);
        if (len < FILE_READ_LEN_MIN) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }

        len = circ_buf_min(len, inst->frame_size);
        frame.bits_bytes = len;

        len = inst->read_frame(inst, &frame);
        if (len == 0) {
            continue;
        }
        frame.bits_bytes = len;

        while (inst->task_active) {
            ret = uapi_adp_send_frame(inst->h_adp, &frame);
            if (ret != EXT_SUCCESS) {
                sap_msleep(THREAD_SLEEP_10MS);
                continue;
            }
            break;
        }
    }

    free(inst->frame_buf);
    inst->frame_buf = TD_NULL;
    athread_set_exit(inst->task, TD_TRUE);
}

static td_s32 sample_ao_sys_init(td_void)
{
    td_s32 ret;

    ret = uapi_adp_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_init, ret);
        return ret;
    }

    ret = uapi_snd_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_init, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_ao_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_snd_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_ao_close_adp(sample_ao_track_inst *inst)
{
    td_s32 ret;

    if (inst->h_adp == 0) {
        return;
    }

    ret = uapi_adp_detach_output(inst->h_adp, inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_detach_output, ret);
    }

    ret = uapi_adp_destroy(inst->h_adp);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_destroy, ret);
    }

    inst->h_adp = 0;
}

static td_s32 sample_ao_open_adp(sample_ao_track_inst *inst)
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

    ret = uapi_adp_attach_output(inst->h_adp, inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_attach_output, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_adp_destroy(inst->h_adp);
    return ret;
}

static td_void sample_ao_close_snd(sample_ao_inst *inst)
{
    td_s32 ret;

    if (inst->h_snd == 0) {
        return;
    }

    ret = uapi_snd_close(inst->h_snd);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_close, ret);
    }

    inst->h_snd = 0;
}

static td_void sample_ao_set_i2s_attr(uapi_snd_attr *snd_attr)
{
    td_bool local_out_port;
    uapi_audio_pcm_format pcm_fmt = {
        .sample_rate = SND_OUT_I2S_SAMPLE_RATE_DEF,
        .channels = SND_OUT_I2S_CHANNEL_DEF,
        .bit_depth = SND_OUT_I2S_BIT_DEPTH_DEF,
    };
    uapi_audio_i2s_attr *i2s_attr = TD_NULL;

    for (td_u32 i = 0; i < snd_attr->port_num; ++i) {
        if (snd_attr->port_attr[i].out_port < UAPI_SND_OUT_PORT_I2S0 ||
            snd_attr->port_attr[i].out_port > UAPI_SND_OUT_PORT_I2S4) {
            continue; /* Set i2s attr when port is i2s */
        }

        i2s_attr = &snd_attr->port_attr[i].u.i2s_attr.attr;
        local_out_port = (snd_attr->port_attr[i].out_port != CAT1_OUT_PORT_DEFAULT) ? TD_TRUE : TD_FALSE;

        if (local_out_port != TD_TRUE) { /* CAT1 adapt */
            pcm_fmt.sample_rate = SIO_CAT1_I2S_SAMPLE_RATE_DEF;
            pcm_fmt.channels = SIO_CAT1_I2S_CHANNEL_DEF;
            pcm_fmt.bit_depth = SIO_CAT1_I2S_BIT_DEPTH_DEF;
        }

#if defined(AUDIO_AW85825) && defined(SAP_SOCMN2)
        if (snd_attr->port_attr[i].out_port == I2S_TDM_TEST_I2S_ID) {
            pcm_fmt.sample_rate = snd_attr->sample_rate;
            pcm_fmt.channels = snd_attr->channels;
            pcm_fmt.bit_depth = snd_attr->bit_depth;
        }
#endif

        get_i2s_attr(i2s_attr, &pcm_fmt, local_out_port);
    }
}

static td_s32 sample_ao_open_snd(sample_ao_inst *inst)
{
    td_s32 ret;
    uapi_snd_attr snd_attr;

    inst->h_snd = 0;
    ret = uapi_snd_get_default_attr(inst->snd_id, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_default_attr, ret);
        return ret;
    }

    snd_attr.port_num = 1;
    snd_attr.port_attr[0].out_port = inst->snd_attr.port_attr[0].out_port;
    snd_attr.channels = inst->snd_attr.channels;
    snd_attr.bit_depth = inst->snd_attr.bit_depth;
    snd_attr.sample_rate = inst->snd_attr.sample_rate;

    sample_ao_set_i2s_attr(&snd_attr);

    ret = uapi_snd_open(&inst->h_snd, inst->snd_id, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_open, ret);
        return ret;
    }

    ret = uapi_snd_set_aef_profile(inst->h_snd, inst->aef_profile);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_profile, ret);
        goto out;
    }

    ret = uapi_snd_set_aef_bypass(inst->h_snd, UAPI_SND_OUT_PORT_ALL, AEF_BYPASS_DEFAULT);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_bypass, ret);
        goto out;
    }

    ret = uapi_snd_set_track_mode(inst->h_snd, snd_attr.port_attr[0].out_port, inst->track_mode);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_bypass, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_snd_close(inst->h_snd);
    return ret;
}

static td_void sample_ao_close_track(sample_ao_track_inst *inst)
{
    td_s32 ret;

    if (inst->h_track == 0) {
        return;
    }

    ret = uapi_snd_destroy_track(inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_destroy_track, ret);
    }

    inst->h_track = 0;
}


static td_s32 sample_ao_open_track(td_handle h_snd, sample_ao_track_inst *inst)
{
    td_s32 ret;
    uapi_snd_track_attr track_attr;

    uapi_snd_gain gain = {
        .integer = inst->volume,
        .decimal = 0,
    };

    inst->h_track = 0;
    ret = uapi_snd_get_track_default_attr(&track_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_track_default_attr, ret);
        return ret;
    }

    ret = uapi_snd_create_track(&inst->h_track, h_snd, &track_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_create_track, ret);
        return ret;
    }

    ret = uapi_snd_set_track_volume(inst->h_track, &gain, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_track_volume, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_snd_destroy_track(inst->h_track);
    return ret;
}

static td_void sample_ao_track_stop_inst(sample_ao_track_inst *inst)
{
    td_s32 ret;

    if (inst->task != TD_NULL) {
        inst->task_active = TD_FALSE;
        athread_exit(inst->task);
        inst->task = TD_NULL;
    }

    ret = uapi_snd_track_stop(inst->h_track);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_stop, ret);
    }
}

static td_s32 sample_ao_track_start_inst(sample_ao_track_inst *inst, td_bool thread_enable)
{
    td_s32 ret;
    athread_attr attr;
    uapi_audio_scene_config asc = {
        .scene_type = UAPI_AUDIO_SCENE_LOCAL_TONE,
        .scene_id = 0,
    };

    if (inst->h_adp != 0) {
        ret = uapi_audio_set_audio_scene(inst->h_adp, &asc);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_audio_set_audio_scene, ret);
        }
    }

    ret = uapi_snd_track_start(inst->h_track, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_track_start, ret);
    }

    if (thread_enable == TD_FALSE) {
        return EXT_SUCCESS;
    }

    inst->task_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_ao_track";

    ret = athread_create(&inst->task, sample_ao_track_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        goto out;
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_snd_track_stop(inst->h_track);
    return ret;
}

static td_void sample_ao_track_close_inst(sample_ao_track_inst *inst)
{
    sample_ao_close_adp(inst);
    sample_ao_close_track(inst);
}

static td_s32 sample_ao_track_open_inst(td_handle h_snd, sample_ao_track_inst *inst)
{
    td_s32 ret;

    /*
     * 数据流向 adp --> track(snd)
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_ao_open_track(h_snd, inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_track, ret);
        return ret;
    }

    ret = sample_ao_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_adp, ret);
        goto out0;
    }

    return EXT_SUCCESS;
out0:
    sample_ao_close_track(inst);
    return ret;
}

static td_void sample_ao_inst_init(sample_ao_inst *inst)
{
    inst->aef_profile = UAPI_SND_AEF_PROFILE_MUSIC; /* aef music scene in default */
    inst->thread_enable = TD_TRUE;
    inst->snd_attr.port_attr[0].out_port = SND_OUT_PORT_DEFAULT;
    inst->snd_attr.channels = SND_OUT_CHANNEL_DEF;
    inst->snd_attr.bit_depth = SND_OUT_BIT_DEPTH_DEF;
    inst->snd_attr.sample_rate = SND_OUT_SAMPLE_RATE_DEF;
    inst->track_mode = TRACK_MODE_DEFAULT;
}

static td_void sample_ao_close_inst(sample_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->track_num; i++) {
        sample_ao_track_close_inst(&inst->track_inst[i]);
    }

    sample_ao_close_snd(inst);
    sample_ao_sys_deinit();
}

static td_s32 sample_ao_open_inst(sample_ao_inst *inst)
{
    td_s32 ret;
    td_u32 i;

    ret = sample_ao_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_sys_init, ret);
        return ret;
    }

    ret = sample_ao_open_snd(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_snd, ret);
        goto out0;
    }

    sample_gui_aef_add_handle(GUI_AEF_AO, inst->h_snd);

    for (i = 0; i < inst->track_num; i++) {
        ret = sample_ao_track_open_inst(inst->h_snd, &inst->track_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_ao_track_open_inst, ret);
            goto out0;
        }
    }

    return EXT_SUCCESS;
out0:
    sample_ao_close_inst(inst);
    return ret;
}

static td_void sample_ao_stop_inst(sample_ao_inst *inst)
{
    td_s32 ret;
    td_u32 i;

    ret = uapi_snd_set_enable(inst->h_snd, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_enable, ret);
        return;
    }

    for (i = 0; i < inst->track_num; i++) {
        sample_ao_track_stop_inst(&inst->track_inst[i]);
    }
}

static td_s32 sample_ao_start_inst(sample_ao_inst *inst)
{
    td_s32 ret;
    td_u32 i;

    for (i = 0; i < inst->track_num; i++) {
        ret = sample_ao_track_start_inst(&inst->track_inst[i], inst->thread_enable);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_ao_track_start_inst, ret);
            sample_ao_stop_inst(inst);
            return ret;
        }
    }

    ret = uapi_snd_set_enable(inst->h_snd, TD_TRUE);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_enable, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_ao_track_close_file(sample_ao_track_inst *inst)
{
    if (inst->h_file != TD_NULL) {
        sap_fclose(inst->h_file);
        inst->h_file = TD_NULL;
    }
}

static td_s32 sample_ao_track_open_file(sample_ao_track_inst *inst)
{
    if (inst->in_stream == TD_NULL) {
        return EXT_FAILURE;
    }

    if (strcmp(inst->in_stream, "array") == 0) {
        inst->h_file = TD_NULL;
        inst->read_frame = sample_ao_track_read_array;
        sap_alert_log_info("sample_ao read data from array.");
    } else if (strcmp(inst->in_stream, "flash") == 0 || strcmp(inst->in_stream, "null") == 0) {
        sap_alert_log_h32(FPGA_FLASH_PCM_DATA_ADDR);
        sap_alert_log_h32(FPGA_FLASH_PCM_SIZE);
        data_info_init(&inst->data_info, FPGA_FLASH_PCM_DATA_ADDR, FPGA_FLASH_PCM_SIZE);
        circ_buf_update_write_pos(&inst->data_info.cb, inst->data_info.cb.size - sizeof(td_s16));

        inst->h_file = TD_NULL;
        inst->read_frame = sample_ao_track_read_flash;
        sap_alert_log_info("sample_ao read data from flash.");
    } else {
        inst->read_frame = sample_ao_track_read_file;
        inst->h_file = sample_audio_open_input_stream(inst->in_stream);
        if (inst->h_file == TD_NULL) {
            sap_err_log_info("open file error!");
            sap_err_log_str(inst->in_stream);
            return EXT_FAILURE;
        }
        sap_alert_log_info("sample_ao read data from file.");
    }

    sap_alert_log_u32(inst->pcm_format.sample_rate);
    sap_alert_log_u32(inst->pcm_format.channels);
    sap_alert_log_u32(inst->pcm_format.bit_depth);

    return EXT_SUCCESS;
}

static td_void sample_ao_close_file(sample_ao_inst *inst)
{
    td_u32 i;

    for (i = 0; i < inst->track_num; i++) {
        sample_ao_track_close_file(&inst->track_inst[i]);
    }
}

static td_s32 sample_ao_open_file(sample_ao_inst *inst)
{
    td_u32 i;
    td_s32 ret;

    for (i = 0; i < inst->track_num; i++) {
        ret = sample_ao_track_open_file(&inst->track_inst[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_ao_track_open_file, ret);
            sample_ao_close_file(inst);
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_void sample_ao_usage(const td_char *name)
{
    sap_printf("play pcm stream\n");
    sap_printf("usage:  %s pcm_file0 volume0 sample_rate0 bit_width0 channels0 "
        "pcm_file1 volume1 sample_rate1 bit_width1 channels1 ...\n",
        name);
    sap_printf("set ao high precision gain\n");
    sap_printf("  %s ao_vol integer decimal\n", name);
    sap_printf("set track high precision gain\n");
    sap_printf("  %s track_vol track_handle integer decimal\n", name);

    sap_printf("examples:\n");
    sap_printf("play pcm stream in file system\n");
    sap_printf("  %s /mnt/data/test0.pcm 0 48000 16 2 /mnt/data/test1.pcm 100 44100 16 1\n", name);
    sap_printf("play pcm stream in fix address\n");
    sap_printf("  %s null 0 48000 16 2\n", name);
    sap_printf("play pcm stream in array\n");
    sap_printf("  %s array -6 16000 16 1\n", name);

    sap_printf("set ao gain to -6dB\n");
    sap_printf("  %s ao_vol -6 0\n", name);
    sap_printf("set ao gain to -6.5dB\n");
    sap_printf("  %s ao_vol -6 -500\n", name);
    sap_printf("set track gain to -2.125dB\n");
    sap_printf("  %s track_vol 0x6d000008 -2 -125\n", name);
}

static inline td_void sample_ao_parse_pcm_format(const sample_ao_track_arg *arg, uapi_audio_pcm_format *pcm_format)
{
    pcm_format->sample_rate = (td_u32)strtoul(arg->sample_rate, TD_NULL, 0);
    pcm_format->bit_depth = (td_u32)strtoul(arg->bit_depth, TD_NULL, 0);
    pcm_format->channels = (td_u32)strtoul(arg->channels, TD_NULL, 0);
    /* 100 --> 10ms pcm data */
    pcm_format->sample_per_frame = pcm_format->sample_rate / 100;
}

static td_s32 sample_ao_parse_cmd_line(td_s32 argc, td_char *argv[], sample_ao_inst *inst)
{
    td_u32 i;
    sample_ao_arg *arg = (sample_ao_arg *)argv;
    const td_u32 track_arg_size = (td_u32)(sizeof(sample_ao_track_arg) / sizeof(td_char *));
    const td_u32 track_num = (td_u32)(argc - 1) / track_arg_size;
    if (argc <= 1 || track_num <= 0) {
        return EXT_FAILURE;
    }

    inst->snd_id = UAPI_SND_0;
    inst->track_num = track_num;
    for (i = 0; i < track_num; i++) {
        inst->track_inst[i].in_stream = arg->track_arg[i].in_stream;
        inst->track_inst[i].volume = (td_s32)strtol(arg->track_arg[i].volume, TD_NULL, 0);
        sample_ao_parse_pcm_format(&arg->track_arg[i], &inst->track_inst[i].pcm_format);
        inst->track_inst[i].frame_size = calc_pcm_frame_size(&inst->track_inst[i].pcm_format);
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ao_vol_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_handle sound = 0;
    uapi_snd_attr snd_attr;
    uapi_snd_gain gain = {0, 0};
    sample_ao_vol_arg *arg = (sample_ao_vol_arg *)argv;
    if (argc != (sizeof(sample_ao_vol_arg) / sizeof(td_char *))) {
        sample_ao_usage(argv[0]);
        return EXT_FAILURE;
    }

    gain.integer = (td_s32)strtol(arg->integer, TD_NULL, 0);
    gain.decimal = (td_s32)strtol(arg->decimal, TD_NULL, 0);

    ret = uapi_snd_get_default_attr(UAPI_SND_0, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_default_attr, ret);
        return ret;
    }

    ret = uapi_snd_open(&sound, UAPI_SND_0, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_open, ret);
        return ret;
    }

    ret = uapi_snd_set_volume(sound, snd_attr.port_attr[0].out_port, &gain, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_volume, ret);
    }

    ret = uapi_snd_close(sound);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_close, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ao_track_vol_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_handle track;
    uapi_snd_gain gain = {0, 0};
    sample_ao_track_vol_arg *arg = (sample_ao_track_vol_arg *)argv;
    if (argc != (sizeof(sample_ao_track_vol_arg) / sizeof(td_char *))) {
        sample_ao_usage(argv[0]);
        return EXT_FAILURE;
    }

    track = (td_u32)strtol(arg->track, TD_NULL, 16); /* 16进制 */
    gain.integer = (td_s32)strtol(arg->integer, TD_NULL, 0);
    gain.decimal = (td_s32)strtol(arg->decimal, TD_NULL, 0);

    ret = uapi_snd_set_track_volume(track, &gain, TD_NULL);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_track_volume, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_ao_i2s_tdm_test_parse_cmd(td_s32 argc, td_char *argv[], sample_ao_inst *inst)
{
    if (argc != I2S_TDM_TEST_MAX_ARGC) {
        sap_printf("i2s tdm test: invalid parameters\n");
        sap_printf("examples: %s i2s_tdm /user/test0.pcm 0 48000 32 4\n", argv[0]);
        return EXT_FAILURE;
    }

#if defined(AUDIO_AW85825) && defined(SAP_SOCMN2)
    inst->snd_attr.port_attr[0].out_port = I2S_TDM_TEST_I2S_ID; /* 执行i2s_tdm测试时才修改snd_attr */
    inst->snd_attr.channels = UAPI_AUDIO_CHANNEL_4;
    inst->snd_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_32;
    inst->snd_attr.sample_rate = UAPI_AUDIO_SAMPLE_RATE_48K;

    td_char *argv_new[6] = {argv[0], };
    for (int i = (I2S_TDM_TEST_INDEX+1); i < argc; i++) {
        argv_new[i-I2S_TDM_TEST_INDEX] = argv[i];
    }
    return sample_ao_parse_cmd_line(6, argv_new, inst); /* 执行i2s_tdm测试时sample_ao播放参数共6个 */
#else
    sap_printf("i2s tdm test need support aw85825 vendor driver\n");
    sap_unused(inst);
    return EXT_FAILURE;
#endif
}

static td_s32 sample_ao_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_ao_inst *inst = sample_ao_alloc_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ao_alloc_inst failed");
        return EXT_FAILURE;
    }

    sample_ao_inst_init(inst);

    if (strcmp(argv[1], "i2s_tdm") == 0) {
        ret = sample_ao_i2s_tdm_test_parse_cmd(argc, argv, inst);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_ao_i2s_tdm_test_parse_cmd, ret);
            goto out0;
        }
    } else {
        ret = sample_ao_parse_cmd_line(argc, argv, inst);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_ao_parse_cmd_line, ret);
            goto out0;
        }
    }

    ret = sample_ao_open_file(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_file, ret);
        goto out0;
    }

    ret = sample_ao_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_inst, ret);
        goto out1;
    }

    ret = sample_ao_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_start_inst, ret);
        goto out2;
    }

    sample_ao_show_inst();
    return EXT_SUCCESS;
out2:
    sample_ao_close_inst(inst);
out1:
    sample_ao_close_file(inst);
out0:
    sample_ao_free_inst(inst);
    return ret;
}

static td_s32 sample_ao_exit(td_void)
{
    sample_ao_inst *inst = sample_ao_get_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ao_get_inst failed");
        return EXT_FAILURE;
    }

    sample_gui_aef_delete_handle(inst->h_snd);
    sample_ao_stop_inst(inst);
    sample_ao_close_inst(inst);
    sample_ao_close_file(inst);
    sample_ao_free_inst(inst);
    sample_ao_show_inst();
    return EXT_SUCCESS;
}

td_s32 sample_ao(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_ao_usage(argv[0]);
        return EXT_SUCCESS;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_ao_exit();
    } else if (strcmp(argv[1], "ao_vol") == 0) {
        return sample_ao_vol_entry(argc, argv);
    } else if (strcmp(argv[1], "track_vol") == 0) {
        return sample_ao_track_vol_entry(argc, argv);
    } else {
        return sample_ao_entry(argc, argv);
    }
}

static td_void sample_ao_player_inst_init_cat1(sample_ao_inst *inst)
{
    /* not CAT1 output use default pcm format */
    if (inst->snd_attr.port_attr[0].out_port != CAT1_OUT_PORT_DEFAULT) {
        return;
    }

    /* pcm format for CAT1 */
    inst->snd_attr.channels = SIO_CAT1_I2S_CHANNEL_DEF;
    inst->snd_attr.sample_rate = SIO_CAT1_I2S_SAMPLE_RATE_DEF;
    inst->aef_profile = UAPI_SND_AEF_PROFILE_NONE;
}

static td_void sample_ao_player_inst_init_test_codec(sample_ao_inst *inst, td_bool codec_test)
{
    if (codec_test == TD_FALSE) {
        return;
    }

    /* pcm format for test codec */
    inst->snd_attr.channels = UAPI_AUDIO_CHANNEL_2;
    inst->snd_attr.bit_depth = UAPI_AUDIO_BIT_DEPTH_24;
    inst->snd_attr.sample_rate = UAPI_AUDIO_SAMPLE_RATE_48K;
}

td_s32 sample_ao_player_open(sample_escast_arg *escast_arg, uapi_snd snd_id, td_bool codec_test)
{
    td_s32 ret;
    sample_ao_inst *inst = sample_ao_alloc_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ao_alloc_inst failed");
        return EXT_FAILURE;
    }

    if (escast_arg == TD_NULL || escast_arg->player == TD_NULL || escast_arg->track == TD_NULL ||
        escast_arg->in_arg == TD_NULL || escast_arg->snd_attr == TD_NULL) {
        return EXT_FAILURE;
    }

    sample_ao_inst_init(inst);

    *escast_arg->player = 0;

    inst->snd_id = snd_id;
    inst->track_num = 1;
    inst->thread_enable = TD_FALSE;
    inst->track_mode = escast_arg->track_mode;
    inst->track_inst[0].run_dsp_loopback = escast_arg->run_dsp_loopback;

    /* Set track input PCM format */
    (td_void)memcpy_s(&inst->track_inst[0].pcm_format, sizeof(uapi_audio_pcm_format),
                      &escast_arg->in_arg->pcm_format, sizeof(escast_arg->in_arg->pcm_format));

    /* Set track output PCM format(sound output format) */
    (td_void)memcpy_s(&inst->snd_attr, sizeof(inst->snd_attr), escast_arg->snd_attr, sizeof(*escast_arg->snd_attr));

    /* Set aef profile type */
    inst->aef_profile = audio_scene_to_aef_profile(escast_arg->audio_scene);

    sample_ao_player_inst_init_cat1(inst); /* Set pcm/i2s attr for cat1 */
    sample_ao_player_inst_init_test_codec(inst, codec_test); /* set pcm attr for test codec */

    ret = sample_ao_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_open_inst, ret);
        goto out0;
    }

    ret = sample_ao_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_start_inst, ret);
        goto out1;
    }

    *escast_arg->track = inst->track_inst[0].h_track;
    *escast_arg->player = inst->track_inst[0].h_adp;
    *escast_arg->snd = inst->h_snd;

    sample_ao_show_inst();
    return EXT_SUCCESS;
out1:
    sample_ao_close_inst(inst);
out0:
    sample_ao_free_inst(inst);
    return ret;
}

td_void sample_ao_player_close(td_void)
{
    sample_ao_inst *inst = sample_ao_get_inst();
    if (inst == TD_NULL) {
        sap_err_log_info("sample_ao_get_inst failed");
        return;
    }

    sample_ao_stop_inst(inst);
    sample_ao_close_inst(inst);
    sample_ao_free_inst(inst);
    sample_ao_show_inst();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
