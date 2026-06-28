/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2026. All rights reserved.
 * Description: vad_feed sample
 * Author: audio
 * Create: 2025-12-01
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "thread_os.h"
#include "osal_list.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_sea.h"
#include "sample_audio_api.h"
#include "sample_audio_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SAMPLE_SQURT(a) ((a) * (a))
#define ENERGY_THRESHOLD_MAX 0x2FFF /* 经验值 */
#define TMP_SAVE_FRAME 16384 /* mp3默认bit_rate为64k，2s对应的字节数为16384 */
#define ANEC_HANDLE_COUNT 2

typedef struct sample_vad_feed_aenc_inst sample_vad_feed_aenc_inst;

struct sample_vad_feed_aenc_inst {
    athread_handle task;
    td_bool task_enable;

    td_bool (*energy_check)(const td_u8 *, const td_u32);
    td_bool (*task_proc)(sample_vad_feed_aenc_inst *inst, uapi_stream_buf stream);

    const td_char *file_name;
    FILE *h_file;

    td_u32 bit_rate;
    td_handle h_adp;
    td_handle h_aenc;
    data_info data_caching;
    td_u32 data_caching_size;
    uapi_acodec_id acodec_id;
};

typedef struct {
    td_bool task_enable;
    td_bool mute;

    sample_vad_feed_aenc_inst aenc_inst[ANEC_HANDLE_COUNT];

    td_handle h_ai;
    td_handle h_sea;
    uapi_ai_port ai_port;
    uapi_ai_gain volume;
    uapi_audio_pcm_format pcm_format;
    struct osal_list_head node;
} sample_vad_feed_inst;

static OSAL_LIST_HEAD(g_sample_vad_feed_list);
static td_bool vad_enable = TD_TRUE;

static td_bool sample_vad_feed_data_caching_init(sample_vad_feed_aenc_inst *inst)
{
    td_void *caching_buf = malloc(inst->data_caching_size);
    if (caching_buf == TD_NULL) {
        sap_err_log_info("caching_buf malloc failed!");
        return EXT_FAILURE;
    }

    data_info_init(&inst->data_caching, (td_u32)(td_uintptr_t)caching_buf, inst->data_caching_size);

    return EXT_SUCCESS;
}

static td_void sample_vad_feed_data_caching_deinit(sample_vad_feed_aenc_inst *inst)
{
    if (inst->data_caching.buf != TD_NULL) {
        free(inst->data_caching.buf);
    }

    (td_void)memset_s(&inst->data_caching, sizeof(inst->data_caching), 0, sizeof(data_info));
}

static sample_vad_feed_inst *sample_vad_feed_alloc_inst(td_void)
{
    sample_vad_feed_inst *inst = (sample_vad_feed_inst *)malloc(sizeof(sample_vad_feed_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    (td_void)memset_s(inst, sizeof(*inst), 0, sizeof(sample_vad_feed_inst));

    osal_list_add_tail(&inst->node, &g_sample_vad_feed_list);
    return inst;
}

static td_void sample_vad_feed_free_inst(sample_vad_feed_inst *inst)
{
    td_u32 i;
    if (inst == TD_NULL) {
        return;
    }

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        sample_vad_feed_data_caching_deinit(&inst->aenc_inst[i]);
    }

    osal_list_del(&inst->node);
    free(inst);
}

static td_void sample_vad_feed_close_file(sample_vad_feed_inst *inst)
{
    td_u32 i;

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        if (inst->aenc_inst[i].h_file != TD_NULL) {
            sap_fclose(inst->aenc_inst[i].h_file);
            inst->aenc_inst[i].h_file = TD_NULL;
        }
    }
}

static td_u32 sample_vad_feed_write_file(FILE *h_file_out, const td_u8 *from, td_u32 len)
{
    td_u32 length;

    length = sap_fwrite(from, 1, len, h_file_out);
    return length;
}

static sample_vad_feed_inst *sample_vad_feed_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_vad_feed_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_vad_feed_list, sample_vad_feed_inst, node);
}

