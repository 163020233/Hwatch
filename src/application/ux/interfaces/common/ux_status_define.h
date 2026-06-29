/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Define ux status \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef UX_STATUS_DEFINE_H
#define UX_STATUS_DEFINE_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    CALL_IDLE = 0,                    /**< 空闲状态 */
    INCOMING_STATE,                   /**< 来电状态 */
    DIALING_STATE,                    /**< 拨号状态 */
    ANSWER_STATE,                     /**< 接听状态 */
    RINGING_STATE,                    /**< 响铃状态 */
    PLAY_MUSIC_STATE,                 /**< 播放音乐状态 */
    STOP_MUSIC_STATE = 6,             /**< 停止音乐状态 */
    PAUSE_MUSIC_STATE,                /**< 暂停音乐状态 */
    PLAY_MUSIC_MUTE_STATE,            /**< 静音播放音乐状态 */

    CALL_STATE_BUTT                   /**< 最大值 */
} call_state_t;

typedef enum {
    HAID_IDLE = 0,                    /**< 空闲状态 */
    HAID_DOING,                       /**< 进行中状态 */
    HAID_FITTING,                     /**< 验配状态 */
    HAID_MEDICAL_ENTER,               /**< 进入医疗模式状态 */
    HAID_MEDICAL_DOING,               /**< 医疗模式进行中状态 */
    HAID_MEDICAL_DONE,                /**< 医疗模式完成状态 */
    HAID_AFC_CALIBRATE,               /**< AFC校准状态 */

    HAID_STATE_BUTT                   /**< 最大值 */
} haid_state_t;

typedef enum {
    STATUS_NOT_CHARGING       = 0,     /**< 非充电状态 */
    STATUS_CHARGING           = 1,     /**< 充电中 */
    STATUS_CHARGE_DONE        = 2,     /**< 充电完成 */

    STATUS_CHARGE_BUTT,                /**< 最大值 */
} charge_state_t;

typedef enum {
    STATUS_SENSOR_WEAR_INVALID = 1,    /**< 无效状态 */
    STATUS_SENSOR_WEAR_IN,             /**< 入耳状态 */
    STATUS_SENSOR_WEAR_OUT,            /**< 出耳状态 */

    STATUS_SENSOR_WEAR_BUTT,           /**< 最大值 */
} sensor_wear_status_t;

typedef enum {
    AUDIO_HAID_MIX_STATE       = 1,     /**< 蓝牙音频（蓝牙通话和音乐）和助听模式混音 */
    AUDIO_HAID_DIVIDE_STATE,            /**< 蓝牙音频（蓝牙通话和音乐）和助听模式互斥 */
    CALL_HAID_MIX_STATE,                /**< 蓝牙通话和助听模式混音，音乐和助听模式互斥 */
    MUSIC_HAID_MIX_STATE,               /**< 蓝牙音乐和助听模式混音，蓝牙通话和助听模式互斥 */

    AUDIO_HAID_BUTT_STATE,              /**< 最大值 */
} audio_aid_mix_state_t;

typedef enum {
    STATUS_BOX_IN = 0,                  /**< 设备在盒内 */
    STATUS_BOX_OUT,                     /**< 设备在盒外 */

    STATUS_BOX_BUTT                     /**< 最大值 */
} position_state_t;

typedef enum {
    STATUS_PEER_EAR_DISCONNECT = 0,     /**< 对耳断连 */
    STATUS_PEER_EAR_CONNECT,            /**< 对耳连接 */

    STATUS_PEER_EAR_BUTT                /**< 最大值 */
} peer_ear_state_t;

typedef enum {
    STATUS_PEER_EAR_SPP_DISCONNECT = 0, /**< 对耳SPP断连 */
    STATUS_PEER_EAR_SPP_CONNECT,        /**< 对耳SPP连接 */

    STATUS_PEER_EAR_SPP_BUTT            /**< 最大值 */
} peer_ear_spp_state_t;

typedef enum {
    TOUCH_CONFIG_INVALID_STATE = 1,     /**< 耳机上下滑动效果失效 */
    TOUCH_CONFIG_ACTIVE_STATE,          /**< 耳机上下滑动效果生效，上滑加音量，下滑减音量 */

    TOUCH_CONFIG_BUTT,                  /**< 最大值 */
} touch_config_state_t;

