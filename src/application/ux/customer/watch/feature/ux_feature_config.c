/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ux feature config file
 * Author: AuthorNameMagicTag
 * Create: 2023-06-13
 */
#include <stdlib.h>
#include "key_id.h"
#include "ux_config.h"
#include "ux_event_define.h"
#include "ux_feature_config.h"

/* 获取mic配置 */
ux_feature_mic_type ux_get_mic_config(void)
{
    return UX_FEATURE_MIC_TYPE_0;
}

/* 获取助听所有场景下的参数配置大小 */
uint32_t ux_get_haid_alg_nv_cfg_size(void)
{
    return HAID_ALG_PARAM_SIZE;
}

/* 获取最小的助听音量配置 */
uint16_t ux_get_haid_vol_min_range(void)
{
    return HAID_VOL_MIN_RANGE;
}

/* 获取设备配对时间 */
uint32_t ux_get_device_con_timeout(void)
{
    return DEVICE_CON_TIMEOUT;
}

/* 获取验配DAC电压 */
uint8_t ux_get_haid_dac_volt(void)
{
    return HAID_DAC_VOLT_DEFAULT;
}

static const ux_recover_policy_t g_recover_policys[] = {
    {
        /* APP 协议侧第一条，对应APP侧触发恢厂 */
        .recover_type = APP_RECOVER_TYPE1,
        .region_info.region_mode[KEY_ID_REGION0] = false,
        .region_info.region_mode[KEY_ID_REGION1] = true,
        .region_info.region_mode[KEY_ID_REGION2] = true,
        .region_info.region_mode[KEY_ID_REGION3] = true,
        .region_info.region_mode[KEY_ID_REGION4] = true,
        .region_info.region_mode[KEY_ID_REGION5] = true,
        .region_info.region_mode[KEY_ID_REGION6] = true,
        .region_info.region_mode[KEY_ID_REGION7] = true,
        .region_info.region_mode[KEY_ID_REGION8] = true,
        .region_info.region_mode[KEY_ID_REGION9] = true,
        .region_info.region_mode[KEY_ID_REGION10] = true,
        .region_info.region_mode[KEY_ID_REGION11] = true,
        .region_info.region_mode[KEY_ID_REGION12] = true,
        .region_info.region_mode[KEY_ID_REGION13] = true,
        .region_info.region_mode[KEY_ID_REGION14] = true,
        .region_info.region_mode[KEY_ID_REGION15] = true,
        .end_action = RECOVER_END_RESET_ACTION,
    },
    {
        /* 按键触发恢厂, 对耳地址清除 */
        .recover_type = BUTTON_RECOVER_TYPE1,
        .region_info.region_mode[KEY_ID_REGION0] = false,
        .region_info.region_mode[KEY_ID_REGION1] = true,
        .region_info.region_mode[KEY_ID_REGION2] = true,
        .region_info.region_mode[KEY_ID_REGION3] = true,
        .region_info.region_mode[KEY_ID_REGION4] = true,
        .region_info.region_mode[KEY_ID_REGION5] = true,
        .region_info.region_mode[KEY_ID_REGION6] = true,
        .region_info.region_mode[KEY_ID_REGION7] = true,
        .region_info.region_mode[KEY_ID_REGION8] = true,
        .region_info.region_mode[KEY_ID_REGION9] = true,
        .region_info.region_mode[KEY_ID_REGION10] = true,
        .region_info.region_mode[KEY_ID_REGION11] = true,
        .region_info.region_mode[KEY_ID_REGION12] = true,
        .region_info.region_mode[KEY_ID_REGION13] = true,
        .region_info.region_mode[KEY_ID_REGION14] = true,
        .region_info.region_mode[KEY_ID_REGION15] = true,
        .end_action = RECOVER_END_ACC_OFF_ACTION,
    },
};

