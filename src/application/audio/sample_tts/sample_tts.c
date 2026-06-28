/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: tts sample
 * Author: audio
 * Create: 2025-10-09
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "securec.h"
#include "thread_os.h"
#include "soc_osal.h"
#include "osal_list.h"
#include "soc_uapi_adp.h"
#include "soc_uapi_tts_frontend.h"
#include "am_model.h"
#include "vocoder_model.h"
#include "sample_audio_api.h"
#include "sample_audio_fops.h"
#include "sample_audio_utils.h"
#include "sample_audio_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AM_OUT_LEN 500
#define AM_OUT_FEATURE_LEN 80
#define VOCODER_IN_LEN 60
#define VOCODER_IN_OVERLAP_LEN 30
#define VOCODER_OUT_OVERLAP_LEN 6000
#define VOCODER_OUT_REMAIN_LEN 9000
#define VOCODER_OUT_DISCARD_LEN 3000
#define AM2VOCODER_FACTOR 200
#define MAX_OVERLAP_TIMES 17
#define FLOAT16_BYTE 2

#define DEFAULT_SAMPLE_TTS_AM_MODEL_NAME "/user/ops/am/am.exeom"
#define DEFAULT_SAMPLE_TTS_VOCODER_MODEL_NAME "/user/ops/vocoder/vocoder.exeom"
#define MAX_TTS_INPUT_GBK_ENCODE_SIZE 100
#define MAX_AM_OUT_REPEATITION_SIZE 2
#define MAX_AM_OUT_MEL_SIZE 80000
#define MAX_VOCODER_INPUT_SIZE 9600
#define MAX_VOCODER_OUTPUT_SIZE 24000
#define MAX_VOCODER_OUTPUT_TILE_SIZE 24000
#define MAX_VOCODER_OUTPUT_LENGHT 12000
#define CROP_LEN 800

typedef struct {
    td_u32 phoneme_data[AUDIO_PIN_YIN_MAX_OUT_SIZE];
    td_u16 phoneme_mask[AUDIO_PIN_YIN_MAX_OUT_SIZE];
} sample_tts_phoneme;

typedef struct sample_tts_inst {
    const td_char *input_file_name;
    const td_char *vocoder_out_file_name;
    FILE *h_vocoder_out_file;
    td_char *input_text;
    sample_tts_phoneme phoneme;

    const td_char *am_model_name;
    td_u8 repeatition[MAX_AM_OUT_REPEATITION_SIZE];
    td_u8 mel[MAX_AM_OUT_MEL_SIZE];
    td_s16 valid_length;
    td_s16 tile_num;

    const td_char *vocoder_model_name;
    td_u8 *vocoder_input;
    td_void *vocoder_input_temp;
    td_void *vocoder_output;
    td_s16 *tile_buf;
    data_info vocoder_tile;

    uapi_audio_pcm_format pcm_format;
    td_handle h_player;
    td_u8 *frame_buf;
    uapi_audio_frame frame;
    td_u32 frame_size;
    FILE *fp;

    uapi_snd_gain gain;
    td_handle h_snd;
    uapi_snd_attr snd_attr;

    athread_handle task_tts;
    td_bool task_tts_active;
    athread_handle task_tts_player;
    td_bool task_tts_player_active;

    struct osal_list_head node;
} sample_tts_inst;

static OSAL_LIST_HEAD(g_sample_tts_list);

static sample_tts_inst *sample_tts_alloc_inst(td_void)
{
    sample_tts_inst *inst = (sample_tts_inst *)malloc(sizeof(sample_tts_inst));
    if (inst == TD_NULL) {
        sap_err_log_info("sample tts inst mem alloc failed!");
        return TD_NULL;
    }
    (td_void) memset_s(inst, sizeof(*inst), 0, sizeof(sample_tts_inst));

    td_void *buf = malloc(MAX_VOCODER_OUTPUT_TILE_SIZE + 8); /* 8:remain */
    if (buf == TD_NULL) {
        sap_err_log_info("buf malloc failed!");
        free(inst);
        return TD_NULL;
    }
    data_info_init(&inst->vocoder_tile, (td_u32)(td_uintptr_t)buf, MAX_VOCODER_OUTPUT_TILE_SIZE + 8); /* 8:remain */

    osal_list_add_tail(&inst->node, &g_sample_tts_list);
    return inst;
}