static td_bool sample_vad_feed_energy_check_16bit(const td_u8 *from, const td_u32 size)
{
    td_u32 i;
    td_s16 *in_buf = (td_s16 *)from;
    const td_u32 point_count = (size >> 1); /* 一次性取16位，每次加载1个点 */
    td_u64 square_sum, frame_energy;
    square_sum = 0;

    if (point_count == 0) {
        return TD_FALSE;
    }

    for (i = 0; i < point_count; i++) {
        square_sum += SAMPLE_SQURT(*(in_buf + i));
    }

    frame_energy = square_sum / point_count;
    return (frame_energy > ENERGY_THRESHOLD_MAX);
}

static td_bool sample_vad_feed_energy_check_24bit(const td_u8 *from, const td_u32 size)
{
    td_u32 i, magnitude;
    td_s32 *in_buf = (td_s32 *)from;
    const td_u32 point_count = (size >> 2); /* 一次性取32位，每次加载1个点 */
    td_u64 square_sum, frame_energy;
    square_sum = 0;

    if (point_count == 0) {
        return TD_FALSE;
    }

    for (i = 0; i < point_count; i++) {
        magnitude = (td_u32)abs(*(in_buf + i));
        magnitude = magnitude >> UAPI_AUDIO_BIT_DEPTH_8; /* 32位中高24位为有效数字 */
        square_sum += SAMPLE_SQURT(magnitude);
    }

    frame_energy = square_sum / point_count;
    return (frame_energy > ENERGY_THRESHOLD_MAX);
}

static td_bool sample_vad_feed_energy_check_32bit(const td_u8 *from, const td_u32 size)
{
    td_u32 i;
    td_s32 *in_buf = (td_s32 *)from;
    const td_u32 point_count = (size >> 2); /* 一次性取32位，每次加载1个点 */
    td_u64 square_sum, frame_energy;
    square_sum = 0;

    if (point_count == 0) {
        return TD_FALSE;
    }

    for (i = 0; i < point_count; i++) {
        square_sum += SAMPLE_SQURT(*(in_buf + i));
    }

    frame_energy = square_sum / point_count;
    return (frame_energy > ENERGY_THRESHOLD_MAX);
}

static td_void sample_vad_feed_data_caching_proc(data_info *data_info, uapi_stream_buf *stream)
{
    circ_buf *cb = &data_info->cb;

    td_u32 free_size = circ_buf_query_free(cb);
    if (stream->size >= free_size) { /* when caching buf full, move rptr automatically */
        /* cicle buf must have 4 Bytes left */
        circ_buf_update_read_pos(cb, stream->size - free_size + sizeof(td_u32));
    }

    (td_void)circ_buf_write(cb, (const td_u8 *)stream->data, stream->size);
}

static td_void sample_vad_feed_caching_move_file(sample_vad_feed_aenc_inst *inst)
{
    circ_buf *cb = &inst->data_caching.cb;
    td_u8 *tmp_buf = TD_NULL;
    td_u32 len;
    td_u32 data_size;

    if (inst->data_caching.buf == TD_NULL) {
        return;
    }

    data_size = circ_buf_query_busy(cb);
    if (data_size == 0) {
        return;
    }

    tmp_buf = (td_u8 *)malloc(data_size);
    if (tmp_buf == TD_NULL) {
        sap_err_log_info("FATAL Error: tmp_buf malloc error");
        return;
    }

    if (circ_buf_read(cb, tmp_buf, data_size) != data_size) {
        sap_err_log_info("Fatal Error: circ_buf_read error");
        free(tmp_buf);
        return;
    }

    len = sample_vad_feed_write_file(inst->h_file, (const td_u8 *)tmp_buf, data_size);
    if (len == 0) {
        sap_err_log_info("Fatal Erorr: the data in circ_buf move to file error");
    }

    free(tmp_buf);
}

