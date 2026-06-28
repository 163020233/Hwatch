/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ux feature config header file
 * Author: AuthorNameMagicTag
 * Create: 2023-04-11
 */

#ifndef UX_FEATURE_CONFIG_H
#define UX_FEATURE_CONFIG_H
#include "errcode.h"
#include "nv.h"
#include "ux_config.h"

#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA) || defined(UT_TEST)
#include "ux_bt_common.h"
#include "key_struct.h"
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define POWER_ON_EVENT_TYPE                     UX_EVENT_TYPE_MASTER   /* AT^POWER命令模拟开机事件的事件类型 */
#define POWER_ON_EVENT_ID                       UX_MASTER_POWER_ON     /* AT^POWER命令模拟开机事件的事件名称 */
#define POWER_OFF_EVENT_TYPE                    UX_EVENT_TYPE_MASTER   /* AT^POWER命令模拟关机事件的事件类型 */
#define POWER_OFF_EVENT_ID                      UX_MASTER_POWER_OFF    /* AT^POWER命令模拟关机事件的事件名称 */

#define HAID_VOL_DEFAULT_RANGE                  8 /* 默认音量档位,产品可配置 */
#define HAID_VOL_MAX_RANGE                      10 /* 最大的音量档位,产品可配置 */
#define HAID_VOL_MIN_RANGE                      1 /* 助听音量最小档位为1档 */
#define HAID_DEFAULT_MIX_VOL_RANGE              8 /* 默认混音模式助听音量档位 */

typedef enum {
    UX_FEATURE_MIC_TYPE_0,
    UX_FEATURE_MIC_TYPE_1,

    UX_FEATURE_MIC_TYPE_MAX,
} ux_feature_mic_type;

typedef enum {
    RECOVER_END_RESET_ACTION = 0,
    RECOVER_END_ACC_OFF_ACTION,

    RECOVER_END_ACTION_BUTT
} recover_end_action_t;

typedef enum {
    APP_RECOVER_TYPE1 = 1,
    BUTTON_RECOVER_TYPE1,

    RECOVER_TYPE_BUTT
} recover_type_t;

typedef enum {
    AUTO_MODE_TYPE = 0,
    MANUAL_MODE_TYPE,
    CUSTOM_MODE_TYPE,

    MODE_TYPE_BUTT
} ux_haid_mode_t;

typedef enum {
    SCENE_MODE_INDEX_0 = 0,
    SCENE_MODE_INDEX_1,
    SCENE_MODE_INDEX_2,
    SCENE_MODE_INDEX_3,
    SCENE_MODE_INDEX_4,
    SCENE_MODE_INDEX_5,
    SCENE_MODE_INDEX_6,

    SCENE_MODE_MAX
} ux_scene_mode_index_t;

typedef struct {
    recover_type_t recover_type;
    nv_restore_mode_t region_info;
    uint8_t end_action;
} ux_recover_policy_t;

typedef struct {
    uint8_t index;  /* 助听场景模式下标 */
    bool    enable;    /* 是否使能 */
    uint8_t type;   /* 类型 */
} ux_haid_scene_mode_t;

/* 获取助听所有场景下的参数配置大小 */
uint32_t ux_get_haid_alg_nv_cfg_size(void);
/* 获取最小的助听音量配置 */
uint16_t ux_get_haid_vol_min_range(void);
/* 获取设备配对时间 */
uint32_t ux_get_device_con_timeout(void);
/* 获取验配DAC电压 */
uint8_t ux_get_haid_dac_volt(void);
/* 根据mode 获取恢复出厂策略参数 */
ux_recover_policy_t *get_recover_policy(uint8_t mode);
/* 获取语言nv id */
uint16_t ux_get_language_nv_id(void);
/* 获取助听参数 nv id */
uint16_t ux_get_haid_param_nv_id(void);
/* 获取助听硬件参数nv id */
uint16_t ux_get_haid_hw_param_nv_id(void);
/* 获取助听医规参数nv id */
uint16_t ux_get_haid_medical_param_nv_id(void);

