/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: aef sample
 * Author: audio
 */

#include <stdlib.h>
#include <string.h>

#include "soc_uapi_sound.h"
#include "soc_uapi_aef_peq.h"
#include "sample_audio_api.h"
#include "soc_uapi_aef_drc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_char *sample;
    td_char *aef_type;
    td_char *enable;
#ifdef SAP_EFFECT_SOFT_EQ_TYPE_SWITCH_SUPPORT
    td_char *aef_filter;
#endif
} sample_aef_arg;

static td_void sample_aef_close_sound(td_handle sound)
{
    td_s32 ret;

    ret = uapi_snd_close(sound);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_close, ret);
        return;
    }
}

static td_s32 sample_aef_open_sound(td_handle *sound)
{
    td_s32 ret;
    uapi_snd_attr snd_attr;

    ret = uapi_snd_get_default_attr(UAPI_SND_0, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_default_attr, ret);
        return ret;
    }

    ret = uapi_snd_open(sound, UAPI_SND_0, &snd_attr);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_open, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

/*
 * WDRC音效开关控制
 * sample_aef wdrc on  -- 打开WDRC音效
 * sample_aef wdrc off -- 关闭WDRC音效
 */
static td_s32 sample_aef_wdrc_entry(const sample_aef_arg *arg)
{
    td_s32 ret;
    td_handle sound;
    td_bool enable;

    if (strcmp(arg->enable, "on") == 0) {
        enable = TD_TRUE;
    } else if (strcmp(arg->enable, "off") == 0) {
        enable = TD_FALSE;
    } else {
        return EXT_FAILURE;
    }

    ret = sample_aef_open_sound(&sound);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_aef_open_sound, ret);
        return ret;
    }

    ret = uapi_snd_set_port_aef_enable(sound, UAPI_SND_OUT_PORT_ALL, UAPI_AEF_TYPE_WDRC, enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_port_aef_enable, ret);
    }

    sample_aef_close_sound(sound);

    return EXT_SUCCESS;
}

#ifdef SAP_EFFECT_HW_EQ_V1_SUPPORT

typedef struct {
    sample_aef_arg basic;
    td_char *bq_coef;
    td_char *ch;
} sample_aef_peq_arg;

static const uapi_audio_bq_coef g_bq_coef_lp = {
    0xF041530D, 0x07C0AA5B, 0x00007F5A, 0x0000FEB4, 0x00007F5A
};

static const uapi_audio_bq_coef g_bq_coef_hp = {
    0xF041530D, 0x07C0AA5B, 0x07DFD5D4, 0xF0405459, 0x07DFD5D4
};

static td_s32 sample_aef_com_eq_rw_param(td_handle sound, uapi_peq_cmd cmd, const uapi_audio_bq_coef *coef)
{
    td_s32 ret;
    uapi_snd_com_eq_coef eq_coef;
    if (cmd != UAPI_PEQ_RW_COM_EQ_CH0_COEF_CMD && cmd != UAPI_PEQ_RW_COM_EQ_CH1_COEF_CMD) {
        sap_printf("unsupported channel.");
        return EXT_FAILURE;
    }

    ret = uapi_snd_get_aef_param(sound, UAPI_SND_OUT_PORT_DAC0, cmd, &eq_coef, sizeof(eq_coef));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return ret;
    }

    sap_err_log_h32(eq_coef.global_gain);
    eq_coef.bypass = 0;
    (td_void)memcpy_s(&(eq_coef.bq[0]), sizeof(uapi_audio_bq_coef), coef, sizeof(*coef));

    ret = uapi_snd_set_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        cmd, &eq_coef, sizeof(eq_coef));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_param, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_aef_calib_eq_rw_param(td_handle sound)
{
    td_s32 ret;
    uapi_snd_calib_eq_coef eq_coef;

    ret = uapi_snd_get_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        UAPI_PEQ_RW_CALIB_EQ_CH0_COEF_CMD, &eq_coef, sizeof(eq_coef));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return ret;
    }

    sap_err_log_h32(eq_coef.global_gain);
    (td_void)memcpy_s(&(eq_coef.bq[0]), sizeof(uapi_audio_bq_coef),
        &g_bq_coef_hp, sizeof(g_bq_coef_hp));

    ret = uapi_snd_set_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        UAPI_PEQ_RW_CALIB_EQ_CH0_COEF_CMD, &eq_coef, sizeof(eq_coef));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_param, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_aef_peq_rw_param(td_handle sound, const sample_aef_peq_arg *arg)
{
    td_s32 ret;
    const uapi_audio_bq_coef *coef;
    td_u32 cmd_no = (td_u32)strtol(arg->ch, TD_NULL, 0);

    if (strcmp(arg->bq_coef, "hp") == 0) {
        coef = &g_bq_coef_hp;
    } else if (strcmp(arg->bq_coef, "lp") == 0) {
        coef = &g_bq_coef_lp;
    } else {
        return EXT_FAILURE;
    }

    ret = sample_aef_com_eq_rw_param(sound, uapi_aef_cmd(UAPI_AEF_TYPE_PEQ, cmd_no), coef);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_aef_com_eq_rw_param, ret);
        return ret;
    }

    ret = sample_aef_calib_eq_rw_param(sound);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_aef_calib_eq_rw_param, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

