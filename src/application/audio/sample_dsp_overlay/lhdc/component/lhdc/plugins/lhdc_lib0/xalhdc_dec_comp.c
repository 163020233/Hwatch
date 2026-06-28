/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: lhdc dec adpater
 * Author: audio
 * Create: 2025-10-21
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "component.h"
#include "ha_codec.h"
#include "ha_codec_define.h"
#include "log_overlay.h"
#include "lhdcv5_util_dec.h"

int memcpy_s(void *dest, size_t dest_max, const void *src, size_t count);
int memset_s(void *dest, size_t dest_max, int c, size_t count);
#define EOK 0

#define LHDC_DEC_MAX_INPUT_BYTES 2560
#define LHDC_DEC_MAX_OUTPUT_BYTES 7680
#define LHDC_DEC_MIN_INPUT_BYTES 16
#define LHDC_ENCODED_BYTES_162 162
#define LHDC_ENCODED_BYTES_176 176

#define LHDC_DEC_INIT_DECODER_FAILED 0x80011000

#define LHDC_BT_NAME_MAX_SIZE 64
#define LHDC_UUID_MAX_SIZE    20
#define LHDC_LICENSE_MAX_SIZE 256

typedef struct {
    td_char bt_name[LHDC_BT_NAME_MAX_SIZE];
    td_u32 bt_name_bytes;
    td_char uuid[LHDC_UUID_MAX_SIZE];
    td_u32 uuid_bytes;
    td_char lhdc_license[LHDC_LICENSE_MAX_SIZE];
    td_u32 lhdc_license_bytes;
} acodec_lhdc_dec_priv_cfg;

typedef struct {
    td_char *bt_name;
    td_u32 bt_name_bytes;
    td_char *uuid;
    td_u32 uuid_bytes;
    td_char *lhdc_license;
    td_u32 lhdc_license_bytes;
} lhdc_license_info_t;

typedef struct {
    td_void *handle;
    td_u32 sample_rate;
    td_u32 bit_depth;
    td_u32 channels;
    td_u32 samples;
    Lhdcv5FrameInfo frm_info;

    td_void *scratch_buf;
    td_u32 scratch_size;
} lhdc_dec_instance;

static acodec_lhdc_dec_priv_cfg g_lhdc_license_info;

td_void platform_get_lhdc5_license_info(lhdc_license_info_t *lhdc_license_info)
{
    lhdc_license_info->bt_name = g_lhdc_license_info.bt_name;
    lhdc_license_info->bt_name_bytes = g_lhdc_license_info.bt_name_bytes;
    lhdc_license_info->uuid = g_lhdc_license_info.uuid;
    lhdc_license_info->uuid_bytes = g_lhdc_license_info.uuid_bytes;
    lhdc_license_info->lhdc_license = g_lhdc_license_info.lhdc_license;
    lhdc_license_info->lhdc_license_bytes = g_lhdc_license_info.lhdc_license_bytes;
}

static td_void platform_set_lhdc5_license_info(const ha_codec_dec_param *open_param)
{
    td_s32 ret;
    acodec_get_private_param *private_param = TD_NULL;

    if (open_param->private_data == TD_NULL || open_param->private_data_size < sizeof(acodec_get_private_param)) {
        return;
    }

    private_param = (acodec_get_private_param *)open_param->private_data;
    if (private_param->private_data == TD_NULL ||
        private_param->private_data_size != sizeof(acodec_lhdc_dec_priv_cfg)) {
        return;
    }

    ret = memcpy_s(&g_lhdc_license_info, sizeof(g_lhdc_license_info),
        private_param->private_data, private_param->private_data_size);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return;
    }
}

static td_s32 ha_lhdc_dec_null_handle_cmd_process(td_u32 cmd, td_void *arg)
{
    td_u32 *frame_size = (td_u32 *)arg;

    switch (cmd) {
        case ACODEC_IN_FRAME_SIZE_GET_CMD:
            /* get default in frame size */
            *frame_size = LHDC_DEC_MAX_INPUT_BYTES;
            break;

        case ACODEC_OUT_FRAME_SIZE_GET_CMD:
            /* get default out frame size */
            *frame_size = LHDC_DEC_MAX_OUTPUT_BYTES;
            break;

        default: /* direct return */
            return ERR_HACODEC_NOT_SUPPORT;
    }

    return AUDIO_SUCCESS;
}