typedef enum {
    STATUS_BT_STANDBY_DOING,            /**< 蓝牙待机进行中 */
    STATUS_BT_STANDBY_DONE,             /**< 蓝牙待机完成 */

    STATUS_BT_STANDBY_BUTT              /**< 最大值 */
} bt_standby_state_t;

typedef enum {
    STATUS_TWS_PAIR_INIT = 0,            /**< TWS配对初始化 */
    STATUS_TWS_PAIR_INVALID,             /**< TWS配对无效 */
    STATUS_TWS_PAIR_VALID,               /**< TWS配对有效 */

    STATUS_TWS_PAIR_MAX                  /**< 最大值 */
} tws_pair_state_t;

typedef enum {
    STATUS_DEVICE_PAIR_DISCONNECT = 0,              /**< 设备断连状态 */
    STATUS_DEVICE_PAIR_CONNECT,                     /**< 设备连接状态 */
    STATUS_DEVICE_PAIR_CONNECTTING,                 /**< 与设备配对中 */
    STATUS_DEVICE_RECON_CONNECTTING,                /**< 正常回连状态 */
    STATUS_DEVICE_RECON_OVERRANGE_CONNECTTING,      /**< 拉锯回连状态 */

    STATUS_DEVICE_PAIR_BUTT                         /**< 最大值 */
} device_pair_state_t;

typedef enum {
    STATUS_DEVICE_RECONN_ALL = 0,                   /**< 同时回连对耳、设备A 设备B(若支持) */
    STATUS_DEVICE_RECONN_TWS,                       /**< 只回连对耳 */
    STATUS_DEVICE_RECONN_END,                       /**< 最大值 */
} device_reconn_phase_t;

typedef enum {
    STATUS_DEVICE_PAIRING_INIT = 0,                 /**< 设备配对初始化 */
    STATUS_DEVICE_START_PAIR,                       /**< 耳机进入配对模式 */
    STATUS_DEVICE_STOP_PAIR,                        /**< 耳机停止配对模式 */
    STATUS_DEVICE_START_CONNECT,                    /**< 耳机进入回连模式 */
    STATUS_DEVICE_STOP_CONNECT,                     /**< 耳机停止回连模式 */
    STATUS_DEVICE_START_DISCOVERY,                  /**< 耳机处于广播模式(星闪/BLE) */
    STATUS_DEVICE_STOP_DISCOVERY,                   /**< 耳机停止广播模式(星闪/BLE) */
    STATUS_DEVICE_START_DISCONNECT,                 /**< 耳机进入断连模式 */

    STATUS_DEVICE_PAIRING_BUTT                      /**< 最大值 */
} device_pairing_state_t;

typedef enum {
    STATUS_SEND_POSITION = 0,                        /**< 耳机位置 */
    STATUS_SEND_WEAR_SIDE,                           /**< 耳机佩戴位置 */

    STATUS_SEND_POSITION_MAX                         /**< 最大值 */
} device_send_position_state_t;

typedef enum {
    STATUS_SEND_CONN_STATE_INIT = 0,         /**< 连接状态初始化 */
    STATUS_SEND_BT_PAIRING,                  /**< 耳机BT协议配对回连状态 */
    STATUS_SEND_SLE_PAIRING,                 /**< 耳机SLE协议配对回连状态 */
    STATUS_SEND_COMBO_PAIRING,               /**< 耳机COMBO协议配对回连状态 */
    STATUS_SEND_CONN_STATE_ALL,              /**< 最大值 */
} device_send_connect_state_t;

typedef enum {
    STATUS_SLAVE_DEVICE_PAIR_NO_TRIGGER = 0,  /**< 按键未触发 */
    STATUS_SLAVE_DEVICE_PAIR_TRIGGER,         /**< 按键触发强配 */

    STATUS_SLAVE_DEVICE_PAIR_BUTT
} slave_trigger_dev_pair_state_t;

typedef enum {
    STATUS_LOW_BATTERY_RESUME = 0,           /**< 低电量恢复 */
    STATUS_LOW_BATTERY_WARNING,              /**< 低电量警告 */
    STATUS_LOW_BATTERY_POWER_OFF,            /**< 低电量关机 */

    STATUS_LOW_BATTERY_BUTT                  /**< 最大值 */
} lowbattery_state_t;

typedef enum {
    STATUS_COVER_CLOSE = 0,                  /**< 合盖 */
    STATUS_COVER_OPEN,                       /**< 开盖 */

    STATUS_COVER_BUTT                        /**< 最大值 */
} cover_state_t;

typedef enum {
    STATUS_EARPHONE_SOUND_ACTIVE = 1,        /**< 耳机发声 */
    STATUS_PHONE_SOUND_ACTIVE,               /**< 手机发声 */

    STATUS_CALL_SOUND_BUTT                   /**< 最大值 */
} call_sound_state_t;

typedef enum {
    STATUS_OFF_WORKING = 0,          /**< 关机工作状态 */
    STATUS_ON_WORKING,               /**< 开机工作状态 */
    STATUS_OFF_WORKING_DELAY,        /**< 延迟关机工作状态 */
    STATUS_ON_WORKING_DELAY,         /**< 延迟开机工作状态 */
    STATUS_ON_DUT,                   /**< DUT模式 */
    STATUS_ON_OTA,                   /**< OTA模式 */
    STATUS_ON_RECOVER,               /**< 恢复模式 */
    STATUS_ON_RESET,                 /**< 复位模式 */
    STATUS_ACC_OFF,                  /**< ACC关闭 */

    STATUS_WORKING_BUTT              /**< 最大值 */
} working_state_t;

typedef enum {
    STATUS_OTA_CHECK_SUCCESS = 0,    /**< 升级前检查成功 */
    STATUS_OTA_FITTING_FAIL,         /**< 未配对 */
    STATUS_OTA_BATTERY_FAIL,         /**< 电量不足 */
    STATUS_OTA_EARPHONE_POS_FAIL,    /**< 耳机不在盒内 */
    STATUS_OTA_RECOVER_FAIL,         /**< 恢厂中 */
    STATUS_OTA_POWER_OFF,            /**< 关机 */
    STATUS_OTA_RESET,                /**< 复位 */
    STATUS_OTA_DEVICE_DISCONNECT,    /**< 设备断连 */
    STATUS_OTA_PEER_DISCONNECT,      /**< 对耳断连 */
    STATUS_OTA_BLE_DISCONNECT,       /**< BLE连接断连 */
    STATUS_OTA_USB_CONNECT,          /**< USB插入 */
    STATUS_OTA_USB_DISCONNECT,       /**< USB拔出 */
    STATUS_OTA_OUT_BOX,              /**< 出盒 */
    STATUS_OTA_IN_BOX,               /**< 入盒 */
    STATUS_OTA_SUCCESS,              /**< 标记OTA成功 */
    STATUS_OTA_FAIL,                 /**< 标记OTA失败 */
    STATUS_OTA_ROLE_MASTER,          /**< 耳机角色变为主耳 */
    STATUS_OTA_ROLE_SLAVE,           /**< 耳机角色变为副耳 */
    STATUS_OTA_EAR_CONNECT,          /**< 对耳连接 */
    STATUS_OTA_CALL_COMING,          /**< 来电 */

    STATUS_OTA_BUTT                  /**< 最大值 */
} ota_state_t;

typedef enum {
    STATUS_FTM_DISABLE = 0,          /**< FTM禁用 */
    STATUS_FTM_ENABLE,               /**< FTM使能 */
    STATUS_BTSIGNAL_ENABLE,          /**< 蓝牙信号使能 */

    STATUS_FTM_BUTT                  /**< 最大值 */
} ftm_state_t;

typedef enum {
    STATUS_STORAGE_POWER_INVALID = 0,        /**< 存储电源无效 */
    STATUS_STORAGE_POWER_ON,                 /**< 存储电源开启 */
    STATUS_STORAGE_POWER_OFF,                /**< 存储电源关闭 */

    STATUS_STORAGE_POWER_BUTT                /**< 最大值 */
} storage_power_state_t;

typedef enum {
    STATUS_BT_POWER_OFF_WITH_BOX = 0,        /**< 蓝牙随盒关机 */
    STATUS_BT_QUICK_POWER_OFF,               /**< 蓝牙快速关机 */
    STATUS_BT_POWER_OFF_WITHOUT_BOX,         /**< 蓝牙不随盒关机 */

    STATUS_BT_POWER_OFF_BUTT                 /**< 最大值 */
} bt_power_off_mode_t;

typedef enum {
    STATUS_BUTTON_CONFIG_INVALID = 1,        /**< 按键配置无效 */
    STATUS_BUTTON_CONFIG_VALID,              /**< 按键配置有效 */

    STATUS_BUTTON_CONFIG_BUTT                /**< 最大值 */
} button_config_mode_t;

typedef enum {
    POWER_OFF_WITH_BUTTON = 1,          /**< 按键关机 */
    POWER_OFF_WITH_INBOX,               /**< 入盒关机 */
    POWER_OFF_WITH_COVER,               /**< 关盖关机 */

    POWER_OFF_MODE_BUTT                 /**< 最大值 */
} power_off_mode_t;

typedef enum {
    POWER_ON_WITH_BUTTON = 1,           /**< 按键开机 */
    POWER_ON_WITH_OUTBOX,               /**< 出盒开机 */
    POWER_ON_WITH_COVER,                /**< 开盒开机 */

    POWER_ON_MODE_BUTT                  /**< 最大值 */
} power_on_mode_t;

typedef enum {
    POWER_ON_NOTIFICA_NOT_FINISH,       /**< 开机通知未完成 */
    POWER_ON_NOTIFICA_FINISH,           /**< 开机通知完成 */
    POWER_OFF_NOTIFICA_NOT_FINISH,      /**< 关机通知未完成 */
    POWER_OFF_NOTIFICA_FINISH,          /**< 关机通知完成 */

    NOTIFICA_WORK_STATE_BUTT            /**< 最大值 */
} notifica_work_state_t;

typedef enum {
    STATUS_TONE_ENABLE_INVALID,         /**< 提示音使能无效 */
    STATUS_TONE_DISABLE,                /**< 提示音禁用 */
    STATUS_TONE_ENABLE,                 /**< 提示音使能 */

    STATUS_TONE_ENABLE_BUTT             /**< 最大值 */
} tone_enable_state_t;

typedef enum {
    STATUS_MUTE_INVALID,                /**< 静音无效 */
    STATUS_MUTE_ENABLE,                 /**< 静音使能 */

    STATUS_MUTE_BUTT                    /**< 最大值 */
} mute_state_t;

typedef enum {
    STATUS_HAID_ENABLED = 1,            /**< 助听使能 */
    STATUS_HAID_DISABLED,               /**< 助听禁用 */

    STATUS_HAID_ENABLE_BUTT             /**< 最大值 */
} haid_enable_state_t;

typedef enum {
    STATUS_AFC_CALIBRATE_INVALID,       /**< AFC校准无效 */
    STATUS_AFC_CALIBRATE_NOT_START = 1, /**< AFC校准未开始 */
    STATUS_AFC_CALIBRATE_DOING = 2,     /**< AFC校准进行中 */
    STATUS_AFC_CALIBRATE_FINISH = 3,    /**< AFC校准完成 */
    STATUS_AFC_CALIBRATE_FAIL = 4,      /**< AFC校准失败 */

    STATUS_AFC_CALIBRATE_BUTT           /**< 最大值 */
} haid_afc_calibrate_state_t;

typedef enum {
    STATUS_RECORDER_STOP,               /**< 录音停止 */
    STATUS_RECORDER_START,              /**< 录音开始 */

    STATUS_RECORDER_BUTT                /**< 最大值 */
} recorder_state_t;

typedef enum {
    CONN_MODE_ONE   = 0x1,              /**< 连接模式1 */
    CONN_MODE_TWO   = 0x2,              /**< 连接模式2 */
    CONN_MODE_THREE = 0x3,              /**< 连接模式3 */
    CONN_MODE_FOUR  = 0x4,              /**< 连接模式4 */

    CONN_MODE_BUTT                      /**< 最大值 */
} conn_mode_t;

typedef enum {
    AUTORATE_MODE_OFF = 0,              /**< 自动播放速率关闭 */
    AUTORATE_MODE_ON  = 1,              /**< 自动播放速率开启 */

    AUTORATE_MODE_BUTT                  /**< 最大值 */
} autorate_mode_t;

typedef enum {
    SOURCE_TX,                          /**< 发射源 */
    SOURCE_RX,                          /**< 接收源 */

    SOURCE_BUTT                         /**< 最大值 */
} device_source_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