static td_s32 sample_aef_peq_entry(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_handle sound;
    td_bool enable;

    sample_aef_peq_arg *arg = (sample_aef_peq_arg *)argv;
    if (argc != (td_s32)(sizeof(sample_aef_peq_arg) / sizeof(td_char *))) {
        return EXT_FAILURE;
    }

    if (strcmp(arg->basic.enable, "on") == 0) {
        enable = TD_TRUE;
    } else if (strcmp(arg->basic.enable, "off") == 0) {
        enable = TD_FALSE;
    } else {
        return EXT_FAILURE;
    }

    ret = sample_aef_open_sound(&sound);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_aef_open_sound, ret);
        return ret;
    }

    ret = uapi_snd_set_port_aef_enable(sound, UAPI_SND_OUT_PORT_DAC0, UAPI_AEF_TYPE_PEQ, enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_port_aef_enable, ret);
        sample_aef_close_sound(sound);
        return ret;
    }

    if (enable == TD_TRUE) {
        (td_void)sample_aef_peq_rw_param(sound, arg);
    }

    sample_aef_close_sound(sound);
    return EXT_SUCCESS;
}

#else

#ifdef SAP_EFFECT_SOFT_EQ_TYPE_SWITCH_SUPPORT

typedef enum {
    SAMPLE_AEF_FILTER_TYPE_HP = 0,
    SAMPLE_AEF_FILTER_TYPE_LS = 1,
    SAMPLE_AEF_FILTER_TYPE_PK = 2,
    SAMPLE_AEF_FILTER_TYPE_HS = 3,
    SAMPLE_AEF_FILTER_TYPE_LP = 4,
    SAMPLE_AEF_FILTER_TYPE_COMMON = 5,
    SAMPLE_AEF_FILTER_TYPE_MAX
} sample_aef_filter_type;

static const uapi_snd_peq_attr g_peq_attr_hp = {
    .band_num = 5,
    .band_attr = {
        // 陡峭高通滤波器：200Hz以下完全衰减
        {UAPI_PEQ_FILTER_TYPE_HP, 200, 7, 0},      // HP: 200Hz, Q=0.7, 0dB
        {UAPI_PEQ_FILTER_TYPE_PK, 500, 20, 3000},  // PK: +3dB @500Hz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 3000, 20, 3000}, // PK: +3dB @3kHz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 5000, 20, 0},    // PK: 0dB @5kHz
        {UAPI_PEQ_FILTER_TYPE_PK, 10000, 20, -2000} // PK: -2dB @10kHz
    }
};

static const uapi_snd_peq_attr g_peq_attr_ls = {
    .band_num = 5,
    .band_attr = {
        // 低架滤波器：100Hz低频增强
        {UAPI_PEQ_FILTER_TYPE_LS, 100, 10, 15000},  // LS: +15dB @100Hz, Q=1.0
        {UAPI_PEQ_FILTER_TYPE_PK, 500, 20, -3000},  // PK: -3dB @500Hz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 3000, 20, -3000}, // PK: -3dB @3kHz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 5000, 20, 0},     // PK: 0dB @5kHz
        {UAPI_PEQ_FILTER_TYPE_PK, 10000, 20, -2000} // PK: -2dB @10kHz
    }
};

