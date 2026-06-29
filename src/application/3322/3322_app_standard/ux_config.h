/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: feature config header file
 * Author: AuthorNameMagicTag
 * Create: 2023-04-11
 */

#ifndef UX_CONFIG_H
#define UX_CONFIG_H
#include <stdbool.h>
#include "ux_event_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef YES
#define YES     true
#endif

#ifndef NO
#define NO      false
#endif

/****************************特性开关*********************************************/
/* 是否支持LED */
#if (CONFIG_SUPPORT_LED_FEATURE == YES)
#define SUPPORT_LED_FEATURE     YES
#else
#define SUPPORT_LED_FEATURE     NO
#endif

/* 是否支持出盒开机 */
#if (CONFIG_SUPPORT_POWER_ON_WITH_OUTBOX == YES)
#define SUPPORT_POWER_ON_WITH_OUTBOX     YES
#else
#define SUPPORT_POWER_ON_WITH_OUTBOX     NO
#endif

/* 是否POWER_OFF事件下电 */
#if (CONFIG_SUPPORT_UX_ACC_OFF_SHUTDOWN == YES)
#define SUPPORT_UX_ACC_OFF_SHUTDOWN    YES
#else
#define SUPPORT_UX_ACC_OFF_SHUTDOWN    NO
#endif

/* 是否支持入盒关机 */
#if (CONFIG_SUPPORT_POWER_OFF_WITH_INBOX == YES)
#define SUPPORT_POWER_OFF_WITH_INBOX     YES
#else
#define SUPPORT_POWER_OFF_WITH_INBOX     NO
#endif

/* 是否支持提示音和灯语不被打断,当前主要针对开关机 */
#if (CONFIG_SUPPORT_NOTIFICA_UNINTERRUPTED == YES)
#define SUPPORT_NOTIFICA_UNINTERRUPTED     YES
#else
#define SUPPORT_NOTIFICA_UNINTERRUPTED     NO
#endif

/* 是否支持入耳播提示音 */
#if (CONFIG_SUPPORT_INEAR_TONE_VALID == YES)
#define SUPPORT_INEAR_TONE_VALID     YES
#else
#define SUPPORT_INEAR_TONE_VALID     NO
#endif

/* 是否支持入盒音频静音 */
#if (CONFIG_SUPPORT_SOUND_MUTE == YES)
#define SUPPORT_SOUND_MUTE     YES
#else
#define SUPPORT_SOUND_MUTE     NO
#endif

/* 是否支持语音助手 */
#if (CONFIG_SUPPORT_VOICE_ASSISTANT == YES)
#define SUPPORT_VOICE_ASSISTANT     YES
#else
#define SUPPORT_VOICE_ASSISTANT     NO
#endif

/* 是否支持耳机出盒下电 */
#if (CONFIG_SUPPORT_OUT_BOX_TIGGER_ACC_OFF == YES)
#define SUPPORT_OUT_BOX_TIGGER_ACC_OFF     YES
#else
#define SUPPORT_OUT_BOX_TIGGER_ACC_OFF     NO
#endif

/* 是否支持混合按键 */
#if (CONFIG_SUPPORT_MIX_BUTTON == YES)
#define SUPPORT_MIX_BUTTON     YES
#else
#define SUPPORT_MIX_BUTTON     NO
#endif

/* 是否支持PEQ音效 */
#if (CONFIG_SUPPORT_AUDIO_PEQ_EFFECT == YES)
#define SUPPORT_AUDIO_PEQ_EFFECT     YES
#else
#define SUPPORT_AUDIO_PEQ_EFFECT     NO
#endif

/* 是否支持混音模式助听音量调节 */
#if (CONFIG_SUPPORT_MIX_HAID_VOL_ADJUST == YES)
#define SUPPORT_MIX_HAID_VOL_ADJUST     YES
#else
#define SUPPORT_MIX_HAID_VOL_ADJUST     NO
#endif

/* 是否支持音乐静音时助听控制 */
#if (CONFIG_SUPPORT_SOUND_MUTE_HAID_CTRL == YES)
#define SUPPORT_SOUND_MUTE_HAID_CTRL    YES
#else
#define SUPPORT_SOUND_MUTE_HAID_CTRL    NO
#endif

/* 是否支持主耳播提示音 */
#if (CONFIG_SUPPORT_ROLE_MASTER_PLAY_TONE == YES)
#define SUPPORT_ROLE_MASTER_PLAY_TONE     YES
#else
#define SUPPORT_ROLE_MASTER_PLAY_TONE     NO
#endif