static td_void sample_tts_free_inst(sample_tts_inst *inst)
{
    if (inst->h_vocoder_out_file != TD_NULL) {
        sap_fclose(inst->h_vocoder_out_file);
        inst->h_vocoder_out_file = TD_NULL;
    }

    if (inst->input_text != TD_NULL) {
        free(inst->input_text);
        inst->input_text = TD_NULL;
    }
    if (inst->vocoder_tile.buf != TD_NULL) {
        free(inst->vocoder_tile.buf);
        inst->vocoder_tile.buf = TD_NULL;
    }
    (td_void) memset_s(&inst->vocoder_tile, sizeof(inst->vocoder_tile), 0, sizeof(data_info));

    osal_list_del(&inst->node);
    free(inst);
    inst = TD_NULL;
}

static td_s32 read_input_to_buffer(sample_tts_inst *inst, td_char *buff, td_s32 buff_size)
{
    td_s32 ret = EXT_SUCCESS;
    FILE *file = TD_NULL;

    if (inst->input_text == TD_NULL) {
        file = sample_audio_open_input_stream(inst->input_file_name);
        if (file == TD_NULL) {
            sap_printf("open %s failed\n", inst->input_file_name);
            return EXT_FAILURE;
        }
        ret = fscanf_s(file, "%s", buff, buff_size);
        if (ret <= 0) {
            sap_err_log_fun(fscanf_s, ret);
            sap_fclose(file);
            file = TD_NULL;
            return EXT_FAILURE;
        }
        sap_fclose(file);
        file = TD_NULL;
    } else {
        ret = snprintf_s(buff, buff_size, buff_size, "%s", inst->input_text);
        if (ret < 0) {
            sap_err_log_fun(snprintf_s, ret);
            return EXT_FAILURE;
        }
        buff[buff_size - 1] = '\0';
    }

    return EXT_SUCCESS;
}

static td_s32 sample_tts_text_to_phoneme(sample_tts_inst *inst)
{
    td_u32 i;
    td_s32 ret = EXT_SUCCESS;
    pin_yin_stru *pin_yin_handle = TD_NULL;
    sample_tts_phoneme *phoneme = &inst->phoneme;
    td_char *in_buff;
    td_u32 in_buff_size = MAX_TTS_INPUT_GBK_ENCODE_SIZE + 1;
    td_u8 *out_buff;
    td_u32 out_buff_size = (AUDIO_CHS_MAX_OUT_NUM * (sizeof(td_u16) + sizeof(td_u8))) + 1;

    in_buff = calloc(in_buff_size, 1);
    if (in_buff == TD_NULL) {
        sap_err_log_info("buf calloc failed!");
        goto out0;
    }
    out_buff = calloc(out_buff_size, 1);
    if (out_buff == TD_NULL) {
        sap_err_log_info("buf calloc failed!");
        goto out1;
    }

    ret = read_input_to_buffer(inst, in_buff, in_buff_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(read_input_to_buffer, ret);
        goto out2;
    }

    ret = audio_pinyin_convert_init((void **)&pin_yin_handle, out_buff, out_buff_size);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(audio_pinyin_convert_init, ret);
        goto out2;
    }

    ret = audio_pinyin_convert_process(pin_yin_handle, in_buff, strlen(in_buff));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(audio_pinyin_convert_process, ret);
        goto out3;
    }

    for (i = 0; i < AUDIO_PIN_YIN_MAX_OUT_SIZE; i++) {
        phoneme->phoneme_data[i] = pin_yin_handle->out_buffer[i];
        phoneme->phoneme_mask[i] = (phoneme->phoneme_data[i] == 0);
    }

out3:
    audio_pinyin_convert_deinit(pin_yin_handle);