static const uapi_snd_peq_attr g_peq_attr_pk = {
    .band_num = 5,
    .band_attr = {
        {UAPI_PEQ_FILTER_TYPE_PK, 120, 20, -3000},  // PK: -3dB @120Hz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 1000, 20, 15000}, // PK: +15dB @1kHz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 3000, 20, 15000}, // PK: +15dB @3kHz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 8000, 20, -15000}, // PK: -15dB @8kHz
        {UAPI_PEQ_FILTER_TYPE_PK, 10000, 20, 0}     // PK: 0dB @10kHz
    }
};

static const uapi_snd_peq_attr g_peq_attr_hs = {
    .band_num = 5,
    .band_attr = {
        {UAPI_PEQ_FILTER_TYPE_PK, 120, 20, -3000},  // PK: -3dB @120Hz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 500, 20, -3000},  // PK: -3dB @500Hz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 3000, 20, -3000}, // PK: -3dB @3kHz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_HS, 8000, 10, 15000}, // HS: +15dB @8kHz, Q=1.0
        {UAPI_PEQ_FILTER_TYPE_PK, 10000, 20, 15000} // PK: +15dB @10kHz
    }
};

static const uapi_snd_peq_attr g_peq_attr_lp = {
    .band_num = 5,
    .band_attr = {
        // 低通滤波器：保留低频,衰减高频
        {UAPI_PEQ_FILTER_TYPE_LP, 500, 7, 0},      // LP: 500Hz, Q=0.7, 0dB
        {UAPI_PEQ_FILTER_TYPE_PK, 500, 20, 3000},  // PK: +3dB @500Hz, Q=2.0
        {UAPI_PEQ_FILTER_TYPE_PK, 3000, 20, -15000}, // PK: -3dB @3kHz
        {UAPI_PEQ_FILTER_TYPE_PK, 8000, 20, -15000}, // PK: -6dB @8kHz
        {UAPI_PEQ_FILTER_TYPE_PK, 10000, 20, -15000}, // PK: 0dB @10kHz
    }
};

static const uapi_snd_peq_attr g_peq_attr_common = {
    .band_num = 10,
    .band_attr = {
        {UAPI_PEQ_FILTER_TYPE_PK, 175,  25,  5200},
        {UAPI_PEQ_FILTER_TYPE_PK, 62,   50, 4000},
        {UAPI_PEQ_FILTER_TYPE_PK, 100,  50, 2000},
        {UAPI_PEQ_FILTER_TYPE_PK, 250,  70, 0},
        {UAPI_PEQ_FILTER_TYPE_PK, 424,  70, -15000},
        {UAPI_PEQ_FILTER_TYPE_PK, 450,  70, -3800},
        {UAPI_PEQ_FILTER_TYPE_PK, 1965, 5,  -1900},
        {UAPI_PEQ_FILTER_TYPE_PK, 4931, 5,  -1300},
        {UAPI_PEQ_FILTER_TYPE_PK, 20139, 5, -900},
        {UAPI_PEQ_FILTER_TYPE_PK, 280,  70, -14100},
    }
};

static td_s32 sample_aef_peq_rw_param(td_handle sound, td_u32 aef_filter)
{
    td_s32 ret;
    uapi_snd_peq_attr peq_attr;

    ret = uapi_snd_get_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        UAPI_PEQ_RW_PARAM_CMD, &peq_attr, sizeof(peq_attr));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return ret;
    }

    sap_err_log_u32(peq_attr.band_num);

    if (aef_filter == SAMPLE_AEF_FILTER_TYPE_HP) {
        (td_void)memcpy_s(&peq_attr, sizeof(uapi_snd_peq_attr), &g_peq_attr_hp, sizeof(uapi_snd_peq_attr));
    } else if (aef_filter == SAMPLE_AEF_FILTER_TYPE_LS) {
        (td_void)memcpy_s(&peq_attr, sizeof(uapi_snd_peq_attr), &g_peq_attr_ls, sizeof(uapi_snd_peq_attr));
    } else if (aef_filter == SAMPLE_AEF_FILTER_TYPE_PK) {
        (td_void)memcpy_s(&peq_attr, sizeof(uapi_snd_peq_attr), &g_peq_attr_pk, sizeof(uapi_snd_peq_attr));
    } else if (aef_filter == SAMPLE_AEF_FILTER_TYPE_HS) {
        (td_void)memcpy_s(&peq_attr, sizeof(uapi_snd_peq_attr), &g_peq_attr_hs, sizeof(uapi_snd_peq_attr));
    } else if (aef_filter == SAMPLE_AEF_FILTER_TYPE_LP) {
        (td_void)memcpy_s(&peq_attr, sizeof(uapi_snd_peq_attr), &g_peq_attr_lp, sizeof(uapi_snd_peq_attr));
    } else if (aef_filter == SAMPLE_AEF_FILTER_TYPE_COMMON) {
        (td_void)memcpy_s(&peq_attr, sizeof(uapi_snd_peq_attr), &g_peq_attr_common, sizeof(uapi_snd_peq_attr));
    }

    ret = uapi_snd_set_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        UAPI_PEQ_RW_PARAM_CMD, &peq_attr, sizeof(peq_attr));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_param, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