/* 是否支持开机触发设备配对 */
#if (CONFIG_SUPPORT_POWER_ON_PARING == YES)
#define SUPPORT_POWER_ON_PARING     YES
#else
#define SUPPORT_POWER_ON_PARING     NO
#endif

/* 是否支持standby配对超时蓝牙下电 */
#if (CONFIG_SUPPORT_PAIRING_STANDBY_FEATURE == YES)
#define SUPPORT_PAIRING_STANDBY_FEATURE     YES
#else
#define SUPPORT_PAIRING_STANDBY_FEATURE     NO
#endif

/* 是否支持配对超时 */
#if (CONFIG_SUPPORT_PAIRING_TIMEOUT == YES)
#define SUPPORT_PAIRING_TIMEOUT     YES
#else
#define SUPPORT_PAIRING_TIMEOUT     NO
#endif

/* 是否支持对耳扫描，即对耳配对特性 */
#if (CONFIG_SUPPORT_PEER_DISCOVER_FEATURE == YES)
#define SUPPORT_PEER_DISCOVER_FEATURE     YES
#else
#define SUPPORT_PEER_DISCOVER_FEATURE     NO
#endif

/* 是否支持回连失败超时进配对 */
#if (CONFIG_SUPPORT_RECONN_FAIL_ENTER_PAIRING == YES)
#define SUPPORT_RECONN_FAIL_ENTER_PAIRING     YES
#else
#define SUPPORT_RECONN_FAIL_ENTER_PAIRING     NO
#endif

/* 是否支持对耳连接失败触发重启功能 */
#if (CONFIG_SUPPORT_PEER_CONN_TIMEOUT_RESET == YES)
#define SUPPORT_PEER_CONN_TIMEOUT_RESET     YES
#else
#define SUPPORT_PEER_CONN_TIMEOUT_RESET     NO
#endif

/* 是否支持上电启动UX */
#if (CONFIG_SUPPORT_START_UX_WITH_PWR_ON == YES)
#define SUPPORT_START_UX_WITH_PWR_ON     YES
#else
#define SUPPORT_START_UX_WITH_PWR_ON     NO
#endif

/****************************宏值定义*********************************************/
/****************************定时器宏值定义****************************************/

/* 关机延时定时器 */
#ifdef CONFIG_POWER_OFF_DELAY_TIMEOUT
#define POWER_OFF_DELAY_TIMEOUT                 CONFIG_POWER_OFF_DELAY_TIMEOUT
#else
#define POWER_OFF_DELAY_TIMEOUT                 120000
#endif

/* 开机异常保护定时器 */
#ifdef CONFIG_POWER_ON_EXC_PROTECT_TIMEOUT
#define POWER_ON_EXC_PROTECT_TIMEOUT            CONFIG_POWER_ON_EXC_PROTECT_TIMEOUT
#else
#define POWER_ON_EXC_PROTECT_TIMEOUT            3000
#endif

/* 关机异常保护定时器 */
#ifdef CONFIG_POWER_OFF_EXC_PROTECT_TIMEOUT
#define POWER_OFF_EXC_PROTECT_TIMEOUT           CONFIG_POWER_OFF_EXC_PROTECT_TIMEOUT
#else
#define POWER_OFF_EXC_PROTECT_TIMEOUT           7000
#endif

/* 下电延时定时器 */
#ifdef CONFIG_ACC_OFF_DELAY_TIMEOUT
#define ACC_OFF_DELAY_TIMEOUT                   CONFIG_ACC_OFF_DELAY_TIMEOUT
#else
#define ACC_OFF_DELAY_TIMEOUT                   3000
#endif

/* 设备配对持续时间，范围为30000 ~ 1800000，单位ms */
#ifdef CONFIG_DEVICE_CON_TIMEOUT
#define DEVICE_CON_TIMEOUT                      CONFIG_DEVICE_CON_TIMEOUT
#else
#define DEVICE_CON_TIMEOUT                      180000
#endif

/* 设备回连持续时，范围为30000 ~ 120000，单位ms */
#ifdef CONFIG_DEVICE_RECON_TIMEOUT
#define DEVICE_RECON_TIMEOUT                    CONFIG_DEVICE_RECON_TIMEOUT
#else
#define DEVICE_RECON_TIMEOUT                    90000
#endif

/* 设备超距回连持续时，范围为30000 ~ 120000，单位ms */
#ifdef CONFIG_DEVICE_OVERRANGE_RECON_TIMEOUT
#define DEVICE_OVERRANGE_RECON_TIMEOUT          CONFIG_DEVICE_OVERRANGE_RECON_TIMEOUT
#else
#define DEVICE_OVERRANGE_RECON_TIMEOUT          90000
#endif