out2:
    free(out_buff);
    out_buff = TD_NULL;
out1:
    free(in_buff);
    in_buff = TD_NULL;
out0:
    return ret;
}

static td_s32 sample_tts_player_open(sample_tts_inst *inst)
{
    td_s32 ret;
    sample_acodec_arg in_arg;
    td_handle track;

    sample_escast_arg escast_arg = {
        .player = &inst->h_player,
        .track = &track,
        .in_arg = &in_arg,
        .snd = &inst->h_snd,
        .snd_attr = &inst->snd_attr,
        .audio_scene = UAPI_AUDIO_SCENE_UNKNOWN,
        .in_file = TD_NULL,
        .in_data_addr = 0,
        .in_data_size = 0,
        .track_mode = TRACK_MODE_DEFAULT,
    };

    /* set track input pcm format matching tts output pcm format */
    ret = memcpy_s(&in_arg.pcm_format, sizeof(in_arg.pcm_format), &inst->pcm_format, sizeof(inst->pcm_format));
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return ret;
    }

    ret = sample_ao_player_open(&escast_arg, UAPI_SND_0, TD_FALSE);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_ao_player_open, ret);
        return ret;
    }

    return uapi_snd_set_volume(inst->h_snd, inst->snd_attr.port_attr[0].out_port, &inst->gain, TD_NULL);
}

static sample_tts_inst *sample_tts_get_inst(td_void)
{
    if (osal_list_empty(&g_sample_tts_list) != TD_FALSE) {
        return TD_NULL;
    }

    return osal_list_first_entry(&g_sample_tts_list, sample_tts_inst, node);
}

static td_void sample_tts_get_def_attr_snd(sample_tts_inst *inst)
{
    inst->snd_attr.port_num = 1;
    inst->snd_attr.port_attr[0].out_port = SND_OUT_PORT_DEFAULT;
    inst->snd_attr.channels = SND_OUT_CHANNEL_DEF;
    inst->snd_attr.bit_depth = SND_OUT_BIT_DEPTH_DEF;
    inst->snd_attr.sample_rate = SND_OUT_SAMPLE_RATE_DEF;
}

static td_void sample_tts_inst_init(sample_tts_inst *inst)
{
    inst->input_file_name = TD_NULL;
    inst->input_text = TD_NULL;
    inst->am_model_name = DEFAULT_SAMPLE_TTS_AM_MODEL_NAME;
    inst->vocoder_model_name = DEFAULT_SAMPLE_TTS_VOCODER_MODEL_NAME;

    inst->pcm_format.channels = UAPI_AUDIO_CHANNEL_1;
    inst->pcm_format.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
    inst->pcm_format.sample_rate = UAPI_AUDIO_SAMPLE_RATE_8K;
    inst->pcm_format.sample_per_frame = inst->pcm_format.sample_rate / 100; /* 100 --> 10ms */

    inst->frame_size = calc_pcm_frame_size(&inst->pcm_format);

    sample_tts_get_def_attr_snd(inst);
}

static td_s32 sample_tts_open_inst(sample_tts_inst *inst)
{
    td_s32 ret;

    ret = sample_tts_player_open(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_tts_player_open, ret);
        return ret;
    }

    ret = initModelEnv();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(initModelEnv, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_void sample_tts_close_inst(td_void)
{
    td_s32 ret;

    ret = destroyModelEnv();
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(destroyModelEnv, ret);
    }

    sample_ao_player_close();
}

static td_s32 sample_tts_am_process(sample_tts_inst *inst)
{
    td_s32 ret = EXT_SUCCESS;

    ret = amModelInit(inst->am_model_name);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(amModelInit, ret);
        return ret;
    }

    ret = amModelExecute(inst->phoneme.phoneme_data, inst->phoneme.phoneme_mask);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(amModelExecute, ret);
        goto out0;
    }

    ret = amModelCopyOutput(inst->repeatition, inst->mel);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(amModelCopyOutput, ret);
    }

out0:
    amModelDestroy();
    return ret;
}

