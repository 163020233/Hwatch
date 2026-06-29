/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: api of drv audio proc
 * Author: audio
 */

#include "securec.h"
#include "drv_audio_osal.h"
#include "audio_debug.h"
#include "audio_osal.h"
#include "drv_audio_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    ha_codec_id acodec_id;
    const td_char *acodec_name;
} acodec_proc_name_item;

typedef struct {
    audio_format format;
    const td_char *name;
} format_proc_name_item;

typedef struct {
    audio_scene scene;
    const td_char *name;
} scene_proc_name_item;

static const acodec_proc_name_item g_acodec_proc_name_list[] = {
    {HA_CODEC_ID_PCM, "pcm"},
    {HA_CODEC_ID_MP3, "mp3"},
    {HA_CODEC_ID_AAC, "aac"},
    {HA_CODEC_ID_SBC, "sbc"},
    {HA_CODEC_ID_MSBC, "msbc"},
    {HA_CODEC_ID_OPUS, "opus"},
    {HA_CODEC_ID_FLAC, "flac"},
    {HA_CODEC_ID_L2HC, "l2hc"},
    {HA_CODEC_ID_L2HC_VOICE, "l2hc_voice"},
    {HA_CODEC_ID_LC3, "lc3"},
    {HA_CODEC_ID_SILK, "silk"},
    {HA_CODEC_ID_AMRWB, "amrwb"},
    {HA_CODEC_ID_AMRNB, "amrnb"},
    {HA_CODEC_ID_VORBIS, "vorbis"},
    {HA_CODEC_ID_LHDC, "lhdc"},
    {HA_CODEC_ID_LDAC, "ldac"},
};

static const format_proc_name_item g_format_proc_name_list[] = {
    {AUDIO_FORMAT_STREAM, "stream"},
    {AUDIO_FORMAT_PCM, "pcm"},
    {AUDIO_FORMAT_AC3, "ac3"},
    {AUDIO_FORMAT_MPEG1, "mpeg1"},
    {AUDIO_FORMAT_MP3, "mp3"},
    {AUDIO_FORMAT_MPEG2, "mpeg2"},
    {AUDIO_FORMAT_AAC_LC, "aac_lc"},
    {AUDIO_FORMAT_DTS, "dts"},
    {AUDIO_FORMAT_ATRAC, "atrac"},
    {AUDIO_FORMAT_DSD, "dsd"},
    {AUDIO_FORMAT_EAC3, "eac3"},
    {AUDIO_FORMAT_DTS_HD, "dts_hd"},
    {AUDIO_FORMAT_MAT, "mat"},
    {AUDIO_FORMAT_DST, "dst"},
    {AUDIO_FORMAT_WMA_PRO, "wma_pro"},
    {AUDIO_FORMAT_CXT, "cxt"},
};

static const scene_proc_name_item g_scene_proc_name_list[] = {
    {AUDIO_SCENE_BT_MUSIC_SNK, "a2dp_snk"},
    {AUDIO_SCENE_BT_VOIP, "bt_voip"},
    {AUDIO_SCENE_LOCAL_TONE, "tone"},
    {AUDIO_SCENE_SYNC_TONE, "sync_tone"},
    {AUDIO_SCENE_LIVE_MIC, "bmic"},
    {AUDIO_SCENE_LIVE_MIC_REC, "bmic_rec"},
    {AUDIO_SCENE_BT_MUSIC_SRC, "a2dp_src"},
    {AUDIO_SCENE_LOCAL_MUSIC, "local"},
    {AUDIO_SCENE_BT_TONE, "bt_tone"},
    {AUDIO_SCENE_SLE_MUSIC_SRC, "slm_src"},
    {AUDIO_SCENE_SLE_VOIP, "sle_voip"},
    {AUDIO_SCENE_SLE_MUSIC_SINK, "slm_sink"},
};

td_char *drv_audio_proc_get_i2s_mode(audio_i2s_mode mode)
{
    if (mode == I2S_STD_MODE) {
        return "i2s_std";
    } else if (mode == I2S_PCM_MODE) {
        return "i2s_pcm";
    } else if (mode == I2S_TDM_MODE) {
        return "i2s_tdm";
    } else if (mode == I2S_PDM_MODE) {
        return "i2s_pdm";
    } else {
        return "null";
    }
}

td_char *drv_audio_proc_get_i2s_mclk(audio_i2s_mclk mclk)
{
    if (mclk == I2S_MCLK_128_FS) {
        return "128*fs";
    } else if (mclk == I2S_MCLK_256_FS) {
        return "256*fs";
    } else if (mclk == I2S_MCLK_384_FS) {
        return "384*fs";
    } else if (mclk == I2S_MCLK_512_FS) {
        return "512*fs";
    } else if (mclk == I2S_MCLK_768_FS) {
        return "768*fs";
    } else if (mclk == I2S_MCLK_1024_FS) {
        return "1024*fs";
    } else if (mclk == I2S_MCLK_1536_FS) {
        return "1536*fs";
    } else {
        return "null";
    }
}