/* 恢复出厂延时定时器 */
#ifdef CONFIG_RECOVER_DELAY_TIMEOUT
#define RECOVER_DELAY_TIMEOUT                   CONFIG_RECOVER_DELAY_TIMEOUT
#else
#define RECOVER_DELAY_TIMEOUT                   5000
#endif

/* OTA升级成功重启延时定时器 */
#ifdef CONFIG_OTA_RESET_DELAY_TIMEOUT
#define OTA_RESET_DELAY_TIMEOUT                 CONFIG_OTA_RESET_DELAY_TIMEOUT
#else
#define OTA_RESET_DELAY_TIMEOUT                 1000
#endif

/* 对耳配对超时定时器 */
#ifdef CONFIG_PEER_CON_TIMEOUT
#define PEER_CON_TIMEOUT                        CONFIG_PEER_CON_TIMEOUT
#else
#define PEER_CON_TIMEOUT                        10000
#endif

/* 对耳回连超时定时器 */
#ifdef CONFIG_PEER_RECON_TIMEOUT
#define PEER_RECON_TIMEOUT                      CONFIG_PEER_RECON_TIMEOUT
#else
#define PEER_RECON_TIMEOUT                      10000
#endif

/* 对耳超距回连超时定时器 */
#ifdef CONFIG_PEER_OVERRANGE_RECON_TIMEOUT
#define PEER_OVERRANGE_RECON_TIMEOUT            CONFIG_PEER_OVERRANGE_RECON_TIMEOUT
#else
#define PEER_OVERRANGE_RECON_TIMEOUT            60000
#endif

/* 对耳扫描超时定时器 */
#ifdef CONFIG_PEER_CON_TRIGGER_TIMEOUT
#define PEER_CON_TRIGGER_TIMEOUT                CONFIG_PEER_CON_TRIGGER_TIMEOUT
#else
#define PEER_CON_TRIGGER_TIMEOUT                5000
#endif

/* 设配配对超时两次，蓝牙下电定时器 */
#ifdef CONFIG_PAIRING_STANDBY_TIMEOUT
#define PAIRING_STANDBY_TIMEOUT                 CONFIG_PAIRING_STANDBY_TIMEOUT
#else
#define PAIRING_STANDBY_TIMEOUT                 20000
#endif

/* 延时触发入耳定时器 */
#ifdef CONFIG_WEAR_IN_START_DELAY_TIMEOUT
#define WEAR_IN_START_DELAY_TIMEOUT             CONFIG_WEAR_IN_START_DELAY_TIMEOUT
#else
#define WEAR_IN_START_DELAY_TIMEOUT             1000
#endif

/* 关机状态下，满电延时下电定时器 */
#ifdef CONFIG_CHARGE_FULL_ACC_OFF_DELAY_TIMEOUT
#define CHARGE_FULL_ACC_OFF_DELAY_TIMEOUT       CONFIG_CHARGE_FULL_ACC_OFF_DELAY_TIMEOUT
#else
#define CHARGE_FULL_ACC_OFF_DELAY_TIMEOUT       10000
#endif

/* 出盒电源模块上报充电状态变化保护定时器 */
#ifdef CONFIG_OUT_BOX_TIGGER_ACC_OFF_TIMEOUT
#define OUT_BOX_TIGGER_ACC_OFF_TIMEOUT          CONFIG_OUT_BOX_TIGGER_ACC_OFF_TIMEOUT
#else
#define OUT_BOX_TIGGER_ACC_OFF_TIMEOUT          30000
#endif

/* 通知BT关机延时定时器 */
#ifdef CONFIG_NOTIFY_BT_OFF_DELAY_TIMEOUT
#define NOTIFY_BT_OFF_DELAY_TIMEOUT                 CONFIG_NOTIFY_BT_OFF_DELAY_TIMEOUT
#else
#define NOTIFY_BT_OFF_DELAY_TIMEOUT                 1000
#endif

/****************************整机宏值定义****************************************/
#ifdef CONFIG_BATTERY_SOC_HEARTBEAT_TIME
#define BATTERY_SOC_HEARTBEAT_TIME       CONFIG_BATTERY_SOC_HEARTBEAT_TIME
#else
#define BATTERY_SOC_HEARTBEAT_TIME       10000
#endif

#ifdef CONFIG_UX_POWER_NOTIFY_BATTERY_SOC_TIME
#define UX_POWER_NOTIFY_BATTERY_SOC_TIME       CONFIG_UX_POWER_NOTIFY_BATTERY_SOC_TIME
#else
#define UX_POWER_NOTIFY_BATTERY_SOC_TIME       10000
#endif