static td_s32 sample_tts_am_post_process(sample_tts_inst *inst)
{
    td_s32 i;
    td_s16 repeatition;
    td_u32 mel_slice_size;
    td_u32 mel_slice_overlap_offset;
    td_s32 ret = EXT_SUCCESS;

    repeatition = ((td_s16 *)inst->repeatition)[0];
    inst->tile_num = (repeatition - VOCODER_IN_LEN + VOCODER_IN_OVERLAP_LEN - 1) / VOCODER_IN_OVERLAP_LEN + 1;
    inst->valid_length = (repeatition - (inst->tile_num - 1) * VOCODER_IN_OVERLAP_LEN) * AM2VOCODER_FACTOR;

    if (inst->tile_num == 1 && inst->valid_length > CROP_LEN) {
        inst->valid_length = inst->valid_length - CROP_LEN;
    } else {
        if (inst->valid_length <= VOCODER_OUT_DISCARD_LEN + CROP_LEN) {
            inst->tile_num = inst->tile_num - 1;
            inst->valid_length = MAX_VOCODER_OUTPUT_LENGHT - CROP_LEN + inst->valid_length - VOCODER_OUT_DISCARD_LEN;
        } else {
            inst->valid_length = inst->valid_length - CROP_LEN;
        }
    }

    inst->vocoder_input = (td_u8 *)malloc(inst->tile_num * MAX_VOCODER_INPUT_SIZE);
    if (inst->vocoder_input == TD_NULL) {
        sap_err_log_info("vocoder_input malloc failed!");
        return EXT_FAILURE;
    }

    mel_slice_size = VOCODER_IN_LEN * AM_OUT_FEATURE_LEN * FLOAT16_BYTE;
    mel_slice_overlap_offset = VOCODER_IN_OVERLAP_LEN * AM_OUT_FEATURE_LEN * FLOAT16_BYTE;
    for (i = 0; i < inst->tile_num; i++) {
        ret = memcpy_s(inst->vocoder_input + i * mel_slice_size, mel_slice_size,
                       inst->mel + i * mel_slice_overlap_offset, mel_slice_size);
        if (ret != EOK) {
            sap_err_log_fun(memcpy_s, ret);
            free(inst->vocoder_input);
            inst->vocoder_input = TD_NULL;
            break;
        }
    }

    return ret;
}

static td_void sample_tts_am_post_process_exit(sample_tts_inst *inst)
{
    if (inst->vocoder_input != TD_NULL) {
        free(inst->vocoder_input);
        inst->vocoder_input = TD_NULL;
    }
}

static void float32_tts(float *__restrict out, const unsigned short in)
{
    unsigned int t1;
    unsigned int t2;
    unsigned int t3;

    t1 = in & 0x7fffu;  // Non-sign bits
    t2 = in & 0x8000u;  // Sign bit
    t3 = in & 0x7c00u;  // Exponent

    t1 <<= 13u;  // Align mantissa on MSB
    t2 <<= 16u;  // Shift sign bit into position

    t1 += 0x38000000;         // Adjust bias
    t1 = (t3 == 0 ? 0 : t1);  // Denormals-as-zero
    t1 |= t2;                 // Re-insert sign bit

    *((unsigned int *)out) = t1;
};