static td_s32 ha_lhdc_dec_get_in_frm_size(td_void *decoder, td_void *arg)
{
    td_u32 *frame_size = (td_u32 *)arg;
    audio_unused(decoder);

    *frame_size = LHDC_DEC_MAX_INPUT_BYTES;
    return AUDIO_SUCCESS;
}

static td_s32 ha_lhdc_dec_get_out_frm_size(td_void *decoder, td_void *arg)
{
    td_u32 *frame_size = (td_u32 *)arg;
    audio_unused(decoder);

    *frame_size = LHDC_DEC_MAX_OUTPUT_BYTES;
    return AUDIO_SUCCESS;
}

static td_s32 ha_lhdc_dec_set_scratch_buf(td_void *decoder, td_void *arg)
{
    lhdc_dec_instance *lhdc_dec = (lhdc_dec_instance *)decoder;
    ha_codec_param_set_scratch_buf *param = (ha_codec_param_set_scratch_buf *)arg;
    if (param->scratch_buf_size < LHDC_DEC_MAX_INPUT_BYTES) {
        lhdc_dec->scratch_buf = TD_NULL;
        return ERR_HACODEC_NOT_ENOUGH_SCRATCH_BUFFER_SIZE;
    }

    check_null_ptr(param->scratch_buf);

    lhdc_dec->scratch_buf = param->scratch_buf;
    lhdc_dec->scratch_size = LHDC_DEC_MAX_INPUT_BYTES;

    return AUDIO_SUCCESS;
}

static td_s32 ha_lhdc_dec_send_cmd(td_void *decoder, td_u32 cmd, td_void *arg)
{
    check_null_ptr(arg);

    if (decoder == TD_NULL) {
        return ha_lhdc_dec_null_handle_cmd_process(cmd, arg);
    }

    switch (cmd) {
        case ACODEC_IN_FRAME_SIZE_GET_CMD:
            return ha_lhdc_dec_get_in_frm_size(decoder, arg);

        case ACODEC_OUT_FRAME_SIZE_GET_CMD:
            return ha_lhdc_dec_get_out_frm_size(decoder, arg);

        case ACODEC_SCRATCH_BUF_SET_CMD:
            return ha_lhdc_dec_set_scratch_buf(decoder, arg);

        default: /* direct return */
            return ERR_HACODEC_NOT_SUPPORT;
    }
}

static td_s32 lhdc_dec_check_param(const ha_codec_dec_param *open_param)
{
    if (open_param->channels != HA_CODEC_CH_NUM_1 &&
        open_param->channels != HA_CODEC_CH_NUM_2) {
        sap_err_log_u32(open_param->channels);
        return ERR_HACODEC_INVALID_CHANNEL;
    }

    if (open_param->sample_rate != HA_CODEC_SAMPLE_RATE_44K &&
        open_param->sample_rate != HA_CODEC_SAMPLE_RATE_48K &&
        open_param->sample_rate != HA_CODEC_SAMPLE_RATE_96K &&
        open_param->sample_rate != HA_CODEC_SAMPLE_RATE_192K) {
        sap_err_log_u32(open_param->sample_rate);
        return ERR_HACODEC_INVALID_SAMPLERATE;
    }

    if (open_param->bit_depth != HA_CODEC_BIT_DEPTH_16 &&
        open_param->bit_depth != HA_CODEC_BIT_DEPTH_24) {
        sap_err_log_u32(open_param->bit_depth);
        return ERR_HACODEC_INVALID_BITDEPTH;
    }

    return AUDIO_SUCCESS;
}