static td_bool sample_vad_feed_nomal_proc(sample_vad_feed_aenc_inst *inst, uapi_stream_buf stream)
{
    td_u32 len = 0;

    if (inst->h_file != TD_NULL) {
        if (vad_enable == TD_FALSE) {
            sample_vad_feed_data_caching_proc(&inst->data_caching, &stream);
        } else {
            sample_vad_feed_caching_move_file(inst);
            len = sample_vad_feed_write_file(inst->h_file, (const td_u8 *)stream.data, stream.size);
            if (len == 0) {
                inst->task = TD_FALSE;
                return EXT_FAILURE;
            }
        }
    }

    return EXT_SUCCESS;
}

static td_bool sample_vad_feed_pcm_proc(sample_vad_feed_aenc_inst *inst, uapi_stream_buf stream)
{
    td_u32 len = 0;

    vad_enable = inst->energy_check((const td_u8 *)stream.data, stream.size);  /* 需要判断16,24,32bit */
    if (inst->h_file != TD_NULL && vad_enable == TD_TRUE) {
        len = sample_vad_feed_write_file(inst->h_file, (const td_u8 *)stream.data, stream.size);
        if (len == 0) {
            inst->task_enable = TD_FALSE;
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}
static td_void sample_vad_feed_thread(td_void *args)
{
    td_s32 ret;
    uapi_stream_buf stream;

    sample_vad_feed_aenc_inst *inst = (sample_vad_feed_aenc_inst *)args;
    if (inst == TD_NULL) {
        sap_err_log_info("vad_feed_aenc_inst is null");
        return;
    }
    
    while (inst->task_enable) {
        ret = uapi_adp_acquire_stream(inst->h_adp, &stream);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        ret = inst->task_proc(inst, stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(task_proc, ret);
            break;
        }

        ret = uapi_adp_release_stream(inst->h_adp, &stream);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_release_stream, ret);
        }
    }

    sample_vad_feed_data_caching_deinit(inst);
    athread_set_exit(inst->task, TD_TRUE);
}

static td_s32 sample_vad_feed_sys_init(td_void)
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

    ret = uapi_sea_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_init, ret);
        goto out1;
    }

    ret = uapi_sea_load_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_load_engine, ret);
        goto out2;
    }

    ret = uapi_aenc_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_init, ret);
        goto out3;
    }

    return EXT_SUCCESS;
out3:
    (td_void)uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
out2:
    (td_void)uapi_sea_deinit();
out1:
    (td_void)uapi_ai_deinit();
out0:
    (td_void)uapi_adp_deinit();
    return ret;
}

static td_void sample_vad_feed_sys_deinit(td_void)
{
    td_s32 ret;

    ret = uapi_aenc_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_aenc_deinit, ret);
    }

    ret = uapi_sea_unload_engine(UAPI_SEA_LIB_SEE, "imedia_2mic");
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_unload_engine, ret);
    }

    ret = uapi_sea_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_deinit, ret);
    }

    ret = uapi_ai_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_deinit, ret);
    }

    ret = uapi_adp_deinit();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_adp_deinit, ret);
    }
}

static td_void sample_vad_feed_close_adp(sample_vad_feed_inst *inst)
{
    td_u32 i;
    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        if (inst->aenc_inst[i].h_adp != 0) {
            uapi_adp_destroy(inst->aenc_inst[i].h_adp);
            inst->aenc_inst[i].h_adp = 0;
        }
    }
}

static td_s32 sample_vad_feed_open_adp(sample_vad_feed_inst *inst)
{
    td_s32 ret;
    td_u32 i;
    uapi_adp_attr adp_attr;

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        inst->aenc_inst[i].h_adp = 0;

        ret = uapi_adp_get_def_attr(&adp_attr);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_get_def_attr, ret);
            return ret;
        }

        ret = uapi_adp_create(&inst->aenc_inst[i].h_adp, &adp_attr);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_adp_create, ret);
            return ret;
        }
    }

    return EXT_SUCCESS;
}