#ifdef CONFIG_UX_POWER_AUTO_POWER_OFF_DETECT_TIME
#define UX_POWER_AUTO_POWER_OFF_DETECT_TIME       CONFIG_UX_POWER_AUTO_POWER_OFF_DETECT_TIME
#else
#define UX_POWER_AUTO_POWER_OFF_DETECT_TIME       1
#endif

#ifdef CONFIG_UX_POWER_AUTO_POWER_OFF_TIME
#define UX_POWER_AUTO_POWER_OFF_TIME       CONFIG_UX_POWER_AUTO_POWER_OFF_TIME
#else
#define UX_POWER_AUTO_POWER_OFF_TIME       30
#endif

#ifdef CONFIG_LOW_BATTERY_WARNING_THRESHOLD
#define LOW_BATTERY_WARNING_THRESHOLD       CONFIG_LOW_BATTERY_WARNING_THRESHOLD
#else
#define LOW_BATTERY_WARNING_THRESHOLD       15
#endif

#ifdef CONFIG_LOW_BATTERY_POWER_OFF_THRESHOLD
#define LOW_BATTERY_POWER_OFF_THRESHOLD       CONFIG_LOW_BATTERY_POWER_OFF_THRESHOLD
#else
#define LOW_BATTERY_POWER_OFF_THRESHOLD       5
#endif

#ifdef CONFIG_LOW_BATTERY_POWER_OFF_TIMEOUT
#define LOW_BATTERY_POWER_OFF_TIMEOUT       CONFIG_LOW_BATTERY_POWER_OFF_TIMEOUT
#else
#define LOW_BATTERY_POWER_OFF_TIMEOUT       30000
#endif

#ifdef CONFIG_OTA_BATTERY_THRESHOLD
#define OTA_BATTERY_THRESHOLD       CONFIG_OTA_BATTERY_THRESHOLD
#else
#define OTA_BATTERY_THRESHOLD       30
#endif
/****************************BT宏值定义****************************************/
/* 无业务双耳间SNIFF的最大保连间隔，单位slots即625μs */
#ifdef CONFIG_TYPE_PEER_NO_TRAFF_MAX_INTVL
#define TYPE_PEER_NO_TRAFF_MAX_INTVL            CONFIG_TYPE_PEER_NO_TRAFF_MAX_INTVL
#else
#define TYPE_PEER_NO_TRAFF_MAX_INTVL            192
#endif

/* 无业务双耳间SNIFF的最小保连间隔，单位slots即625μs */
#ifdef CONFIG_TYPE_PEER_NO_TRAFF_MIN_INTVL
#define TYPE_PEER_NO_TRAFF_MIN_INTVL            CONFIG_TYPE_PEER_NO_TRAFF_MIN_INTVL
#else
#define TYPE_PEER_NO_TRAFF_MIN_INTVL            192
#endif

/* 与手机间SNIFF的最大保连间隔，单位slots即625μs */
#ifdef CONFIG_TYPE_MOBILE_MAX_INTVL
#define TYPE_MOBILE_MAX_INTVL                   CONFIG_TYPE_MOBILE_MAX_INTVL
#else
#define TYPE_MOBILE_MAX_INTVL                   800
#endif

/* 与手机间SNIFF的最小保连间隔，单位slots即625μs */
#ifdef CONFIG_TYPE_MOBILE_MIN_INTVL
#define TYPE_MOBILE_MIN_INTVL                   CONFIG_TYPE_MOBILE_MIN_INTVL
#else
#define TYPE_MOBILE_MIN_INTVL                   384
#endif

/* 音乐静音检测时长,单位s */
#ifdef CONFIG_SOUND_MUTE_DURATION
#define SOUND_MUTE_DURATION                     CONFIG_SOUND_MUTE_DURATION
#else
#define SOUND_MUTE_DURATION                     0
#endif

/* ble广播间隔，单位slots即625μs */
#ifdef CONFIG_BLE_INTERVAL_TIME
#define BLE_INTERVAL_TIME                       CONFIG_BLE_INTERVAL_TIME
#else
#define BLE_INTERVAL_TIME                       800
#endif

/* ble广播发射功率 */
#ifdef CONFIG_BLE_ADV_TX_PWR
#define BLE_ADV_TX_PWR                          CONFIG_BLE_ADV_TX_PWR
#else
#define BLE_ADV_TX_PWR                          0x08
#endif

/* ble名称最长20个字节 */
#ifdef CONFIG_BLE_DEV_NAME_MAX_LEN
#define BLE_DEV_NAME_MAX_LEN                    CONFIG_BLE_DEV_NAME_MAX_LEN
#else
#define BLE_DEV_NAME_MAX_LEN                    20
#endif