static td_s32 sample_tts_vocoder_buf_create(sample_tts_inst *inst)
{
    td_s32 ret = EXT_SUCCESS;

    inst->vocoder_input_temp = malloc(MAX_VOCODER_INPUT_SIZE);
    if (inst->vocoder_input_temp == TD_NULL) {
        sap_err_log_info("malloc vocoder_input_temp failed");
        return EXT_FAILURE;
    }
    (td_void) memset_s(inst->vocoder_input_temp, MAX_VOCODER_INPUT_SIZE, 0, MAX_VOCODER_INPUT_SIZE);

    inst->vocoder_output = malloc(MAX_VOCODER_OUTPUT_SIZE);
    if (inst->vocoder_output == TD_NULL) {
        sap_err_log_info("malloc vocoder_output failed");
        goto out0;
    }
    (td_void) memset_s(inst->vocoder_output, MAX_VOCODER_OUTPUT_SIZE, 0, MAX_VOCODER_OUTPUT_SIZE);

    inst->tile_buf = (td_s16 *)malloc(MAX_VOCODER_OUTPUT_TILE_SIZE);
    if (inst->tile_buf == TD_NULL) {
        sap_err_log_info("malloc tile_buf failed");
        goto out1;
    }
    (td_void) memset_s(inst->tile_buf, MAX_VOCODER_OUTPUT_TILE_SIZE, 0, MAX_VOCODER_OUTPUT_TILE_SIZE);

    return EXT_SUCCESS;
out1:
    free(inst->vocoder_output);
    inst->vocoder_output = TD_NULL;
out0:
    free(inst->vocoder_input_temp);
    inst->vocoder_input_temp = TD_NULL;
    return ret;
}

static td_void sample_tts_vocoder_buf_destory(sample_tts_inst *inst)
{
    if (inst->tile_buf != TD_NULL) {
        free(inst->tile_buf);
        inst->tile_buf = TD_NULL;
    }

    if (inst->vocoder_output != TD_NULL) {
        free(inst->vocoder_output);
        inst->vocoder_output = TD_NULL;
    }

    if (inst->vocoder_input_temp != TD_NULL) {
        free(inst->vocoder_input_temp);
        inst->vocoder_input_temp = TD_NULL;
    }
}

static td_void sample_tts_sample_float16_to_int16(const td_void *float16_src, td_u32 float16_size, td_s16 *int16_dst,
                                                  td_u32 *int16_size)
{
    td_u32 samples = float16_size / FLOAT16_BYTE;

    for (td_u32 i = 0; i < samples; i++) {
        float tmp;
        float32_tts(&tmp, ((unsigned short *)float16_src)[i]);
        int16_dst[i] = (td_s16)(tmp * 32767); /* 32767: 16-bit PCM */
    }

    *int16_size = samples * sizeof(td_s16);
}

static td_s32 sample_tts_vocoder_write_file(sample_tts_inst *inst, const td_u8 *from, td_u32 tile_size)
{
    td_u32 write;
    write = sap_fwrite(from, 1, tile_size, inst->h_vocoder_out_file);
    if (write == 0) {
        sap_printf("sample_tts_vocoder_write_file is zero");
        return EXT_FAILURE;
    }
    return EXT_SUCCESS;
}

static td_s32 sample_tts_send_tile(sample_tts_inst *inst, td_s32 tile_index)
{
    const td_void *tile_float16_src;
    td_u32 tile_float16_size;
    td_u32 tile_size;
    td_u32 free_size;
    td_u32 len;
    td_s32 ret = EXT_SUCCESS;

    if (inst->tile_num == 1) {
        tile_float16_src = ((unsigned char *)inst->vocoder_output);
        tile_float16_size = inst->valid_length * FLOAT16_BYTE;
    } else if (tile_index == 0) {
        tile_float16_src = ((unsigned char *)inst->vocoder_output);
        tile_float16_size = VOCODER_OUT_REMAIN_LEN * FLOAT16_BYTE;
    } else if (tile_index == inst->tile_num - 1) {
        tile_float16_src = ((unsigned char *)inst->vocoder_output) + VOCODER_OUT_OVERLAP_LEN;
        tile_float16_size = (inst->valid_length - VOCODER_OUT_DISCARD_LEN) * FLOAT16_BYTE;
    } else {
        tile_float16_src = ((unsigned char *)inst->vocoder_output) + VOCODER_OUT_OVERLAP_LEN;
        tile_float16_size = VOCODER_OUT_OVERLAP_LEN * FLOAT16_BYTE;
    }

    sample_tts_sample_float16_to_int16(tile_float16_src, tile_float16_size, inst->tile_buf, &tile_size);

    if (inst->h_vocoder_out_file != TD_NULL) {
        ret = sample_tts_vocoder_write_file(inst, (const td_u8 *)inst->tile_buf, tile_size);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_tts_vocoder_write_file, ret);
            return ret;
        }
    }

    while (inst->task_tts_active == TD_TRUE) {
        free_size = circ_buf_query_free(&inst->vocoder_tile.cb);
        if (free_size < tile_size) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }

        len = circ_buf_write(&inst->vocoder_tile.cb, (const td_u8 *)inst->tile_buf, tile_size);
        if (len != tile_size) {
            sap_printf("tile_size: %u, len: %u\n", tile_size, len);
        }
        break;
    }
    sap_printf("%s exit!\n", __FUNCTION__);

    return ret;
}