static td_void sample_vad_feed_close_aenc(sample_vad_feed_inst *inst)
{
    td_s32 ret;
    td_u32 i;

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        if (inst->aenc_inst[i].h_aenc != 0) {
        ret = uapi_aenc_detach_output(inst->aenc_inst[i].h_aenc, inst->aenc_inst[i].h_adp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_detach_output, ret);
        }

        ret = uapi_aenc_destroy(inst->aenc_inst[i].h_aenc);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_destroy, ret);
        }

        inst->aenc_inst[i].h_aenc = 0;
        }
    }
}

static td_s32 sample_vad_feed_open_aenc(sample_vad_feed_inst *inst)
{
    td_s32 ret;
    td_u32 i;
    uapi_aenc_attr aenc_attr;

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        inst->aenc_inst[i].h_aenc = 0;

        ret = uapi_aenc_create(&inst->aenc_inst[i].h_aenc);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_create, ret);
            return ret;
        }

        ret = uapi_aenc_get_attr(inst->aenc_inst[i].h_aenc, &aenc_attr);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_get_attr, ret);
            goto out;
        }

        aenc_attr.codec_id = inst->aenc_inst[i].acodec_id;
        aenc_attr.param.interleaved = TD_TRUE;
        aenc_attr.param.channels = inst->pcm_format.channels;
        aenc_attr.param.bit_depth = inst->pcm_format.bit_depth;
        aenc_attr.param.sample_rate = inst->pcm_format.sample_rate;
        aenc_attr.param.samples_per_frame = inst->pcm_format.sample_per_frame;
        aenc_attr.param.private_data = TD_NULL;
        aenc_attr.param.private_data_size = 0;

        ret = uapi_aenc_set_attr(inst->aenc_inst[i].h_aenc, &aenc_attr);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_set_attr, ret);
            goto out;
        }

        ret = uapi_aenc_attach_output(inst->aenc_inst[i].h_aenc, inst->aenc_inst[i].h_adp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_attach_output, ret);
            goto out;
        }

        if (inst->aenc_inst[i].bit_rate != 0) {
            ret = uapi_aenc_set_param(inst->aenc_inst[i].h_aenc, UAPI_AENC_PARAM_BIT_RATE,
            &inst->aenc_inst[i].bit_rate, sizeof(inst->aenc_inst[i].bit_rate));
            if (ret != EXT_SUCCESS) {
                sap_err_log_fun(uapi_aenc_set_param, ret);
                goto out;
            }
        }
    }

    return EXT_SUCCESS;

out:
    (td_void)sample_vad_feed_close_aenc;
    return ret;
}

static td_void sample_vad_feed_close_sea(sample_vad_feed_inst *inst)
{
    td_s32 ret;
    td_u32 i;

    if (inst->h_sea == 0) {
        return;
    }

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        ret = uapi_sea_detach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->aenc_inst[i].h_aenc);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_detach_output, ret);
        }
    }

    ret = uapi_sea_destroy(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_destroy, ret);
    }

    inst->h_sea = 0;
}

static td_s32 sample_vad_feed_open_sea(sample_vad_feed_inst *inst)
{
    td_s32 ret;
    td_u32 i;
    uapi_sea_attr sea_attr;
    uapi_sea_eng_sel sea_eng;
    uapi_sea_afe_attr afe_attr = {
        .type = UAPI_SEA_AFE_SEE,
        .mode = UAPI_SEA_NORM_MODE,
        .u.see.options = UAPI_SEA_AFE_OPT_AGC,
    };

    inst->h_sea = 0;

    /* Create SEA instance */
    (td_void)memset_s(&sea_eng, sizeof(sea_eng), 0, sizeof(sea_eng));
    sea_eng.afe.type = UAPI_SEA_AFE_SEE;
    sea_eng.afe.lib_id = UAPI_SEA_LIB_SEE;
    ret = uapi_sea_get_default_attr(&sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_get_default_attr, ret);
        return ret;
    }

    sea_attr.in_pcm.bit_depth = inst->pcm_format.bit_depth;
    sea_attr.in_pcm.channels = inst->pcm_format.channels;
    sea_attr.in_pcm.sample_rate = inst->pcm_format.sample_rate;
    sea_attr.in_pcm.sample_per_frame = inst->pcm_format.sample_per_frame;
    sea_attr.ref_pcm.channels = 0;
    ret = uapi_sea_create(&inst->h_sea, &sea_eng, &sea_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_create, ret);
        return ret;
    }

    ret = uapi_sea_set_afe_attr(inst->h_sea, UAPI_SEA_AFE_SEE, &afe_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_set_afe_attr, ret);
        goto out;
    }
    
    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        ret = uapi_sea_attach_output(inst->h_sea, UAPI_SEA_OUTPUT_ASR_SRC, inst->aenc_inst[i].h_aenc);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_attach_output, ret);
            goto out;
        }
    }

    return EXT_SUCCESS;