ux_recover_policy_t *get_recover_policy(uint8_t mode)
{
    for (int16_t i = 0; i < (int16_t) arry_num(g_recover_policys); i++) {
        if (g_recover_policys[i].recover_type == mode) {
            return (ux_recover_policy_t *)(&g_recover_policys[i]);
        }
    }

    return NULL;
}

bool is_sound_mute_valid(void)
{
    return SUPPORT_SOUND_MUTE;
}

const char* ux_get_default_bt_name(void)
{
    return DEFAULT_BT_NAME;
}

const char* ux_get_default_ble_name(void)
{
    return DEFAULT_BLE_NAME;
}

uint16_t ux_get_language_nv_id(void)
{
    return (uint16_t)NV_ID_LANGUAGE;
}

static ux_haid_scene_mode_t g_haid_scene_modes[SCENE_MODE_MAX] = {
    { SCENE_MODE_INDEX_0, true, AUTO_MODE_TYPE },
    { SCENE_MODE_INDEX_1, true, MANUAL_MODE_TYPE },
    { SCENE_MODE_INDEX_2, true, MANUAL_MODE_TYPE },
    { SCENE_MODE_INDEX_3, true, MANUAL_MODE_TYPE },
    { SCENE_MODE_INDEX_4, true, MANUAL_MODE_TYPE },
    { SCENE_MODE_INDEX_5, true, CUSTOM_MODE_TYPE },
    { SCENE_MODE_INDEX_6, true, CUSTOM_MODE_TYPE },
};

ux_haid_scene_mode_t *get_haid_scene_mode_info(void)
{
    return g_haid_scene_modes;
}

bool is_support_scene_auto_mode(void)
{
    return false;
}

bool is_support_scene_manual_mode(void)
{
    return true;
}

bool is_support_scene_custom_mode(void)
{
    return true;
}

bool is_support_power_on_with_outbox(void)
{
    return SUPPORT_POWER_ON_WITH_OUTBOX;
}

bool is_support_power_off_with_inbox(void)
{
    return SUPPORT_POWER_OFF_WITH_INBOX;
}

bool is_support_power_on_pairing(void)
{
    return SUPPORT_POWER_ON_PARING;
}

bool is_support_bt_standby(void)
{
    return SUPPORT_PAIRING_STANDBY_FEATURE;
}

bool is_support_pair_timeout(void)
{
    return SUPPORT_PAIRING_TIMEOUT;
}

bool is_support_peer_discover(void)
{
    return SUPPORT_PEER_DISCOVER_FEATURE;
}

bool is_support_reconn_fail_enter_pairing(void)
{
    return SUPPORT_RECONN_FAIL_ENTER_PAIRING;
}

bool is_support_notifica_uninterrupted(void)
{
    return SUPPORT_NOTIFICA_UNINTERRUPTED;
}

bool is_support_inear_tone_valid(void)
{
    return SUPPORT_INEAR_TONE_VALID;
}

bool is_support_peer_conn_timeout_reset(void)
{
    return SUPPORT_PEER_CONN_TIMEOUT_RESET;
}

bool is_support_voice_assistant(void)
{
    return SUPPORT_VOICE_ASSISTANT;
}

bool is_support_role_master_play_tone(void)
{
    return SUPPORT_ROLE_MASTER_PLAY_TONE;
}

bool is_out_box_tigger_acc_off_valid(void)
{
    return SUPPORT_OUT_BOX_TIGGER_ACC_OFF;
}

bool is_support_audio_peq_effect(void)
{
    return SUPPORT_AUDIO_PEQ_EFFECT;
}

bool is_support_mix_button(void)
{
    return SUPPORT_MIX_BUTTON;
}

bool is_support_mix_haid_vol_adjust(void)
{
    return SUPPORT_MIX_HAID_VOL_ADJUST;
}

bool is_support_sound_mute_haid_ctrl(void)
{
    return SUPPORT_SOUND_MUTE_HAID_CTRL;
}

bool is_support_ux_acc_off_shutdown(void)
{
    return SUPPORT_UX_ACC_OFF_SHUTDOWN;
}