static td_s32 sample_tts_vocoder_process(sample_tts_inst *inst)
{
    td_s32 tile_index;
    td_s32 ret = EXT_SUCCESS;

    ret = vocoderModelInit(inst->vocoder_model_name);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(vocoderModelInit, ret);
        return ret;
    }

    ret = sample_tts_vocoder_buf_create(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_tts_vocoder_buf_create, ret);
        goto out0;
    }

    for (tile_index = 0; tile_index < inst->tile_num && inst->task_tts_active == TD_TRUE; tile_index++) {
        ret = memcpy_s(inst->vocoder_input_temp, MAX_VOCODER_INPUT_SIZE,
                       inst->vocoder_input + tile_index * MAX_VOCODER_INPUT_SIZE, MAX_VOCODER_INPUT_SIZE);
        if (ret != EOK) {
            sap_err_log_fun(memcpy_s, ret);
            break;
        }

        ret = vocoderModelExecute(inst->vocoder_input_temp);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(vocoderModelExecute, ret);
            break;
        }

        ret = vocoderModelCopyOutput(inst->vocoder_output);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(vocoderModelCopyOutput, ret);
            break;
        }

        ret = sample_tts_send_tile(inst, tile_index);
        if (ret != EXT_SUCCESS) {
            sap_err_log_fun(sample_tts_send_tile, ret);
            break;
        }
    }

    sample_tts_vocoder_buf_destory(inst);
out0:
    vocoderModelDestroy();
    sap_printf("%s exit!\n", __FUNCTION__);

    return ret;
}

static td_void sample_tts_thread(td_void *args)
{
    td_s32 ret;
    sample_tts_inst *inst = (sample_tts_inst *)args;

    ret = sample_tts_text_to_phoneme(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_tts_text_to_phoneme, ret);
        goto out;
    }

    ret = sample_tts_am_process(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_tts_am_process, ret);
        goto out;
    }

    ret = sample_tts_am_post_process(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_tts_am_post_process, ret);
        goto out;
    }

    ret = sample_tts_vocoder_process(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_tts_vocoder_process, ret);
        sample_tts_am_post_process_exit(inst);
        goto out;
    }

    sample_tts_am_post_process_exit(inst);
out:
    athread_set_exit(inst->task_tts, TD_TRUE);
    sap_printf("%s exit!\n", __FUNCTION__);
}

static td_void sample_tts_build_frame(const sample_tts_inst *inst, uapi_audio_frame *frame)
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

static td_void sample_tts_player(td_void *args)
{
    td_s32 ret;
    sample_tts_inst *inst = (sample_tts_inst *)args;
    td_u32 len;
    td_u32 busy;
    circ_buf *cb = TD_NULL;

    inst->frame_buf = (td_u8 *)malloc(inst->frame_size);
    if (inst->frame_buf == TD_NULL) {
        sap_err_log_u32(inst->frame_size);
        sap_err_log_info("malloc frame_buf failed");
        goto out;
    }
    sample_tts_build_frame(inst, &inst->frame);
    cb = &inst->vocoder_tile.cb;

    while (inst->task_tts_player_active == TD_TRUE) {
        uapi_adp_query_free(inst->h_player, &len);
        if (len < inst->frame_size) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }

        busy = circ_buf_query_busy(cb);
        if (busy == 0) {
            sap_msleep(THREAD_SLEEP_5MS);
            continue;
        }
        busy = sap_min(busy, inst->frame_size);
        inst->frame.bits_bytes = busy;

        if (circ_buf_read(cb, (td_u8 *)inst->frame.bits_buffer, inst->frame.bits_bytes) != inst->frame.bits_bytes) {
            sap_err_log_info("Fatal Error: circ_buf_read error");
            break;
        }

        ret = uapi_adp_send_frame(inst->h_player, &inst->frame);
        if (ret != EXT_SUCCESS) {
            sap_msleep(THREAD_SLEEP_10MS);
            continue;
        }
    }

    free(inst->frame_buf);
    inst->frame_buf = TD_NULL;