out:
    (td_void)sample_vad_feed_close_sea;
    return ret;
}

static td_void sample_vad_feed_close_ai(sample_vad_feed_inst *inst)
{
    td_s32 ret;

    if (inst->h_ai == 0) {
        return;
    }

    ret = uapi_ai_detach_output(inst->h_ai, inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_detach_output, ret);
    }

    ret = uapi_ai_close(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_close, ret);
    }

    inst->h_ai = 0;
}

static td_void sample_vad_feed_set_port_attr(const sample_vad_feed_inst *inst, uapi_ai_attr *ai_attr)
{
    td_s32 ret;

    ret = memcpy_s(&ai_attr->pcm_attr, sizeof(ai_attr->pcm_attr), &inst->pcm_format,
        sizeof(inst->pcm_format));
    if (ret != EOK) {
        return;
    }

    ai_attr->ref_attr.enable = TD_FALSE;

    switch (inst->ai_port) {
        case UAPI_AI_PORT_PDM0:
        case UAPI_AI_PORT_PDM1:
            ai_attr->port_attr.pdm.i2s_attr.channels = inst->pcm_format.channels;
            ai_attr->port_attr.pdm.i2s_attr.bit_depth = inst->pcm_format.bit_depth;
            break;
        case UAPI_AI_PORT_ADC0:
        case UAPI_AI_PORT_I2S0:
        case UAPI_AI_PORT_I2S1:
            ai_attr->port_attr.i2s.i2s_attr.channels = inst->pcm_format.channels;
            ai_attr->port_attr.i2s.i2s_attr.bit_depth = inst->pcm_format.bit_depth;
            break;
        default:
            break;
    }
}

static td_s32 sample_vad_feed_open_ai(sample_vad_feed_inst *inst)
{
    td_s32 ret;
    uapi_ai_attr ai_attr;

    ret = uapi_ai_get_default_attr(inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_get_default_attr, ret);
        return ret;
    }

    sample_vad_feed_set_port_attr(inst, &ai_attr);

    ret = uapi_ai_open(&inst->h_ai, inst->ai_port, &ai_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_open, ret);
        return ret;
    }

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

    if (inst->h_sea != 0) {
        ret = uapi_ai_attach_output(inst->h_ai, inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_ai_attach_output, ret);
            goto out;
        }
    }

    return EXT_SUCCESS;
out:
    (td_void)uapi_ai_close(inst->h_ai);
    return ret;
}

static td_void sample_vad_feed_inst_init(sample_vad_feed_inst *inst)
{
    inst->ai_port = UAPI_AI_PORT_MAX;
    vad_enable = TD_TRUE;

    /* ai/aenc的默认属性 */
    inst->aenc_inst[0].acodec_id = UAPI_ACODEC_ID_MP3;
    inst->aenc_inst[0].file_name = TD_NULL;
    inst->aenc_inst[0].bit_rate = 0;
    inst->aenc_inst[0].data_caching_size = TMP_SAVE_FRAME;
    inst->aenc_inst[1].acodec_id = UAPI_ACODEC_ID_PCM;
    inst->aenc_inst[1].file_name = TD_NULL;
    inst->aenc_inst[1].bit_rate = 0;
    inst->aenc_inst[0].data_caching_size = TMP_SAVE_FRAME;
    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_16K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */
    inst->volume.integer = AI_VOLUME_DEFAULT;
    inst->volume.decimal = 0;
}