#ifdef SAP_PROC_SUPPORT
td_s32 drv_audio_proc_print_head(td_void *file, const td_char *format, ...)
{
    td_s32 i;
    td_s32 len;
    va_list args;

    td_char *title = TD_NULL;
    td_char head_title_format[] = {
        [0 ... PROC_MAX_TITLE_FORMAT_LEN - 1] = '=', '\n', '\0'
    };

    title = (td_char *)audio_osal_calloc(sizeof(td_char) * (PROC_MAX_TITLE_FORMAT_LEN + 1));
    if (title == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }
    va_start(args, format);
    len = vsnprintf_s(title, PROC_MAX_TITLE_FORMAT_LEN, PROC_MAX_TITLE_FORMAT_LEN - 1, format, args);
    va_end(args);

    if (len < 0) {
        sap_err_log_fun(vsnprintf_s, len);
        audio_osal_kfree(title);
        return AUDIO_FAILURE;
    }

    for (i = PROC_TITLE_PREFIX_LEN; i < PROC_TITLE_PREFIX_LEN + len; i++) {
        head_title_format[i] = title[i - PROC_TITLE_PREFIX_LEN];
    }

    drv_audio_osal_proc_print(file, "%s", head_title_format);
    audio_osal_kfree(title);

    return AUDIO_SUCCESS;
}

td_void drv_audio_proc_print_tail(td_void *file)
{
    td_s32 i;
    td_char title_format[PROC_MAX_TITLE_FORMAT_LEN + 2] = {'\0'}; /* 2: add '\n'&'\0' */

    for (i = 0; i < (PROC_MAX_TITLE_FORMAT_LEN - 1); i += 2) { /* 2: one '=' + one space */
        title_format[i] = '=';
        title_format[i + 1] = ' ';
    }
    title_format[PROC_MAX_TITLE_FORMAT_LEN] = '\n';

    drv_audio_osal_proc_print(file, "%s", title_format);
}

td_s32 drv_audio_proc_print_title(td_void *file, const td_char *format, ...)
{
    td_s32 i;
    td_s32 len;
    va_list args;

    td_char *title = TD_NULL;
    td_char title_format[] = {
        [0 ... PROC_MAX_TITLE_FORMAT_LEN - 1] = '-', '\n', '\0'
    };

    title = (td_char *)audio_osal_calloc(sizeof(td_char) * (PROC_MAX_TITLE_FORMAT_LEN + 1));
    if (title == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }
    va_start(args, format);
    len = vsnprintf_s(title, PROC_MAX_TITLE_FORMAT_LEN, PROC_MAX_TITLE_FORMAT_LEN - 1, format, args);
    va_end(args);

    if (len < 0) {
        sap_err_log_fun(vsnprintf_s, len);
        audio_osal_kfree(title);
        return AUDIO_FAILURE;
    }

    for (i = PROC_TITLE_PREFIX_LEN; i < PROC_TITLE_PREFIX_LEN + len; i++) {
        title_format[i] = title[i - PROC_TITLE_PREFIX_LEN];
    }

    drv_audio_osal_proc_print(file, "%s", title_format);
    audio_osal_kfree(title);

    return AUDIO_SUCCESS;
}
#endif

const td_char *drv_audio_proc_get_acodec_name(ha_codec_id id)
{
    td_u32 i;

    for (i = 0; i < (sizeof(g_acodec_proc_name_list) / sizeof(g_acodec_proc_name_list[0])); i++) {
        if (id == g_acodec_proc_name_list[i].acodec_id) {
            return g_acodec_proc_name_list[i].acodec_name;
        }
    }

    return "unknown";
}

const td_char *drv_audio_proc_get_format_name(audio_format format)
{
    td_u32 i;

    for (i = 0; i < (sizeof(g_format_proc_name_list) / sizeof(g_format_proc_name_list[0])); i++) {
        if (format == g_format_proc_name_list[i].format) {
            return g_format_proc_name_list[i].name;
        }
    }

    return "unknown";
}

const td_char *drv_audio_proc_get_scene_name(audio_scene scene)
{
    td_u32 i;

    for (i = 0; i < (sizeof(g_scene_proc_name_list) / sizeof(g_scene_proc_name_list[0])); i++) {
        if (scene == g_scene_proc_name_list[i].scene) {
            return g_scene_proc_name_list[i].name;
        }
    }

    return "unknown";
}

const td_char *drv_audio_proc_get_vol_type(audio_vol_type type)
{
    if (type == AUDIO_VOL_ANA_PGA) {
        return "ana_pga";
    } else if (type == AUDIO_VOL_DIG_PGA) {
        return "dig_pga";
    } else if (type == AUDIO_VOL_SOFT_GAIN) {
        return "soft_gain";
    } else {
        return "null";
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