out:
    athread_set_exit(inst->task_tts_player, TD_TRUE);
    sap_printf("%s exit!\n", __FUNCTION__);
}

static td_s32 sample_tts_start_inst(sample_tts_inst *inst)
{
    td_s32 ret;
    athread_attr attr;

    inst->task_tts_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_tts_send";
    ret = athread_create(&inst->task_tts, sample_tts_thread, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        return ret;
    }
    sap_printf("sample_tts_send thread create ok!\n");

    inst->task_tts_player_active = TD_TRUE;
    attr.priority = ATHREAD_PRIORITY_NORMAL;
    attr.stack_size = 0x1000; /* 4k */
    attr.name = "sample_tts_player";
    ret = athread_create(&inst->task_tts_player, sample_tts_player, (td_void *)inst, &attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(athread_create, ret);
        return ret;
    }
    sap_printf("sample_tts_player thread create ok!\n");

    return EXT_SUCCESS;
}

static td_void sample_tts_stop_inst(sample_tts_inst *inst)
{
    if (inst->task_tts_player != TD_NULL) {
        inst->task_tts_player_active = TD_FALSE;
        athread_exit(inst->task_tts_player);
        inst->task_tts_player = TD_NULL;
    }

    if (inst->task_tts != TD_NULL) {
        inst->task_tts_active = TD_FALSE;
        athread_exit(inst->task_tts);
        inst->task_tts = TD_NULL;
    }
}

static td_void sample_tts_usage(td_void)
{
    sap_printf("\n");
    sap_printf("If you want me to read a file aloud");
    sap_printf("    usage:  sample_tts -i [InputFileName]\n");
    sap_printf("\n");
    sap_printf("If you want me to read a piece of text aloud");
    sap_printf("    usage:  sample_tts -l [ChineseText]\n");
    sap_printf("\n");
    sap_printf("If you want to set play volume");
    sap_printf("    usage:  sample_tts -v [VolumeIntegerValue]\n");
    sap_printf("\n");
    sap_printf("If you want to set snd out port(dac0/i2s0)\n");
    sap_printf("    usage:  sample_tts -p [OutPortName]\n");
    sap_printf("\n");
    sap_printf("If you want to generate a vocoder ouput file\n");
    sap_printf("    usage:  sample_tts -o [OutputFileName]\n");
    sap_printf("\n");
    sap_printf("        -h     show this usage message and abort\n");
    sap_printf("\n");
}

static td_s32 sample_tts_set_input_text(const td_char *opt_arg, sample_tts_inst *inst)
{
    td_s32 ret;
    td_u32 length = 0;

    length = strlen(opt_arg) + 1;

    inst->input_text = malloc(length);
    if (inst->input_text == TD_NULL) {
        sap_err_log_info("input_text malloc failed!");
        ret = EXT_FAILURE;
        return ret;
    }

    ret = snprintf_s(inst->input_text, length, length, "%s", opt_arg);
    if (ret < 0) {
        sap_err_log_fun(snprintf_s, ret);
        free(inst->input_text);
        inst->input_text = TD_NULL;
        return ret;
    }

    inst->input_text[length - 1] = '\0';

    return EXT_SUCCESS;
}