/* 是否支持自动模式进入切换模式中 */
bool is_support_scene_auto_mode(void);
/* 是否支持手动模式进入切换模式中 */
bool is_support_scene_manual_mode(void);
/* 是否支持自定义模式进入切换模式中 */
bool is_support_scene_custom_mode(void);
/* 支持入盒静音功能 */
bool is_sound_mute_valid(void);
/* 获取默认bt名称 */
const char* ux_get_default_bt_name(void);
/* 获取默认ble名称 */
const char* ux_get_default_ble_name(void);
/* 获取助听场景信息 */
ux_haid_scene_mode_t *get_haid_scene_mode_info(void);
/* 支持GPIO_LED功能 */
bool is_support_gpio_led(void);
/* 支持PWM_LED功能 */
bool is_support_pwm_led(void);
/* 支持出盒开机 */
bool is_support_power_on_with_outbox(void);
/* 支持入盒关机 */
bool is_support_power_off_with_inbox(void);
/* 支持开机触发设备配对 */
bool is_support_power_on_pairing(void);
/* 支持standby配对超时蓝牙下电 */
bool is_support_bt_standby(void);
/* 是否支持配对超时 */
bool is_support_pair_timeout(void);
/* 支持对耳扫描，即对耳配对特性 */
bool is_support_peer_discover(void);
/* 支持回连失败超时进配对 */
bool is_support_reconn_fail_enter_pairing(void);
/* 支持提示音和灯语不被打断,当前主要针对开关机 */
bool is_support_notifica_uninterrupted(void);
/* 支持入耳播提示音 */
bool is_support_inear_tone_valid(void);
/* 支持对耳失败重启功能 */
bool is_support_peer_conn_timeout_reset(void);
/* 支持语音助手功能 */
bool is_support_voice_assistant(void);
/* 支持主耳播提示音 */
bool is_support_role_master_play_tone(void);
/* 支持音频PEQ音效 */
bool is_support_audio_peq_effect(void);
/* 支持出盒下电 */
bool is_out_box_tigger_acc_off_valid(void);
/* 支持混合按键 */
bool is_support_mix_button(void);
/* 支持混音模式下助听音量调节 */
bool is_support_mix_haid_vol_adjust(void);
/* 支持音频静音时助听控制 */
bool is_support_sound_mute_haid_ctrl(void);
/* 支持出盒开机 */
bool is_support_start_up_when_outbox(void);
/* 支持入盒关机 */
bool is_support_shut_down_when_inbox(void);
/* 获取麦克风配置 */
ux_feature_mic_type ux_get_mic_config(void);

/* 支持ACC_OFF下电 */
bool is_support_ux_acc_off_shutdown(void);
/* 支持BT待机 */
bool is_support_bt_standby(void);

/* 支持UX默认的智能佩戴检测策略 */
bool is_support_ux_wear_detection_policy(void);
bool is_support_start_ux_with_power_on(void);

/* 是否支持同时连接两个设备 */
bool is_support_dual_connection(void);

/* 是否支持双连接时第二路通话抢占出声 */
bool is_support_dual_call_preemption(void);

/* 是否支持音频低时延 */
bool is_support_low_latency(void);

/* 是否支持获取音乐歌曲信息 */
bool is_support_music_attribute(void);

/* 是否只支持发送TWS耳机主耳电量, 默认是发送当前耳机电量 */
bool is_support_send_master_battery(void);

/* 是否支持BLE主副切 */
bool is_support_ble_handover(void);
/* 是否支持sle 数传 */
bool is_support_sle_data_port(void);
/* 获取默认TWS对耳配对码 */
const char* ux_get_default_tws_pair_code(void);
/* 是否支持通过蓝牙名称扫描匹配对耳 */
bool is_support_tws_pair_by_name(void);
/* 是否支持热插拔唤醒usb */
bool is_support_usb_hotplug_aware(void);
/* 是否支持电源唤醒usb */
bool is_support_usb_power_aware(void);
/* 是否支持加载usb设备 */
bool is_support_usb_device_connect(void);
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA) || defined(UT_TEST)
/* 获取nv配置下的默认助听音量所有参数 */
haid_vol_t ux_get_haid_vol_nv_cfg(void);
/* 获取TWS耳机BT回连功率档位 */
uint8_t ux_get_bt_page_pwr_reduction(void);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