static td_void sample_vad_feed_close_inst(sample_vad_feed_inst *inst)
{
    sample_vad_feed_close_ai(inst);
    sample_vad_feed_close_sea(inst);
    sample_vad_feed_close_aenc(inst);
    sample_vad_feed_close_adp(inst);
    sample_vad_feed_sys_deinit();
}

static td_s32 sample_vad_feed_open_inst(sample_vad_feed_inst *inst)
{
    td_s32 ret;

    ret = sample_vad_feed_sys_init();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_vad_feed_sys_init, ret);
        return ret;
    }

    /*
     * 数据流向 ai -->sea--> aenc --> adp
     * 按照数据流向反方向打开实例
     * 按照数据方向attach output
     */
    ret = sample_vad_feed_open_adp(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_vad_feed_open_adp, ret);
        goto out0;
    }

    ret = sample_vad_feed_open_aenc(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_vad_feed_open_aenc, ret);
        goto out1;
    }

    ret = sample_vad_feed_open_sea(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_vad_feed_open_sea, ret);
        goto out2;
    }

    ret = sample_vad_feed_open_ai(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_vad_feed_open_ai, ret);
        goto out3;
    }

    return EXT_SUCCESS;
out3:
    sample_vad_feed_close_sea(inst);
out2:
    sample_vad_feed_close_aenc(inst);
out1:
    sample_vad_feed_close_adp(inst);
out0:
    sample_vad_feed_sys_deinit();
    return ret;
}

static td_void sample_vad_feed_stop_inst(sample_vad_feed_inst *inst)
{
    td_s32 ret;
    td_u32 i;

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        if (inst->aenc_inst[i].task != TD_NULL) {
            inst->aenc_inst[i].task_enable = TD_FALSE;
            athread_exit(inst->aenc_inst[i].task);
            inst->aenc_inst[i].task = TD_NULL;
        }
    }

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        ret = uapi_aenc_stop(inst->aenc_inst[i].h_aenc);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_stop, ret);
        }
    }

    ret = uapi_sea_stop(inst->h_sea);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_sea_stop, ret);
    }
    sample_gui_aef_delete_handle(inst->h_sea);

    ret = uapi_ai_stop(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_stop, ret);
    }
}

static td_s32 sample_vad_feed_start_inst(sample_vad_feed_inst *inst)
{
    td_s32 ret;
    td_u32 i;
    athread_attr attr[ANEC_HANDLE_COUNT];

    ret = uapi_ai_start(inst->h_ai);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_ai_start, ret);
        return ret;
    }

    ret = uapi_sea_start(inst->h_sea);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_sea_start, ret);
            goto out;
        }

    sample_gui_aef_add_handle(GUI_AEF_SEA, inst->h_sea);

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        ret = uapi_aenc_start(inst->aenc_inst[i].h_aenc);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(uapi_aenc_start, ret);
            goto out;
        }

        inst->aenc_inst[i].task_enable = TD_TRUE;
        attr[i].priority = ATHREAD_PRIORITY_NORMAL;
        attr[i].stack_size = 0x1000; /* 4k */
        attr[i].name = "sample_vad_feed";

        ret = athread_create(&inst->aenc_inst[i].task, sample_vad_feed_thread,
            (td_void *)(&inst->aenc_inst[i]), &attr[i]);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(athread_create, ret);
            goto out;
        }
    }

    return EXT_SUCCESS;

out:
    (td_void)sample_vad_feed_stop_inst;
    return ret;
}

