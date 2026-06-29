/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: core util
 * Author: audio
 */

#include "audio_struct.h"
#include "audio_debug.h"
#include "core_util.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 core_check_sample_rate(audio_sample_rate sample_rate)
{
    td_u32 i;
    const td_u32 sample_rate_table[] = {
        SAMPLE_RATE_4K,
        SAMPLE_RATE_8K,
        SAMPLE_RATE_11K,
        SAMPLE_RATE_12K,
        SAMPLE_RATE_16K,
        SAMPLE_RATE_22K,
        SAMPLE_RATE_24K,
        SAMPLE_RATE_32K,
        SAMPLE_RATE_44K,
        SAMPLE_RATE_48K,
        SAMPLE_RATE_88K,
        SAMPLE_RATE_96K,
        SAMPLE_RATE_176K,
        SAMPLE_RATE_192K,
        SAMPLE_RATE_288K,
        SAMPLE_RATE_384K,
        SAMPLE_RATE_768K,
    };

    for (i = 0; i < sizeof(sample_rate_table) / sizeof(sample_rate_table[0]); i++) {
        if (sample_rate_table[i] == sample_rate) {
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_u32(sample_rate);
    return ERR_SYS_INVALID_PARAM;
}

td_s32 core_check_sample_rate_range(td_u32 sample_rate)
{
    if ((sample_rate < SAMPLE_RATE_4K) || (sample_rate > SAMPLE_RATE_768K)) {
        sap_err_log_s32(sample_rate);
        return ERR_SYS_INVALID_PARAM;
    }

    return AUDIO_SUCCESS;
}

td_s32 core_check_bit_depth(audio_bit_depth bit_depth)
{
    td_u32 i;
    const td_u32 bit_depth_table[] = {
        BIT_DEPTH_16,
        BIT_DEPTH_24,
        BIT_DEPTH_32,
        BIT_DEPTH_32F,
    };

    for (i = 0; i < sizeof(bit_depth_table) / sizeof(bit_depth_table[0]); i++) {
        if (bit_depth_table[i] == bit_depth) {
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_u32(bit_depth);
    return ERR_SYS_INVALID_PARAM;
}

td_s32 core_check_channel(audio_channel channel)
{
    td_u32 i;
    const td_u32 channel_table[] = {
        AUDIO_CH_1,
        AUDIO_CH_2,
        AUDIO_CH_4,
        AUDIO_CH_6,
        AUDIO_CH_8,
        AUDIO_CH_16,
    };

    for (i = 0; i < sizeof(channel_table) / sizeof(channel_table[0]); i++) {
        if (channel_table[i] == channel) {
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_u32(channel);
    return ERR_SYS_INVALID_PARAM;
}

td_s32 core_check_ao_port(ao_port_id port)
{
    td_u32 i;
    const td_u32 port_table[] = {
        AO_PORT_DAC0,
        AO_PORT_DAC1,
        AO_PORT_DAC2,
        AO_PORT_DAC3,
        AO_PORT_I2S0,
        AO_PORT_I2S1,
        AO_PORT_I2S2,
        AO_PORT_I2S3,
        AO_PORT_I2S4,
        AO_PORT_SPDIF0,
        AO_PORT_HDMI0,
        AO_PORT_HDMI1,
        AO_PORT_ARC0,
        AO_PORT_ARC1,
        AO_PORT_USB0,
        AO_PORT_USB1,
        AO_PORT_BT0,
        AO_PORT_BT1,
        AO_PORT_CAST0,
        AO_PORT_CAST1,
    };

    for (i = 0; i < sizeof(port_table) / sizeof(port_table[0]); i++) {
        if (port_table[i] == port) {
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_h32(port);
    return ERR_SYS_INVALID_PARAM;
}

td_s32 core_check_bool(td_bool val)
{
    if (val != TD_FALSE && val != TD_TRUE) {
        sap_err_log_s32(val);
        return ERR_SYS_INVALID_PARAM;
    }

    return AUDIO_SUCCESS;
}

static td_s32 core_check_i2s_mode(audio_i2s_mode i2s_mode)
{
    if (i2s_mode <= I2S_PDM_MODE) {
        return AUDIO_SUCCESS;
    }

    sap_err_log_h32(i2s_mode);
    return ERR_AI_INVALID_PARAM;
}

static td_s32 core_check_i2s_mclk(audio_i2s_mclk i2s_mclk)
{
    if (i2s_mclk < I2S_MCLK_MAX) {
        return AUDIO_SUCCESS;
    }

    sap_err_log_h32(i2s_mclk);
    return ERR_AI_INVALID_PARAM;
}

static td_s32 core_check_i2s_bclk(audio_i2s_bclk i2s_bclk)
{
    td_u32 i;
    const td_u32 i2s_bclk_table[] = {
        I2S_BCLK_1_DIV,
        I2S_BCLK_2_DIV,
        I2S_BCLK_3_DIV,
        I2S_BCLK_4_DIV,
        I2S_BCLK_6_DIV,
        I2S_BCLK_8_DIV,
        I2S_BCLK_12_DIV,
        I2S_BCLK_16_DIV,
        I2S_BCLK_24_DIV,
        I2S_BCLK_32_DIV,
        I2S_BCLK_48_DIV,
        I2S_BCLK_64_DIV,
        I2S_BCLK_96_DIV,
    };

    for (i = 0; i < sizeof(i2s_bclk_table) / sizeof(i2s_bclk_table[0]); i++) {
        if (i2s_bclk_table[i] == i2s_bclk) {
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_h32(i2s_bclk);
    return ERR_AI_INVALID_PARAM;
}

static td_s32 core_check_i2s_bit_depth(audio_bit_depth i2s_bit_depth)
{
    td_u32 i;
    const td_u32 i2s_bit_depth_table[] = {
        BIT_DEPTH_8,
        BIT_DEPTH_16,
        BIT_DEPTH_18,
        BIT_DEPTH_20,
        BIT_DEPTH_24,
        BIT_DEPTH_32,
    };

    for (i = 0; i < sizeof(i2s_bit_depth_table) / sizeof(i2s_bit_depth_table[0]); i++) {
        if (i2s_bit_depth_table[i] == i2s_bit_depth) {
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_h32(i2s_bit_depth);
    return ERR_AI_INVALID_PARAM;
}

static td_s32 core_check_i2s_channel(audio_channel i2s_channel)
{
    td_u32 i;
    const td_u32 i2s_channel_table[] = {
        AUDIO_CH_1,
        AUDIO_CH_2,
        AUDIO_CH_4,
        AUDIO_CH_6,
        AUDIO_CH_8,
    };

    for (i = 0; i < sizeof(i2s_channel_table) / sizeof(i2s_channel_table[0]); i++) {
        if (i2s_channel_table[i] == i2s_channel) {
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_h32(i2s_channel);
    return ERR_AI_INVALID_PARAM;
}

static td_s32 core_check_i2s_pcm_delay(audio_i2s_pcm_delay i2s_pcm_delay)
{
    td_u32 i;
    const td_u32 i2s_pcm_delay_table[] = {
        I2S_PCM_0_DELAY,
        I2S_PCM_1_DELAY,
        I2S_PCM_8_DELAY,
        I2S_PCM_16_DELAY,
        I2S_PCM_17_DELAY,
        I2S_PCM_24_DELAY,
        I2S_PCM_32_DELAY,
    };

    for (i = 0; i < sizeof(i2s_pcm_delay_table) / sizeof(i2s_pcm_delay_table[0]); i++) {
        if (i2s_pcm_delay_table[i] == i2s_pcm_delay) {
            return AUDIO_SUCCESS;
        }
    }

    sap_err_log_h32(i2s_pcm_delay);
    return ERR_AI_INVALID_PARAM;
}

td_s32 core_check_i2s_attr(const audio_i2s_attr *i2s_attr)
{
    td_s32 ret;

    ret = core_check_bool(i2s_attr->master);
    if (ret != AUDIO_SUCCESS) {
        return ERR_SYS_INVALID_PARAM;
    }

    ret = core_check_i2s_mode(i2s_attr->i2s_mode);
    if (ret != AUDIO_SUCCESS) {
        return ERR_SYS_INVALID_PARAM;
    }

    ret = core_check_i2s_mclk(i2s_attr->mclk);
    if (ret != AUDIO_SUCCESS) {
        return ERR_SYS_INVALID_PARAM;
    }

    ret = core_check_i2s_bclk(i2s_attr->bclk);
    if (ret != AUDIO_SUCCESS) {
        return ERR_SYS_INVALID_PARAM;
    }

    ret = core_check_i2s_bit_depth(i2s_attr->bit_depth);
    if (ret != AUDIO_SUCCESS) {
        return ERR_SYS_INVALID_PARAM;
    }

    ret = core_check_i2s_channel(i2s_attr->channels);
    if (ret != AUDIO_SUCCESS) {
        return ERR_SYS_INVALID_PARAM;
    }

    ret = core_check_bool(i2s_attr->sample_rise_edge);
    if (ret != AUDIO_SUCCESS) {
        return ERR_SYS_INVALID_PARAM;
    }

    ret = core_check_i2s_pcm_delay(i2s_attr->pcm_delay_cycle);
    if (ret != AUDIO_SUCCESS) {
        return ERR_AI_INVALID_PARAM;
    }
    return AUDIO_SUCCESS;
}

td_u32 sample_size(td_u32 bit_depth)
{
    switch (bit_depth) {
        case BIT_DEPTH_8:
            return sizeof(td_s8);

        case BIT_DEPTH_16:
            return sizeof(td_s16);

        case BIT_DEPTH_24:
        case BIT_DEPTH_32:
        case BIT_DEPTH_32F:
            return sizeof(td_s32);

        case BIT_DEPTH_64:
            return sizeof(td_s64);

        default:
            return 0;
    }
}

/* how many bytes per ms */
td_u32 calc_frame_size_per_ms(const audio_pcm_format *pcm_format)
{
    td_u32 sample_size;

    if (pcm_format->sample_rate < SAMPLE_RATE_4K || pcm_format->sample_rate > SAMPLE_RATE_768K) {
        return 0;
    }

    if (pcm_format->channels < AUDIO_CH_MONO || pcm_format->channels > AUDIO_CH_16) {
        return 0;
    }

    sample_size = calc_sample_size(pcm_format->channels, pcm_format->bit_depth);
    return (sample_size * pcm_format->sample_rate) / AUDIO_MS_NUM_PER_SECOND;
}

/* how many bytes per frame */
td_u32 calc_frame_size_per_frame(const audio_pcm_format *pcm_format)
{
    if (pcm_format->channels < AUDIO_CH_MONO || pcm_format->channels > AUDIO_CH_16) {
        return 0;
    }

    /* 帧长校验为0-40ms */
    if (pcm_format->samples_per_frame == 0 ||
        (pcm_format->samples_per_frame > pcm_format->sample_rate / AUDIO_40MS_NUM_PER_SECOND)) {
        return 0;
    }

    return calc_sample_size(pcm_format->channels, pcm_format->bit_depth) * pcm_format->samples_per_frame;
}

/* how many samples per frame */
td_u32 calc_samples_per_frame(td_u32 frame_size, td_u32 channels, td_u32 bit_depth)
{
    td_u32 sample_size = calc_sample_size(channels, bit_depth);
    if (sample_size == 0) {
        return 0;
    }

    return (frame_size / sample_size);
}

td_u32 calc_sample_size(td_u32 channels, td_u32 bit_depth)
{
    return sample_size(bit_depth) * channels;
}

td_u32 calc_pcm_frame_ms(const audio_pcm_format *pcm_format, td_u32 pcm_size)
{
    td_u32 size_per_ms;

    size_per_ms = calc_frame_size_per_ms(pcm_format);
    if (size_per_ms == 0) {
        return 0;
    }

    return pcm_size / size_per_ms;
}

/* how many bytes per frame */
td_u32 calc_audio_frame_size_per_frame(const audio_frame *frame)
{
    if (frame == TD_NULL) {
        return 0;
    }

    if (frame->channels < AUDIO_CH_MONO || frame->channels > AUDIO_CH_16) {
        return 0;
    }

    return calc_sample_size(frame->channels, frame->bit_depth) * frame->pcm_samples;
}

td_u8 audio_crc8(td_u8 *message, td_u8 len)
{
    td_u8 crc;
    td_u8 i;
    td_u8 crc_len = len;
    td_u8 *crc_message = message;
    crc = 0;
    while ((crc_len--) != 0) {
        crc ^= *crc_message++;
        for (i = 0; i < 8; i++) { /* CRC 8 algorithm */
            if ((crc & 0x01) != 0) {
                crc = (crc >> 1) ^ 0x8c;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/* Table of CRC-32's of all single-byte values, IEEE 802.3 (0x04C11DB7) */
static const td_u32 g_audio_crc32_tab[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
    0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
    0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
    0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
    0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
    0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
    0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
    0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
    0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
    0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
    0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
    0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
    0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
    0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
    0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
    0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
    0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
    0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
    0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
    0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
    0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
    0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
    0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
    0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
    0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
    0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
    0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL,
};

static td_u32 audio_crc32_no_comp(td_u32 crc_start, const td_u8 *buffer, td_u32 length)
{
    td_u32 crc = crc_start;
    const td_u32 *tab = g_audio_crc32_tab;
    const td_u32 *buf = (const td_u32 *)buffer;
    td_u32 len = length;
    td_u32 rem_len;

    if (buffer == NULL) {
        return crc;
    }

    /* Align it */
    if (((((uintptr_t)buf) & 3) != 0) && (len > 0)) {              /* 3: for align to 4 byte address. */
        const td_u8 *p = (const td_u8 *)buf;
        do {
            crc = tab[(crc ^ (*p++)) & 0xFF] ^ (crc >> 8);  /* Array size: 0xFF, bit size 8. */
        } while ((--len > 0) && ((((uintptr_t)p) & 3) != 0));      /* 3: for align to 4 byte address. */
        buf = (const td_u32 *)p;
    }

    rem_len = len & 0x3;
    len >>= 2;  /* right shift 2 bit, lenght of td_u8 array to td_u32 array */
    for (; len > 0; len--) {
        /* load data 32 bits wide, xor data 32 bits wide. */
        crc ^= *buf++;    /* use pre increment for speed */
        crc = tab[(crc ^ (0)) & 0xFF] ^ (crc >> 8);         /* Array size: 0xFF, bit size 8. */
        crc = tab[(crc ^ (0)) & 0xFF] ^ (crc >> 8);         /* Array size: 0xFF, bit size 8. */
        crc = tab[(crc ^ (0)) & 0xFF] ^ (crc >> 8);         /* Array size: 0xFF, bit size 8. */
        crc = tab[(crc ^ (0)) & 0xFF] ^ (crc >> 8);         /* Array size: 0xFF, bit size 8. */
    }

    /* Process the last few bytes */
    if (rem_len > 0) {
        const td_u8 *p = (const td_u8 *)buf;
        do {
            crc = tab[(crc ^ (*p++)) & 0xFF] ^ (crc >> 8);  /* Array size: 0xFF, bit size 8. */
        } while (--rem_len > 0);
    }

    return crc;
}

td_u32 audio_crc32(td_u32 crc_start, const td_u8 *buffer, td_u32 length)
{
    return audio_crc32_no_comp(crc_start ^ 0xFFFFFFFFL, buffer, length) ^ 0xFFFFFFFFL;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