static td_s32 sample_tts_open_vocoder_out_file(sample_tts_inst *inst)
{
    inst->h_vocoder_out_file = TD_NULL;

    if (inst->vocoder_out_file_name != TD_NULL) {
        inst->h_vocoder_out_file = sample_audio_open_output_stream(inst->vocoder_out_file_name);
        if (inst->h_vocoder_out_file == TD_NULL) {
            sap_printf("open file %s error!\n", inst->vocoder_out_file_name);
            return EXT_FAILURE;
        }
    }
    return EXT_SUCCESS;
}

static td_s32 sample_tts_parse_opt(td_char opt, const td_char *opt_arg, sample_tts_inst *inst)
{
    if (opt == 'h') {
        sample_tts_usage();
        return EXT_FAILURE;
    }

    if (opt_arg == TD_NULL) {
        sap_printf("invalid option -- '%c'\n", opt);
        return EXT_FAILURE;
    }

    switch (opt) {
        case 'i':
            inst->input_file_name = opt_arg;
            break;
        case 'l':
            return sample_tts_set_input_text(opt_arg, inst);
        case 'v':
            inst->gain.integer = strtol(opt_arg, TD_NULL, 0);
            break;
        case 'p':
            inst->snd_attr.port_attr[0].out_port = sample_audio_get_ao_port(opt_arg);
            break;
        case 'o':
            inst->vocoder_out_file_name = opt_arg;
            return sample_tts_open_vocoder_out_file(inst);
        default:
            sap_printf("invalid command line\n");
            sample_tts_usage();
            return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_tts_parse_cmd_line(td_s32 argc, td_char *argv[], sample_tts_inst *inst)
{
    parg_state arg_parse;
    td_s32 ret;
    td_char opt;

    if ((argv == TD_NULL) || (argc < 0x2)) {
        goto out;
    }

    parg_init(&arg_parse);
    while (1) {
        ret = parg_getopt(&arg_parse, argc, argv, "i:l:v:p:o:h");
        if (ret == -1) {
            break;
        }
        opt = (td_char)ret;
        if (sample_tts_parse_opt(opt, arg_parse.optarg, inst) != EXT_SUCCESS) {
            return EXT_FAILURE;
        }
    }
    if (inst->input_text != TD_NULL) {
        inst->input_file_name = TD_NULL;
    }
    return EXT_SUCCESS;
out:
    sample_tts_usage();
    return EXT_FAILURE;
}

static td_s32 sample_tts_entry(int argc, td_char *argv[])
{
    td_s32 ret;
    sample_tts_inst *inst = sample_tts_get_inst();

    if (inst != TD_NULL) {
        sap_printf("%s is already running\n", argv[0]);
        return EXT_FAILURE;
    }

    inst = sample_tts_alloc_inst();
    if (inst == TD_NULL) {
        return EXT_FAILURE;
    }

    sample_tts_inst_init(inst);

    ret = sample_tts_parse_cmd_line(argc, argv, inst);
    if (ret != EXT_SUCCESS) {
        goto out0;
    }

    ret = sample_tts_open_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_tts_open_inst, ret);
        goto out0;
    }

    ret = sample_tts_start_inst(inst);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_tts_start_inst, ret);
        goto out1;
    }

    sap_printf("sample_rate: %u, channels: %u, bit_depth: %u\n", inst->pcm_format.sample_rate,
               inst->pcm_format.channels, inst->pcm_format.bit_depth);

    return EXT_SUCCESS;
out1:
    sample_tts_close_inst();
out0:
    sample_tts_free_inst(inst);

    return ret;
}

static td_s32 sample_tts_exit(td_void)
{
    sample_tts_inst *inst = sample_tts_get_inst();

    if (inst != TD_NULL) {
        sample_tts_stop_inst(inst);
        sample_tts_close_inst();
        sample_tts_free_inst(inst);
    }

    return EXT_SUCCESS;
}

td_s32 sample_tts(td_s32 argc, td_char *argv[])
{
    if ((argc > 1) && (strcmp(argv[1], "q") == 0)) {
        return sample_tts_exit();
    } else {
        return sample_tts_entry(argc, argv);
    }
}

#ifdef __cplusplus
}
#endif