static td_void sample_vad_feed_usage(const td_char *name)
{
    sap_printf(" usage: %s -p [port] -c [channels] -b [bit_depth] -s [sample_rate] -v [volume] -r [bit_rate]\n", name);
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("        -p     specify ai port(mad0/i2s0/i2s1/adc0)\n"
           "        -t     the type of encoder format(mp3/opus/lc3/l2hc/sbc/msbc)(default:mp3)\n"
           "        -l     the type of encoder format(mp3/opus/lc3/l2hc/sbc/msbc)(default:pcm)\n"
           "        -c     channels of ai port(default: 1)\n"
           "        -b     bit_depth of ai port(default: 16)\n"
           "        -s     samplerate of ai port(default: 16000)\n"
           "        -o     the file of place (mp3/ogg)(default: NULL)\n"
           "        -d     the file of place (pcm)(default: NULL)\n"
           "        -v     volume of ai port(default: 0dB)\n"
           "        -m     mute ai port(default: false)\n"
           "        -r     bit_rate of aenc_first(default: false)\n"
           "        -f     the vad flag (default: false)\n"
           "\n");

    sap_printf(" examples:\n");
    sap_printf(" %s -p mad0 -c 1 -s 16000\n", name);
    sap_printf(" %s -p i2s1 -c 1 -s 16000\n", name);
    sap_printf(" %s -p adc0 -c 1 -s 16000 -v 30\n", name);
    sap_printf(" %s -p adc0 -c 1 -s 16000 -v 30 -t mp3\n", name);
    sap_printf(" %s -p adc0 -c 1 -s 16000 -v 30 -t opus\n", name);
    sap_printf(" %s -p adc0 -c 1 -s 16000 -v 30 -t opus -o file1\n", name);
    sap_printf(" %s -p adc0 -c 1 -s 16000 -v 30 -r 64000 -t opus -o file1 -t pcm -o file2\n", name);
    sap_printf("\n");
}

static td_s32 sample_vad_feed_parse_opt(td_char opt, const td_char *opt_arg,
    sample_vad_feed_inst *inst, td_u32 aenc_num)
{
    if (opt == 'h') {
        return EXT_FAILURE;
    }

    if (opt == 'm') {
        inst->mute = TD_TRUE;
        return EXT_SUCCESS;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 's':
            inst->pcm_format.sample_rate = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'c':
            inst->pcm_format.channels = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'b':
            inst->pcm_format.bit_depth = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'p':
            inst->ai_port = sample_audio_get_ai_port(opt_arg);
            break;
        case 'v':
            inst->volume.integer = (td_s32)strtol(opt_arg, TD_NULL, 0);
            break;
        case 'r':
            inst->aenc_inst[aenc_num].bit_rate = (td_u32)strtoul(opt_arg, TD_NULL, 0);
            break;
        case 'o':
            inst->aenc_inst[aenc_num].file_name = opt_arg;
            break;
        case 't': /* direct return */
            return sample_audio_get_acodec_id(opt_arg, &inst->aenc_inst[aenc_num].acodec_id);
            break;
        default:
            sap_printf("invalid command line\n");
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_vad_feed_parse_cmd_line(td_s32 argc, td_char *argv[], sample_vad_feed_inst *inst)
{
    td_s32 ret;
    td_char opt;
    parg_state arg_parse;
    td_u32 aenc_num = 0;
    td_u32 step = 0;

    if (argv == TD_NULL) {
        return EXT_FAILURE;
    }

    parg_init(&arg_parse);

    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "s:c:b:r:o:p:v:t:hm");
        if (ret == -1) {
            break;
        }

        opt = (td_char)ret;

        if (opt == 't') {
            aenc_num += step;
            step = 1;
        }
        if (sample_vad_feed_parse_opt(opt, arg_parse.optarg, inst, aenc_num) != EXT_SUCCESS) {
            sample_vad_feed_usage(argv[0]);
            return EXT_FAILURE;
        }
    }

    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    if (inst->ai_port == UAPI_AI_PORT_MAX) {
        sample_vad_feed_usage(argv[0]);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_vad_feed_open_file(sample_vad_feed_inst *inst)
{
    td_u32 i;
    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        inst->aenc_inst[i].h_file = TD_NULL;
        if (inst->aenc_inst[i].file_name != TD_NULL) {
            inst->aenc_inst[i].h_file = sample_audio_open_output_stream(inst->aenc_inst[i].file_name);
            if (inst->aenc_inst[i].h_file == TD_NULL) {
                sap_printf("open file %s error!\n", inst->aenc_inst[i].file_name);
                return EXT_FAILURE;
            }
        }
    }

    return EXT_SUCCESS;
}

static td_s32 sample_vad_feed_set_proc(sample_vad_feed_inst *inst)
{
    td_bool vad_flag = TD_FALSE;
    td_u32 i;

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        if (inst->aenc_inst[i].acodec_id == UAPI_ACODEC_ID_PCM) {
            vad_flag = TD_TRUE;
            vad_enable = TD_FALSE;
        }
    }

    for (i = 0; i < ANEC_HANDLE_COUNT; i++) {
        if (vad_flag == TD_TRUE && (inst->aenc_inst[i].acodec_id != UAPI_ACODEC_ID_PCM)) {
            if (sample_vad_feed_data_caching_init(&inst->aenc_inst[i]) != EXT_SUCCESS) {
                return EXT_FAILURE;
            }

            inst->aenc_inst[i].task_proc = sample_vad_feed_nomal_proc;
        } else if (vad_flag == TD_TRUE && (inst->aenc_inst[i].acodec_id == UAPI_ACODEC_ID_PCM)) {
            if (inst->pcm_format.bit_depth == UAPI_AUDIO_BIT_DEPTH_16) {
                inst->aenc_inst[i].energy_check = sample_vad_feed_energy_check_16bit;
            } else if (inst->pcm_format.bit_depth == UAPI_AUDIO_BIT_DEPTH_24) {
                inst->aenc_inst[i].energy_check = sample_vad_feed_energy_check_24bit;
            } else if (inst->pcm_format.bit_depth == UAPI_AUDIO_BIT_DEPTH_32) {
                inst->aenc_inst[i].energy_check = sample_vad_feed_energy_check_32bit;
            }

            inst->aenc_inst[i].task_proc = sample_vad_feed_pcm_proc;
        }

        if (inst->aenc_inst[i].task_proc == TD_NULL) {
            inst->aenc_inst[i].task_proc = sample_vad_feed_nomal_proc;
        }
    }

    return EXT_SUCCESS;
}