#else

static td_s32 sample_aef_peq_rw_param(td_handle sound)
{
    td_s32 ret;
    uapi_snd_peq_attr peq_attr;

    ret = uapi_snd_get_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        UAPI_PEQ_RW_PARAM_CMD, &peq_attr, sizeof(peq_attr));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return ret;
    }

    sap_err_log_u32(peq_attr.band_num);

    ret = uapi_snd_set_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        UAPI_PEQ_RW_PARAM_CMD, &peq_attr, sizeof(peq_attr));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_param, ret);
        return ret;
    }

    return EXT_SUCCESS;
}

#endif
/*
 * PEQ音效开关控制
 * sample_aef peq on  -- 打开PEQ音效
 * sample_aef peq off -- 关闭PEQ音效
 */
static td_s32 sample_aef_peq_entry(const sample_aef_arg *arg)
{
    td_s32 ret;
    td_handle sound;
    td_bool enable;

#ifdef SAP_EFFECT_SOFT_EQ_TYPE_SWITCH_SUPPORT
    td_u32 aef_filter;
#endif

    if (strcmp(arg->enable, "on") == 0) {
        enable = TD_TRUE;

#ifdef SAP_EFFECT_SOFT_EQ_TYPE_SWITCH_SUPPORT
        aef_filter = atoi(arg->aef_filter);
        if (aef_filter >= SAMPLE_AEF_FILTER_TYPE_MAX) {
            return EXT_FAILURE;
        }
#endif
    } else if (strcmp(arg->enable, "off") == 0) {
        enable = TD_FALSE;
    } else {
        return EXT_FAILURE;
    }

    ret = sample_aef_open_sound(&sound);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_aef_open_sound, ret);
        return ret;
    }

    ret = uapi_snd_set_port_aef_enable(sound, UAPI_SND_OUT_PORT_DAC0, UAPI_AEF_TYPE_PEQ, enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_port_aef_enable, ret);
        sample_aef_close_sound(sound);
        return ret;
    }

    if (enable == TD_TRUE) {
#ifdef SAP_EFFECT_SOFT_EQ_TYPE_SWITCH_SUPPORT
        (td_void)sample_aef_peq_rw_param(sound, aef_filter);
#else
        (td_void)sample_aef_peq_rw_param(sound);
#endif
    }

    sample_aef_close_sound(sound);
    return EXT_SUCCESS;
}
#endif

#ifdef SAP_EFFECT_DRC_SUPPORT
typedef struct {
    td_char *sample;
    td_char *aef_type;
    td_char *enable;
    td_char *drc_type;
} sample_aef_drc_arg;

typedef enum {
    SAMPLE_AEF_DRC_TYPE_RAW = 0,
    SAMPLE_AEF_DRC_TYPE_VOIP = 1,
    SAMPLE_AEF_DRC_TYPE_MUSIC = 2,
    SAMPLE_AEF_DRC_TYPE_MAX,
} sample_aef_drc_type;

static const uapi_snd_drc_attr g_drc_attr_voip = {
    .drc_target_gain = 40,
    .drc_max_lvl = -40,
    .drc_silence_threshold = -240,
    .transit_band_width = 80,
    .silence_gain = -80,
};

static const uapi_snd_drc_attr g_drc_attr_music = {
    .drc_target_gain = 60,
    .drc_max_lvl = -20,
    .drc_silence_threshold = -200,
    .transit_band_width = 60,
    .silence_gain = -60,
};