static td_s32 lhdc_dec_init(lhdc_dec_instance *lhdc_dec)
{
    td_s32 ret, sample_size;

    /* Initial lhdcv5 decoder */
    ret = lhdcv5_util_init_decoder(lhdc_dec->handle, lhdc_dec->bit_depth, lhdc_dec->sample_rate,
        LHDCV5_AUDIO_FMT_OUT_PCM, LHDCV5_FRM_DURAION_5_MS, 0, VERSION_5);
    if (ret != LHDCV5_UTIL_DEC_SUCCESS) {
        sap_err_log_fun(lhdcv5_util_init_decoder, ret);
        return LHDC_DEC_INIT_DECODER_FAILED;
    }

    sample_size = lhdcv5_util_dec_get_sample_size(lhdc_dec->handle);
    if (sample_size < 0) {
        sap_err_log_fun(lhdcv5_util_dec_get_sample_size, sample_size);
        return sample_size;
    }
    lhdc_dec->samples = sample_size;

    ret = lhdcv5_util_dec_set_channel(lhdc_dec->handle, LHDCV5_OUTPUT_STEREO);
    if (ret != LHDCV5_UTIL_DEC_SUCCESS) {
        sap_err_log_fun(lhdcv5_util_dec_set_channel, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

static td_s32 ha_lhdc_dec_init(td_void **ph_decoder, const ha_codec_dec_param *open_param)
{
    td_s32 ret;
    td_u32 mem_size;
    lhdc_dec_instance *lhdc_dec;

    check_null_ptr(open_param);
    check_null_ptr(ph_decoder);

    /* check input param */
    ret = lhdc_dec_check_param(open_param);
    if (ret != AUDIO_SUCCESS) {
        return ret;
    }

    platform_set_lhdc5_license_info(open_param);

    lhdc_dec = (lhdc_dec_instance *)malloc(sizeof(lhdc_dec_instance));
    if (lhdc_dec == TD_NULL) {
        sap_err_log_info("lhdc_dec instance memory allocated failed");
        return ERR_HACODEC_ALLOC_MEM_FAILED;
    }

    lhdc_dec->sample_rate = open_param->sample_rate;
    lhdc_dec->bit_depth = open_param->bit_depth;
    lhdc_dec->channels = open_param->channels;

    /* Get the memory size required for LHDC-V decoding */
    ret = lhdcv5_util_dec_get_mem_req(VERSION_5, lhdc_dec->sample_rate,
        LHDCV5_AUDIO_FMT_OUT_PCM, LHDCV5_LOSSLESS_MODE_OFF, &mem_size);
    if (ret != LHDCV5_UTIL_DEC_SUCCESS) {
        sap_err_log_fun(lhdcv5_util_dec_get_mem_req, ret);
        goto out0;
    }

    lhdc_dec->handle = malloc(mem_size);
    if (lhdc_dec->handle == TD_NULL) {
        sap_err_log_fun(malloc, ret);
        ret = ERR_HACODEC_ALLOC_MEM_FAILED;
        goto out0;
    }
    (void)memset_s(lhdc_dec->handle, mem_size, 0, mem_size);

    /* Initial lhdcv5 decoder */
    ret = lhdc_dec_init(lhdc_dec);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(lhdc_dec_init, ret);
        goto out1;
    }

    *ph_decoder = lhdc_dec;
    return AUDIO_SUCCESS;

out1:
    if (ret != (td_s32)LHDC_DEC_INIT_DECODER_FAILED) {
        lhdcv5_util_dec_destroy(lhdc_dec->handle);
    }
    free(lhdc_dec->handle);
    lhdc_dec->handle = TD_NULL;
out0:
    free(lhdc_dec);
    lhdc_dec = TD_NULL;
    return ret;
}

static td_s32 ha_lhdc_dec_deinit(td_void *h_decoder)
{
    lhdc_dec_instance *lhdc_dec = (lhdc_dec_instance *)h_decoder;
    check_null_ptr(lhdc_dec);
    check_null_ptr(lhdc_dec->handle);

    lhdcv5_util_dec_destroy(lhdc_dec->handle);
    free(lhdc_dec->handle);
    lhdc_dec->handle = TD_NULL;
    free(lhdc_dec);
    lhdc_dec = TD_NULL;

    return AUDIO_SUCCESS;
}

static td_s32 ha_lhdc_dec_get_max_pcm_out_size(td_void *h_decoder, td_u32 *out_sizes)
{
    check_null_ptr(h_decoder);
    check_null_ptr(out_sizes);

    *out_sizes = LHDC_DEC_MAX_OUTPUT_BYTES;

    return AUDIO_SUCCESS;
}

static td_s32 ha_lhdc_dec_get_max_bits_out_size(td_void *h_decoder, td_u32 *out_sizes)
{
    check_null_ptr(h_decoder);
    check_null_ptr(out_sizes);

    *out_sizes = 0;

    return AUDIO_SUCCESS;
}

static td_void ha_lhdc_dec_set_data_attr(lhdc_dec_instance *lhdc_dec,
    ha_codec_dec_in_packet *apkt, ha_codec_dec_out_packet *out, td_u32 out_length)
{
    apkt->data += lhdc_dec->frm_info.frame_len;
    apkt->size -= lhdc_dec->frm_info.frame_len;
    out->pcm_out_buf_size = out_length;
    out->out_channels = lhdc_dec->channels;
    out->out_sample_rate = lhdc_dec->sample_rate;
    out->interleaved = TD_TRUE;
    out->bit_per_sample = lhdc_dec->bit_depth;
    out->pcm_out_samples = lhdc_dec->samples;
}

static td_s32 ha_lhdc_dec_process(lhdc_dec_instance *lhdc_dec, ha_codec_dec_in_packet *apkt,
    ha_codec_dec_out_packet *out, td_u32 *out_length)
{
    td_s32 ret;
    td_u8 *output = (td_u8 *)out->pcm_out_buf;
    td_u32 enc_bytes;
    enc_bytes = lhdc_dec->sample_rate == SAMPLE_RATE_44K ? LHDC_ENCODED_BYTES_176 : LHDC_ENCODED_BYTES_162;

    ret = lhdcv5_util_dec_fetch_frame_info(lhdc_dec->handle, lhdc_dec->scratch_buf, enc_bytes, &lhdc_dec->frm_info);
    if (ret != LHDCV5_UTIL_DEC_SUCCESS) {
        sap_err_log_fun(lhdcv5_util_dec_fetch_frame_info, ret);
        return ret;
    }
    if (apkt->size < lhdc_dec->frm_info.frame_len) {
        sap_err_log_u32(apkt->size);
        return ERR_HACODEC_NOT_ENOUGH_DATA;
    }

    ret = memcpy_s(lhdc_dec->scratch_buf, lhdc_dec->scratch_size, apkt->data, lhdc_dec->frm_info.frame_len);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return ERR_HACODEC_MEM_COPY_FAILED;
    }

    ret = lhdcv5_util_dec_process(lhdc_dec->handle, output, lhdc_dec->scratch_buf,
        lhdc_dec->frm_info.frame_len, out_length);
    if (ret != LHDCV5_UTIL_DEC_SUCCESS) {
        if (ret == LHDCV5_UTIL_DEC_ERROR) {
            apkt->data = apkt->data + 1;
            apkt->size = apkt->size - 1;
        }
        sap_err_log_fun(lhdcv5_util_dec_process, ret);
        return ERR_HACODEC_DECODE_FAILED;
    }

    return AUDIO_SUCCESS;
}

static td_s32 ha_lhdc_dec_decode_frame(td_void *h_decoder, ha_codec_dec_in_packet *apkt, ha_codec_dec_out_packet *out)
{
    td_s32 ret;
    lhdc_dec_instance *lhdc_dec = (lhdc_dec_instance *)h_decoder;
    td_u32 out_length;

    if (apkt->size < LHDC_DEC_MIN_INPUT_BYTES) {
        sap_err_log_u32(apkt->size);
        return ERR_HACODEC_NOT_ENOUGH_DATA;
    }

    ret = memcpy_s(lhdc_dec->scratch_buf, lhdc_dec->scratch_size, apkt->data, LHDC_DEC_MIN_INPUT_BYTES);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return ERR_HACODEC_MEM_COPY_FAILED;
    }
    clear_output(out);

    ret = ha_lhdc_dec_process(lhdc_dec, apkt, out, &out_length);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(ha_lhdc_dec_process, ret);
        return ret;
    }

    if (lhdc_dec->bit_depth == HA_CODEC_BIT_DEPTH_24) {
        td_u32 *out32 = (td_u32 *)out->pcm_out_buf;
        td_u32 i;
        for (i = 0; i < lhdc_dec->channels * lhdc_dec->samples; i++) {
            out32[i] <<= 8; /* 8 bit left shift to 24 bit MSB */
        }
    }

    ha_lhdc_dec_set_data_attr(lhdc_dec, apkt, out, out_length);
    return AUDIO_SUCCESS;
}

AUDIO_STATIC ha_codec_decode ha_audio_lhdc_decode_entry = {
    .name = (const td_char *)"lhdc",
    .codec_id = HA_CODEC_ID_LHDC,
    .version.version = 0x10000001,
    .description = (const td_char *)"lhdc decoder",
    .init = ha_lhdc_dec_init,
    .deinit = ha_lhdc_dec_deinit,
    .send_cmd = ha_lhdc_dec_send_cmd,
    .get_max_pcm_out_size = ha_lhdc_dec_get_max_pcm_out_size,
    .get_max_bits_out_size = ha_lhdc_dec_get_max_bits_out_size,
    .decode_frame = ha_lhdc_dec_decode_frame,
};

define_component(ADEC, ha_audio_lhdc_decode_entry);