static td_s32 sample_vad_feed_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    sample_vad_feed_inst *inst = sample_vad_feed_get_inst();
    if (inst != TD_NULL) {
        sap_printf("%s is already running", argv[0]);
        return EXT_FAILURE;
    }

    inst = sample_vad_feed_alloc_inst();
    if (inst == TD_NULL) {
        sap_printf("sample_vad_feed_alloc_inst failed");
        return EXT_FAILURE;
    }

    sample_vad_feed_inst_init(inst);

    ret = sample_vad_feed_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_vad_feed_set_proc(inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_vad_feed_open_file(inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_vad_feed_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_vad_feed_open_inst, ret);
        goto out1;
    }

    ret = sample_vad_feed_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_vad_feed_start_inst, ret);
        goto out2;
    }

    return EXT_SUCCESS;
out2:
    sample_vad_feed_close_inst(inst);
out1:
    sample_vad_feed_close_file(inst);
out0:
    sample_vad_feed_free_inst(inst);
    return ret;
}

static td_s32 sample_vad_feed_exit(td_void)
{
    sample_vad_feed_inst *inst = sample_vad_feed_get_inst();
    if (inst != TD_NULL) {
        sample_vad_feed_stop_inst(inst);
        sample_vad_feed_close_inst(inst);
        sample_vad_feed_close_file(inst);
        sample_vad_feed_free_inst(inst);
    }

    return EXT_SUCCESS;
}

td_s32 sample_vad_feed(td_s32 argc, td_char *argv[])
{
    if (argc <= 1) {
        sample_vad_feed_usage(argv[0]);
        return EXT_FAILURE;
    }

    if (strcmp(argv[1], "q") == 0) {
        return sample_vad_feed_exit();
    } else {
        return sample_vad_feed_entry(argc, argv);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