static td_s32 sample_aef_drc_rw_param(td_handle sound, td_u32 drc_type)
{
    td_s32 ret;
    uapi_snd_drc_attr drc_attr;

    ret = uapi_snd_get_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        UAPI_DRC_RW_PARAM_CMD, &drc_attr, sizeof(drc_attr));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_get_aef_param, ret);
        return ret;
    }

    if (drc_type == SAMPLE_AEF_DRC_TYPE_VOIP) {
        (td_void)memcpy_s(&drc_attr, sizeof(uapi_snd_peq_attr), &g_drc_attr_voip, sizeof(uapi_snd_peq_attr));
    } else if (drc_type == SAMPLE_AEF_DRC_TYPE_MUSIC) {
        (td_void)memcpy_s(&drc_attr, sizeof(uapi_snd_peq_attr), &g_drc_attr_music, sizeof(uapi_snd_peq_attr));
    }

    ret = uapi_snd_set_aef_param(sound, UAPI_SND_OUT_PORT_DAC0,
        UAPI_DRC_RW_PARAM_CMD, &drc_attr, sizeof(drc_attr));
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_aef_param, ret);
        return ret;
    }

    return ret;
}

static td_s32 sample_aef_drc_entry(td_char *argv[])
{
    td_s32 ret;
    td_handle sound;
    td_bool enable;
    td_u32 drc_type;

    sample_aef_drc_arg *arg = (sample_aef_drc_arg *)argv;

    if (strcmp(arg->enable, "on") == 0) {
        enable = TD_TRUE;
        drc_type = atoi(arg->drc_type);
        if (drc_type >= SAMPLE_AEF_DRC_TYPE_MAX) {
            return EXT_FAILURE;
        }
    } else if (strcmp(arg->enable, "off") == 0) {
        enable = TD_FALSE;
    } else {
        return EXT_FAILURE;
    }

    ret = sample_aef_open_sound(&sound);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_aef_open_sound, ret);
        return ret;
    }

    ret = uapi_snd_set_port_aef_enable(sound, UAPI_SND_OUT_PORT_DAC0, UAPI_AEF_TYPE_DRC, enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_port_aef_enable, ret);
        sample_aef_close_sound(sound);
        return ret;
    }

    if (enable == TD_TRUE) {
        sample_aef_drc_rw_param(sound, drc_type);
    }

    sample_aef_close_sound(sound);
    return EXT_SUCCESS;
}
#endif

#ifdef SAP_EFFECT_CUSTOMER_SUPPORT
static td_s32 sample_customer_aef_entry(td_char *argv[])
{
    td_s32 ret;
    td_handle sound;
    td_bool enable;

    sample_aef_arg *arg = (sample_aef_arg *)argv;

    if (strcmp(arg->enable, "on") == 0) {
        enable = TD_TRUE;
    } else if (strcmp(arg->enable, "off") == 0) {
        enable = TD_FALSE;
    } else {
        return EXT_FAILURE;
    }

    ret = sample_aef_open_sound(&sound);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(sample_aef_open_sound, ret);
        return ret;
    }

    ret = uapi_snd_set_port_aef_enable(sound, UAPI_SND_OUT_PORT_DAC0, UAPI_AEF_TYPE_CUSTOMER, enable);
    if (ret != EXT_SUCCESS) {
        sap_err_log_fun(uapi_snd_set_port_aef_enable, ret);
        sample_aef_close_sound(sound);
        return ret;
    }

    sample_aef_close_sound(sound);
    return EXT_SUCCESS;
}
#endif

td_s32 sample_aef(td_s32 argc, td_char *argv[])
{
    sample_aef_arg *arg = (sample_aef_arg *)argv;
    if (argc < (td_s32)(sizeof(*arg) / sizeof(td_char *))) {
        return EXT_FAILURE;
    }

    if (strcmp(arg->aef_type, "wdrc") == 0) {
        return sample_aef_wdrc_entry(arg);
    } else if (strcmp(arg->aef_type, "peq") == 0) {
#ifdef SAP_EFFECT_HW_EQ_V1_SUPPORT
        return sample_aef_peq_entry(argc, argv);
#else
        return sample_aef_peq_entry(arg);
#endif
    } else if (strcmp(arg->aef_type, "drc") == 0) {
#ifdef SAP_EFFECT_DRC_SUPPORT
        return sample_aef_drc_entry(argv);
#else
        return EXT_FAILURE;
#endif
    } else if (strcmp(arg->aef_type, "cust") == 0) {
#ifdef SAP_EFFECT_CUSTOMER_SUPPORT
        return sample_customer_aef_entry(argv);
#else
        return EXT_FAILURE;
#endif
    } else {
        return EXT_FAILURE;
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