/* ble扩展最长19个字节 */
#ifdef CONFIG_BLE_EXPAND_VAL_MAX_LEN
#define BLE_EXPAND_VAL_MAX_LEN                  CONFIG_BLE_EXPAND_VAL_MAX_LEN
#else
#define BLE_EXPAND_VAL_MAX_LEN                  19
#endif

/* ble广播扩展类型 */
#ifdef CONFIG_BLE_ADV_EXPAND_TYPE_VAL
#define BLE_ADV_EXPAND_TYPE_VAL                 CONFIG_BLE_ADV_EXPAND_TYPE_VAL
#else
#define BLE_ADV_EXPAND_TYPE_VAL                 0x3d
#endif

/* 主副切信息 */
#if (CONFIG_BT_CONFIG_HAND_OVER_VALID == YES)
#define BT_CONFIG_HAND_OVER                     BT_CONFIG_HAND_OVER_VALID
#elif (CONFIG_BT_CONFIG_HAND_OVER_FIXED_LEFT == YES)
#define BT_CONFIG_HAND_OVER                     BT_CONFIG_HAND_OVER_FIXED_LEFT
#elif (CONFIG_BT_CONFIG_HAND_OVER_FIXED_RIGHT == YES)
#define BT_CONFIG_HAND_OVER                     BT_CONFIG_HAND_OVER_FIXED_RIGHT
#else
#define BT_CONFIG_HAND_OVER                     BT_CONFIG_HAND_OVER_VALID
#endif

/* 对耳连接失败触发重启最大次数限制 */
#ifdef CONFIG_MAX_PEER_CONN_RESET
#define MAX_PEER_CONN_RESET                     CONFIG_MAX_PEER_CONN_RESET
#else
#define MAX_PEER_CONN_RESET                     3
#endif

/* PAIRING_STANDBY特性下，设备配对连续失败最大次数 */
#ifdef CONFIG_DEVICE_CON_FAILED_MAX_NUMS
#define DEVICE_CON_FAILED_MAX_NUMS              CONFIG_DEVICE_CON_FAILED_MAX_NUMS
#else
#define DEVICE_CON_FAILED_MAX_NUMS              1
#endif

/* 正常情况下，设备回连超时后，设备尝试回连次数 */
#ifdef CONFIG_DEVICE_RECON_TIMES
#define DEVICE_RECON_TIMES                      CONFIG_DEVICE_RECON_TIMES
#else
#define DEVICE_RECON_TIMES                      1
#endif

/* 超距断连导致设备回连超时后，设备尝试回连连续次数 */
#ifdef CONFIG_DEVICE_OVERRANGE_RECON_TIMES
#define DEVICE_OVERRANGE_RECON_TIMES            CONFIG_DEVICE_OVERRANGE_RECON_TIMES
#else
#define DEVICE_OVERRANGE_RECON_TIMES            1
#endif

/* 默认BT名称 */
#ifdef CONFIG_DEFAULT_BT_NAME
#define DEFAULT_BT_NAME                         CONFIG_DEFAULT_BT_NAME
#else
#define DEFAULT_BT_NAME                         "melody"
#endif

/* 默认BLE名称 */
#ifdef CONFIG_DEFAULT_BLE_NAME
#define DEFAULT_BLE_NAME                        CONFIG_DEFAULT_BLE_NAME
#else
#define DEFAULT_BLE_NAME                        "melodyble"
#endif

/****************************HAID宏值定义****************************************/
/* 助听验配DAC的默认电压1.2v,取值范围[0x2,0xa],对应[1.2v,1.6v] */
#ifdef CONFIG_HAID_DAC_VOLT_DEFAULT
#define HAID_DAC_VOLT_DEFAULT                   CONFIG_HAID_DAC_VOLT_DEFAULT
#else
#define HAID_DAC_VOLT_DEFAULT                   0x2
#endif

/* 助听算法参数大小,需要与nv结构定义保持一致,最大不能超过 */
#ifdef CONFIG_HAID_ALG_PARAM_SIZE
#define HAID_ALG_PARAM_SIZE                     CONFIG_HAID_ALG_PARAM_SIZE
#else
#define HAID_ALG_PARAM_SIZE                     3584
#endif

/* 3.5mm耳机特性开关 */
#if defined(CONFIG_SUPPORT_JACK) && (CONFIG_SUPPORT_JACK == YES)
#define SUPPORT_JACK YES
#else
#define SUPPORT_JACK NO
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif