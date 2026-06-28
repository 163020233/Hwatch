/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux event define \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef UX_EVENT_DEFINE_H
#define UX_EVENT_DEFINE_H
#include "ux_config.h"

#define INVALID_EVENT (uint16_t)(0xFFFF)
#define get_event(type, event) (uint16_t)(((type) << 8) | (event))
#define parse_event_type(event) (uint8_t)((event) >> 8)
#define parse_event(event) (uint8_t)((event) & 0x00FF)
#define arry_num(array) (sizeof(array) / sizeof((array)[0]))
#define UX_EVENT_USB_UAC_NUM 13                          /**< USB UAC事件数量 */

typedef enum {
    UX_EVENT_TYPE_MASTER = 0,             /**< 主机事件类型 */
    UX_EVENT_TYPE_POWER,                  /**< 电源事件类型 */
    UX_EVENT_TYPE_BG,                     /**< 蓝牙&星闪事件类型 */
    UX_EVENT_TYPE_AUDIO,                  /**< 音频事件类型 */
    UX_EVENT_TYPE_USB,                    /**< USB事件类型 */
    UX_EVENT_TYPE_COMMU = 5,              /**< 通信事件类型 */
    UX_EVENT_TYPE_SPP_HOST,               /**< SPP主机事件类型 */
    UX_EVENT_TYPE_BLE_HOST,               /**< BLE主机事件类型 */
    UX_EVENT_TYPE_CMD_SYNC,               /**< 命令同步事件类型 */
    UX_EVENT_TYPE_POWER_MGR,              /**< 电源管理事件类型 */
    UX_EVENT_TYPE_TONE = 10,              /**< 提示音事件类型 */
    UX_EVENT_TYPE_PLAYER,                 /**< 本地播放器事件类型 */
    UX_EVENT_TYPE_BS_REPORT,              /**< BS上报事件类型 */
    UX_EVENT_TYPE_SLE_REPORT,             /**< SLE上报事件类型 */
    UX_EVENT_TYPE_DONGLE,                 /**< Dongle事件类型 */
    UX_EVENT_TYPE_CHARGE,                 /**< 充电事件类型 */
    UX_EVENT_TYPE_HAID = 15,              /**< 助听事件类型 */
    UX_EVENT_TYPE_PAIRING_TIMER,          /**< 配对定时器事件类型 */
    /** 以下事件待逐步废弃 */
    UX_EVENT_TYPE_BOX,                    /**< 盒事件类型 */
    UX_EVENT_TYPE_PAIRING,                /**< 配对事件类型 */
    UX_EVENT_TYPE_BUTTON,                 /**< 按键事件类型 */
    UX_EVENT_TYPE_TOUCHPAD,               /**< 触摸板事件类型 */
    UX_EVENT_TYPE_WEAR,                   /**< 佩戴事件类型 */
    UX_EVENT_TYPE_CONNECT,                /**< 连接事件类型 */

    UX_EVENT_TYPE_POWER_ACTIVITY,         /**< 电源活动事件类型 */
    UX_EVENT_TYPE_PROTOCOL_ACTIVITY,      /**< 协议活动事件类型 */

    UX_EVENT_TYPE_BUTT                    /**< 最大值 */
} ux_event_type_t;

typedef enum {
    UX_BS_MSG_START = 0,                        /**< 起始消息 */
    UX_BS_MSG_SLE_STACK_STATE,                  /**< 协议栈使能状态上报 */
    UX_BS_MSG_SLE_CONNECT_STATE,                /**< 设备连接状态上报 */
    UX_BS_MSG_SLE_TWS_CONNECT_STATE,            /**< 双耳连接状态上报 */
    UX_BS_MSG_SLE_TWS_ROLE_REPORT,              /**< TWS耳机主副耳角色上报 */
    UX_BS_MSG_SLE_TWS_HANDOVER_STATE = 5,       /**< TWS耳机主副切状态上报 */
    UX_BS_MSG_SLE_OBJ_INFO_REPORT,              /**< 星闪设备机信息上报 */
    UX_BS_MSG_SLE_OBJ_INFO_RECOVER,             /**< 星闪设备集信息恢复 */
    UX_BS_MSG_SLE_PAIR_COMPLETE,                /**< 配对完成事件上报 */
    UX_BS_MSG_SLE_CALL_STATUS_NOTIFY,           /**< 通话状态上报 */
    UX_BS_MSG_SLE_MEDIA_INFO_UPDATE = 10,       /**< 媒体信息上报 */
    UX_BS_MSG_SLE_STREAM_CODEC_REPORT,          /**< 音频流编解码信息上报 */
    UX_BS_MSG_SLE_STREAM_STATE_REPORT,          /**< 音频流状态上报 */
    UX_BS_MSG_SLE_SINGLE_PDL_REPORT,            /**< 单条配对信息上报 */
    UX_BS_MSG_SLE_CONN_ORDER_REPORT,            /**< 回连优先级信息上报 */
    UX_BS_MSG_COMMON_STACK_STATE = 15,          /**< 协议栈使能状态上报 */
    UX_BS_MSG_COMMON_ACTIVE_DEVICE_REPORT,      /**< 活跃设备上报 */
    UX_BS_MSG_COMMON_SINGLE_PDL_REPORT,         /**< 蓝牙配对信息上报 */
    UX_BS_MSG_COMMON_CONNECT_STATE_REPORT,      /**< 蓝牙连接状态上报 */
    UX_BS_MSG_COMMON_CONNECT_ORDER_REPORT,      /**< 蓝牙回连顺序上报 */
    UX_BS_MSG_COMMON_PAIRING_STATE_REPORT = 20, /**< 蓝牙配对起始状态上报 */
    UX_BS_MSG_COMMON_WARN_TONE_SYNC_REPORT,     /**< 双耳同步提示音上报 */
    UX_BS_MSG_SLE_DISCOVERY_STATE_REPORT,       /**< 星闪配对起始状态上报 */
    UX_BS_MSG_SLE_AUTO_CONN_STATE_REPORT,       /**< 星闪回连起始状态上报 */
    UX_BS_MSG_SLE_CALL_VOLUME_UPDATE_REPORT,    /**< 星闪通话音量上报 */
    UX_BS_MSG_SLE_MEDIA_VOLUME_UPDATE_REPORT,   /**< 星闪媒体音量上报 */
    UX_BS_MSG_SLE_CALL_TERMINATE_NOTIFY = 26,   /**< 通话结束状态上报 */
    UX_BS_MSG_COMBO_DISCOVERY_STATE_REPORT,     /**< 双模模式配对起始状态上报 */
    UX_BS_MSG_COMBO_AUTO_CONN_STATE_REPORT,     /**< 双模模式回连起始状态上报 */
    UX_BS_MSG_COMBO_PDL_INFO_REPORT,            /**< 双模模式回连信息上报 */
    UX_BS_MSG_COMBO_CONNECT_ORDER_REPORT,       /**< 双模模式回连顺序上报 */

    UX_BS_MSG_BUTT,                             /**< 最大值 */
} ux_event_bs_msg_t;

typedef enum {
    UX_BG_SLE_PAIRING_START,            /**< 星闪配对开始 */
    UX_BG_SLE_PAIRING_STOP,             /**< 星闪配对停止 */
    UX_BG_SLE_PAIRED,                   /**< 星闪已配对 */
    UX_BG_SLE_CONNECTED,                /**< 星闪已连接 */
    UX_BG_SLE_DISCONNECTED,             /**< 星闪已断连 */
    UX_BG_SLE_STREAM_REQUEST,           /**< 星闪流请求 */
    UX_BG_SLE_CREATE_AUDIO_PATH,        /**< 星闪创建音频通路 */
    UX_BG_SLE_DESTROY_AUDIO_PATH,       /**< 星闪销毁音频通路 */
    UX_BG_SLE_AUTORATE_CHANGE_REQUEST,  /**< 星闪自动播放速率变更请求 */
    UX_BG_SLE_TRANSMIT_MODE_CHANGE,     /**< transmit mode切换 */

    UX_BG_BT_CONNECTED,                 /**< 蓝牙已连接 */
    UX_BG_BT_DISCONNECTED,              /**< 蓝牙已断连 */

    UX_BG_MAX                           /**< 最大值 */
} ux_event_bg_t;

typedef enum {
    UX_POWER_BEGIN = 0,               /**< 电源事件起始 */
    UX_POWER_KEY = UX_POWER_BEGIN,    /**< 电源键事件 */
    UX_POWER_ON,                      /**< 开机事件 */
    UX_POWER_OFF,                     /**< 关机事件 */
    UX_POWER_ON_ACK,                  /**< 开机应答 */
    UX_POWER_OFF_ACK,                 /**< 关机应答 */
    UX_ACC_ON,                        /**< 上电状态 */
    UX_ACC_OFF,                       /**< 下电状态 */
    UX_ACC_OFF_ACK,                   /**< 单核下电回复 */
    UX_POWER_STANDBY,                 /**< 待机状态 */
    UX_POWER_RESET,                   /**< 重启 */
    UX_POWER_SHUTDOWN,                /**< 关盒下电 */
    UX_POWER_STARTUP,                 /**< 开盒上电 */
    UX_POWER_BT_POWER_ON,             /**< BT核上电 */
    UX_POWER_BT_POWER_OFF,            /**< BT核下电 */
    UX_POWER_HIFI_POWER_ON,           /**< HIFI核上电 */
    UX_POWER_HIFI_POWER_OFF,          /**< HIFI核下电 */
    UX_POWER_ACC_OFF,                 /**< 无需特殊处理的下电 */
    UX_POWER_OFF_TRIGGER,             /**< 触发power off */
    UX_POWER_END,                     /**< 电源事件结束 */

    UX_POWER_MAX                      /**< 最大值 */
} ux_event_power_t;

typedef enum {
    UX_CHARGE_START = 0,                    /**< 充电事件起始 */
    UX_CHARGE_PLUG_IN = UX_CHARGE_START,    /**< 在盒内 */
    UX_CHARGE_PLUG_OUT,                     /**< 在盒外 */

    UX_CHARGE_IN_CHARGING,                  /**< 充电中 */
    UX_CHARGE_NO_CHARGING,                  /**< 未充电 */
    UX_CHARGE_DONE,                         /**< 充满电 */
    UX_CHARGE_BATTERY_LEVEL,                /**< 电量信息 */

    UX_CHARGE_LOWBATTERY_WARNING,           /**< 低电量通知 */
    UX_CHARGE_LOWBATTERY_RESUME,            /**< 低电量恢复通知 */
    UX_CHARGE_LOWBATTERY_POWER_OFF,         /**< 低电量关机 */
    UX_CHARGE_BATTERY_NOTIFY,               /**< 通知BT电量发生变化 */

    UX_CHARGE_POWER_OFF,                    /**< 通知电源模块关机 */
    UX_CHARGE_POWER_ON,                     /**< 通知电源模块开机 */

    UX_CHARGE_BUTT                          /**< 最大值 */
} ux_event_charge_t;

typedef enum {
    UX_USB_CONNECT = 0,                     /**< USB连接 */
    UX_USB_DISCONNECT,                      /**< USB断连 */
    UX_USB_UAC_PARAM_UPDATE,                /**< USB UAC参数更新 */
    UX_USB_UPDATE_UAC_CHANNELS,             /**< USB UAC通道更新 */
    UX_USB_UAC_UPLINK_START,                /**< UAC上行流创建 */
    UX_USB_UAC_UPLINK_STOP,                 /**< UAC上行流销毁 */

    UX_USB_SERVICE_MAX                      /**< 最大值 */
} ux_event_usb_t;

typedef enum {
    UX_PROTOCOL_ACTIVITY_DIAG_MSG = 0,      /**< 诊断消息 */
    UX_PROTOCOL_ACTIVITY_HID_MSG,           /**< HID消息 */
    UX_PROTOCOL_ACTIVITY_OTA_MSG,           /**< OTA消息 */
    UX_PROTOCOL_ACTIVITY_CUSTOMER_MSG,      /**< 客户消息 */

    UX_PROTOCOL_ACTIVITY_MAX = 0xff,        /**< 最大值 */
} ux_event_protocol_activity_t;

typedef enum {
    UX_MASTER_START = 0,                         /**< 主机事件起始 */
    UX_MASTER_POWER_ON = UX_MASTER_START,        /**< 开机 */
    UX_MASTER_POWER_ON_ACK = 1,                  /**< 开机应答 */
    UX_MASTER_POWER_OFF = 2,                     /**< 关机 */
    UX_MASTER_POWER_OFF_ACK = 3,                 /**< 关机应答 */
    UX_MASTER_POWER_ON_NOTIFICA = 4,             /**< 开机通知 */
    UX_MASTER_POWER_OFF_NOTIFICA = 5,            /**< 关机通知 */
    UX_MASTER_POWER_INQUIRY = 6,                 /**< 电源查询 */
    UX_MASTER_ACC_ON = 7,                        /**< ACC开启 */
    UX_MASTER_ACC_OFF = 8,                       /**< ACC关闭 */
    UX_MASTER_ACC_OFF_ACK = 9,                   /**< ACC关闭应答 */
    UX_MASTER_POWER_STANDBY = 10,                /**< 电源待机 */
    UX_MASTER_BT_POWER_ON_ACK = 11,              /**< BT核开机应答 */
    UX_MASTER_BT_CONFIG_INFO = 12,               /**< BT配置信息 */
    UX_MASTER_BT_CONFIG_INFO_ACK = 13,           /**< BT配置信息应答 */
    UX_MASTER_BT_POWER_ON = 14,                  /**< BT核开机 */
    UX_MASTER_BT_POWER_OFF = 15,                 /**< BT核关机 */
    UX_MASTER_HIFI_POWER_ON = 16,                /**< HIFI核开机 */
    UX_MASTER_HIFI_POWER_OFF = 17,               /**< HIFI核关机 */
    UX_MASTER_POWER_BUTTON_KEY = 18,             /**< 电源按键 */
    UX_MASTER_POWER_BOX_KEY = 19,                /**< 盒电源按键 */
    UX_MASTER_POWER_OFF_CB = 20,                 /**< 关机回调 */

    UX_MASTER_HAID_MODE_START = 0x20,            /**< 助听模式开始 */
    UX_MASTER_HAID_MODE_START_RESP,              /**< 助听模式开始应答 */
    UX_MASTER_HAID_MODE_STOP,                    /**< 助听模式停止 */
    UX_MASTER_HAID_MODE_STOP_RESP,               /**< 助听模式停止应答 */
    UX_MASTER_HAID_FIT_START,                    /**< 助听验配开始 */
    UX_MASTER_HAID_FIT_START_RESP,               /**< 助听验配开始应答 */
    UX_MASTER_HAID_FIT_STOP,                     /**< 助听验配停止 */
    UX_MASTER_HAID_FIT_STOP_RESP,                /**< 助听验配停止应答 */
    UX_MASTER_HAID_ASSISTANT,                    /**< 助听助手 */
    UX_MASTER_HAID_ASSISTANT_RESP,               /**< 助听助手应答 */
    UX_MASTER_HAID_CHANGE_MODE,                  /**< 助听模式切换 */
    UX_MASTER_HAID_CHANGE_MODE_RESP,             /**< 助听模式切换应答 */
    UX_MASTER_HAID_VOLUMEUP,                     /**< 助听音量增加 */
    UX_MASTER_HAID_VOLUMEDOWN,                   /**< 助听音量减少 */
    UX_MASTER_HAID_UPDATE_SCENE_MODE,            /**< 助听场景模式更新 */

    UX_MASTER_SOUND_CALL_INCOMING = 0x40,        /**< 手机来电 */
    UX_MASTER_SOUND_CALL_ANSWER = 65,            /**< 耳机侧请求接通电话 */
    UX_MASTER_SOUND_CALL_ANSWER_PHONE = 66,      /**< 手机已接通电话 */
    UX_MASTER_SOUND_CALL_HANGUP = 67,            /**< 耳机侧请求挂断电话 */
    UX_MASTER_SOUND_CALL_HANGUP_PHONE = 68,      /**< 手机已挂断电话 */
    UX_MASTER_SOUND_CALL_REJECT = 69,            /**< 耳机侧请求拒接电话 */
    UX_MASTER_SOUND_CALL_REJECT_PHONE = 70,      /**< 手机已拒接电话 */
    UX_MASTER_SOUND_CALL_VOLUMEUP = 71,          /**< 耳机侧请求提高听筒音量 */
    UX_MASTER_SOUND_CALL_VOLUMEDOWN = 72,        /**< 耳机侧请求降低听筒音量 */
    UX_MASTER_SOUND_CALL_DIALING = 73,           /**< 手机主动拨打电话 */
    UX_MASTER_SOUND_CALL_PHONE_ACTIVE = 74,      /**< 手机侧出声 */
    UX_MASTER_SOUND_CALL_EARPHONE_ACTIVE = 75,   /**< 耳机侧出声 */
    UX_MASTER_SOUND_CALL_VOLUME_SYNC = 76,       /**< 通话音量同步 */

    UX_MASTER_SOUND_MUSIC_PLAY = 77,             /**< 耳机侧请求播放音乐 */
    UX_MASTER_SOUND_MUSIC_PLAY_PHONE = 78,       /**< 手机侧已播放音乐 */
    UX_MASTER_SOUND_MUSIC_PLAY_NEXT = 79,        /**< 耳机侧请求播放下一首 */
    UX_MASTER_SOUND_MUSIC_PLAY_PRE = 80,         /**< 耳机侧请求播放上一首 */
    UX_MASTER_SOUND_MUSIC_STOP = 81,             /**< 耳机侧请求停止音乐 */
    UX_MASTER_SOUND_MUSIC_PAUSE = 82,            /**< 耳机侧请求暂停音乐 */
    UX_MASTER_SOUND_MUSIC_PAUSE_PHONE = 83,      /**< 手机侧已暂停音乐 */
    UX_MASTER_SOUND_MUSIC_VOLUMEUP = 84,         /**< 耳机侧请求提高媒体音量 */
    UX_MASTER_SOUND_MUSIC_VOLUMEDOWN = 85,       /**< 耳机侧请求降低媒体音量 */
    UX_MASTER_VOICE_ASSISTANT = 86,              /**< 耳机侧请求拉起手机语音助手 */
    UX_MASTER_MAIN_STATE_CHANGE = 87,            /**< 主状态变化 */
    UX_MASTER_SOUND_MUTE = 88,                   /**< 静音 */
    UX_MASTER_SOUND_UNMUTE = 89,                 /**< 取消静音 */
    UX_MASTER_SOUND_SINK_A2DP_START = 90,        /**< A2DP Sink开始 */
    UX_MASTER_SOUND_SINK_A2DP_STOP = 91,         /**< A2DP Sink停止 */
    UX_MASTER_SOUND_SCO_START = 92,              /**< SCO开始 */
    UX_MASTER_SOUND_SCO_STOP = 93,               /**< SCO停止 */
    UX_MASTER_SOUND_SLE_MUSIC_START = 94,        /**< SLE音乐开始 */
    UX_MASTER_SOUND_SLE_MUSIC_STOP = 95,         /**< SLE音乐停止 */
    UX_MASTER_SOUND_SLE_VOICE_START = 96,        /**< SLE语音开始 */
    UX_MASTER_SOUND_SLE_VOICE_STOP = 97,         /**< SLE语音停止 */
    UX_MASTER_SOUND_MUSIC_VOLUME_SYNC = 98,      /**< 音乐音量同步 */
    UX_MASTER_SOUND_CALL_RINGING = 99,           /**< 对端已振铃 */
    UX_MASTER_SOUND_MUSIC_ATTR_DISABLE = 100,    /**< 关闭获取音乐歌曲属性信息功能 */
    UX_MASTER_SOUND_MUSIC_ATTR_ENABLE = 101,     /**< 打开获取音乐歌曲属性信息功能 */

    UX_MASTER_RECOVER_START = 0xB0,              /**< 恢复开始 */
    UX_MASTER_RECOVER_END,                       /**< 恢复结束 */
    UX_MASTER_OTA_START,                         /**< UX向BTH通知OTA开始 */
    UX_MASTER_OTA_RESULT,                        /**< UX向BTH通知OTA结果（目前只有失败时通知） */
    UX_MASTER_OTA_REQ,                           /**< OTA向UX请求升级 */
    UX_MASTER_OTA_REQ_ACK,                       /**< UX向OTA回复请求升级的结果 */
    UX_MASTER_OTA_GET_MESSAGE_REQ,               /**< OTA向UX请求信息 */
    UX_MASTER_OTA_GET_MESSAGE_REQ_ACK,           /**< UX向OTA回复请求的信息 */
    UX_MASTER_OTA_INFORM,                        /**< UX向OTA发送通知 */
    UX_MASTER_OTA_FAIL,                          /**< OTA向UX发送失败通知 */
    UX_MASTER_OTA_SUCC,                          /**< OTA向UX发送成功通知 */
    UX_MASTER_DUT_START,                         /**< DUT开始 */
    UX_MASTER_DUT_END,                           /**< DUT结束 */
    UX_MASTER_PLAY_TONE,                         /**< 播放提示音 */
    UX_MASTER_WEAR_CONTROL,                      /**< 佩戴控制 */
    UX_MASTER_WEAR_INFO,                         /**< 佩戴信息 */
    UX_MASTER_WEAR_DETECTION_SWITCH,             /**< 佩戴检测开关 */
    UX_MASTER_DUAL_CONNECTION_SWITCH,            /**< 双设备连接开关 */
    UX_MASTER_LOW_LATENCY_SWITCH,                /**< 低时延开关 */
    UX_MASTER_POSITION_INFO,                     /**< 位置信息 */
    UX_MASTER_BLE_CONN_INFO,                     /**< BLE连接状态通知 */
    UX_MASTER_APP_INFO,                          /**< APP修改NV值通知整机 */
    UX_MASTER_RESET_INFO,                        /**< 整机RESET */
    UX_MASTER_DUT_INIT,                          /**< DUT模式下初始化 */
    UX_MASTER_DUT_INIT_RSP,                      /**< DUT模式下BT初始化rsp */
    UX_MASTER_MEDICAL_TEST_ENTER,                /**< 进入医规测试 */
    UX_MASTER_MEDICAL_TEST_EXIT,                 /**< 退出医规测试 */
    UX_MASTER_SET_GAIN,                          /**< 设置增益 */
    UX_MASTER_SET_DENOISE_MODE,                  /**< 设置降噪使能 */
    UX_MASTER_LOW_CUT_ENABLE,                    /**< 设置低切使能 */
    UX_MASTER_SET_REC_BIT,                       /**< 设置录音格式 */
    UX_MASTER_SET_MUTE_ENABLE,                   /**< 设置静音使能状态 */
    UX_MASTER_GET_REMAIN_TIME_REQ,               /**< 获取剩余录制时间请求 */
    UX_MASTER_GET_REMAIN_TIME_REQ_ACK,           /**< 获取剩余录制时间请求应答 */
    UX_MASTER_ISO_CONNECT,                       /**< ISO通路创建 */
    UX_MASTER_DEVICE_NAME_INFO,                  /**< 名称更新通知 */

    UX_MASTER_RECORDER_START,                    /**< 录音开始 */
    UX_MASTER_RECORDER_STOP,                     /**< 录音停止 */

    UX_MASTER_PAIR_TRX,                          /**< 配对TRX */
    UX_MASTER_PAIR_PHONE,                        /**< 配对手机 */
    UX_MASTER_UNPAIR_TRX,                        /**< 取消配对TRX */
    UX_MASTER_UNPAIR_PHONE,                      /**< 取消配对手机 */
    UX_MASTER_UAC_UPLINK_STREAM_SET,             /**< UAC上行通路创建结果 */
    UX_MASTER_USB_DETECT_STATE,                  /**< USB插拔状态变化 */
    UX_MASTER_SOUND_MUTE_HAID_CTRL,              /**< 音频静音时助听处理 */
    UX_MASTER_SOUND_UNMUTE_HAID_CTRL,            /**< 音频恢复时助听处理 */
    UX_MASTER_HAID_AFC_CALIBRATE_START,          /**< 开启啸叫校准 */
    UX_MASTER_HAID_AFC_CALIBRATE_START_RESP,     /**< 开启啸叫校准回复 */
    UX_MASTER_HAID_AFC_CALIBRATE_END,            /**< 啸叫校准完成 */
    UX_MASTER_HAID_AFC_CALIBRATE_END_RESP,       /**< 啸叫校准完成回复 */
    UX_MASTER_HAID_AFC_CALIBRATE_STOP,           /**< 停止啸叫校准 */
    UX_MASTER_HAID_AFC_CALIBRATE_STOP_RESP,      /**< 停止啸叫校准回复 */

    UX_MASTER_MAX,                               /**< 最大值 */
    UX_MASTER_NONE = UX_MASTER_MAX,              /**< 无效值 */
} ux_event_master_t;

typedef enum {
    UX_BOX_START = 0,                /**< 盒事件起始 */
    UX_BOX_INBOX = UX_BOX_START,     /**< 入盒 */
    UX_BOX_OUTBOX,                   /**< 出盒 */
    UX_BOX_POWER_ON,                 /**< 盒开机 */
    UX_BOX_POWER_OFF,                /**< 盒关机 */

    UX_BOX_COVER_OPEN,               /**< 开盖 */
    UX_BOX_COVER_CLOSE,              /**< 合盖 */
    UX_BOX_BATTERY_NOTIFY,           /**< 盒电量通知 */

    UX_BOX_BUTT                      /**< 最大值 */
} ux_event_box_t;

typedef enum {
    UX_BUTTON1_START = 0,            /**< 按键1起始 */
    UX_BUTTON1_CLICKUP,              /**< 按键1短按释放 */
    UX_BUTTON1_CLICKUP_2,            /**< 按键1双击释放 */
    UX_BUTTON1_CLICKUP_4,            /**< 按键1四击释放 */
    UX_BUTTON1_CLICKUP_5,            /**< 按键1五击释放 */
    UX_BUTTON1_CLICKUP_6,            /**< 按键1六击释放 */
    UX_BUTTON1_CLICKDOWN,            /**< 按键1按下 */

    UX_BUTTON1_LONGPRESS_1S,         /**< 按键1长按1秒 */
    UX_BUTTON1_LONGPRESS_2S,         /**< 按键1长按2秒 */
    UX_BUTTON1_LONGPRESS_3S,         /**< 按键1长按3秒 */
    UX_BUTTON1_LONGPRESS_5S,         /**< 按键1长按5秒 */
    UX_BUTTON1_LONGPRESS_10S,        /**< 按键1长按10秒 */
    UX_BUTTON1_INVILID,              /**< 按键1无效 */

    UX_BUTTON2_START,                /**< 按键2起始 */
    UX_BUTTON2_CLICKUP,              /**< 按键2短按释放 */
    UX_BUTTON2_CLICKUP_2,            /**< 按键2双击释放 */
    UX_BUTTON2_CLICKUP_4,            /**< 按键2四击释放 */
    UX_BUTTON2_CLICKUP_5,            /**< 按键2五击释放 */
    UX_BUTTON2_CLICKUP_6,            /**< 按键2六击释放 */
    UX_BUTTON2_CLICKDOWN,            /**< 按键2按下 */

    UX_BUTTON2_LONGPRESS_1S,         /**< 按键2长按1秒 */
    UX_BUTTON2_LONGPRESS_2S,         /**< 按键2长按2秒 */
    UX_BUTTON2_LONGPRESS_3S,         /**< 按键2长按3秒 */
    UX_BUTTON2_LONGPRESS_5S,         /**< 按键2长按5秒 */
    UX_BUTTON2_LONGPRESS_10S,        /**< 按键2长按10秒 */
    UX_BUTTON2_INVILID,              /**< 按键2无效 */

    UX_BUTTON3_START,                /**< 按键3起始 */
    UX_BUTTON3_CLICKUP,              /**< 按键3短按释放 */
    UX_BUTTON3_CLICKUP_2,            /**< 按键3双击释放 */
    UX_BUTTON3_CLICKUP_4,            /**< 按键3四击释放 */
    UX_BUTTON3_CLICKUP_5,            /**< 按键3五击释放 */
    UX_BUTTON3_CLICKUP_6,            /**< 按键3六击释放 */
    UX_BUTTON3_CLICKDOWN,            /**< 按键3按下 */

    UX_BUTTON3_LONGPRESS_1S,         /**< 按键3长按1秒 */
    UX_BUTTON3_LONGPRESS_2S,         /**< 按键3长按2秒 */
    UX_BUTTON3_LONGPRESS_3S,         /**< 按键3长按3秒 */
    UX_BUTTON3_LONGPRESS_5S,         /**< 按键3长按5秒 */
    UX_BUTTON3_LONGPRESS_10S,        /**< 按键3长按10秒 */
    UX_BUTTON3_INVILID,              /**< 按键3无效 */

    UX_MIX_BUTTON_TYPE1,             /**< 混合按键类型1 */
    UX_MIX_BUTTON_TYPE2,             /**< 混合按键类型2 */
    UX_MIX_BUTTON_TYPE3,             /**< 混合按键类型3 */

    UX_BUTTON_BUTT                   /**< 最大值 */
} ux_event_button_t;

typedef enum {
    UX_TOUCHPAD_START = 0,                    /**< 触摸板事件起始 */
    UX_TOUCHPAD_LIGHT_TOUCH = UX_TOUCHPAD_START, /**< 触摸板轻触 */
    UX_TOUCHPAD_SLIP_UP,                      /**< 触摸板上滑 */
    UX_TOUCHPAD_SLIP_DOWN,                    /**< 触摸板下滑 */
    UX_TOUCHPAD_LONG_TOUCH,                   /**< 触摸板长按 */

    UX_TOUCHPAD_BUTT                          /**< 最大值 */
} ux_event_touchpad_t;

typedef enum {
    UX_PAIRING_START = 0,                       /**< 配对事件起始 */
    UX_PAIRING_CONN_DEVICE_REQ = UX_PAIRING_START, /**< 发起设备配对请求 */
    UX_PAIRING_RECONN_DEVICE,                   /**< 发起设备回连请求 */
    UX_CONN_DEVICE_RSP,                         /**< 设备已连接报告 */
    UX_PAIRING_DISCONN_DEVICE,                  /**< 发起设备断连请求 */
    UX_PAIRING_DISCONN_DEVICE_REPORT,           /**< 设备已断连报告 */

    UX_PAIRING_CONN_PEER_REQ = 5,               /**< 发起对耳配对请求 */
    UX_PAIRING_RECONN_PEER,                     /**< 发起对耳回连请求 */
    UX_CONN_PEER_RSP,                           /**< 对耳已连接报告 */
    UX_PAIRING_DISCONN_PEER_REPORT,             /**< 对耳已断连报告 */
    UX_PAIRING_STANDBY,                         /**< 发起待机请求 */

    UX_PAIRING_STANDBY_TWS_NOTIFY = 10,         /**< 对耳进待机报告 */
    UX_TWS_ROLE_REPORT,                         /**< 主副耳角色报告 */
    UX_PAIRING_WITHOUT_LOCATION,                /**< 无定位配对 */
    UX_PAIRING_POWER_OFF,                       /**< 关闭蓝牙业务 */
    UX_PAIRING_POWER_ON,                        /**< 打开蓝牙业务 */
    UX_CONN_PEER_DEVICE_REPORT,                 /**< 对耳与设备的连接状态报告 */

    UX_PAIRING_DISCOVER_PEER_REQ = 16,          /**< 发起对耳扫描请求 */
    UX_PAIRING_DISCOVER_PEER_RSP,               /**< 对耳扫描成功报告 */
    UX_CONN_PEER_SPP_RSP,                       /**< 对耳SPP连接状态报告 */
    UX_PAIRING_PAIRMODE_REPORT,                 /**< 进入和退出配对模式报告 */
    UX_PAIRING_STOP_RECONN_DEVICE,              /**< 停止回连设备请求 */
    UX_PAIRING_STOP_PAIR_DEVICE,                /**< 停止配对设备请求 */

    UX_PAIRING_BLE_DISCONNECT,                  /**< 发起断开BLE连接请求 */
    UX_PAIRING_BLE_START_DISCOVERY,             /**< 发起启动BLE广播请求 */
    UX_PAIRING_BLE_STOP_DISCOVERY,              /**< 发起停止BLE广播请求 */
    UX_PAIRING_BLE_EXPAND,                      /**< 携带BLE扩展字段数据启动BLE广播 */
    UX_PAIRING_BLE_EXPAND_END,                  /**< 携带BLE扩展字段数据以及用户定制的结束字段数据启动BLE广播 */

    UX_PAIRING_CLEAR_PDL,                       /**< 清除配对信息 */
    UX_PAIRING_PEER_UNLINK_REQ,                 /**< 清除对耳地址信息 */

    UX_PAIRING_BUTT                             /**< 最大值 */
} ux_event_pairing_t;

typedef enum {
    UX_WEAR_START = UX_BOX_BUTT + 1,    /**< 佩戴事件起始 */
    UX_WEAR_IN = UX_WEAR_START,         /**< 入耳 */
    UX_WEAR_OUT,                        /**< 出耳 */

    UX_WEAR_BUTT                        /**< 最大值 */
} ux_event_wear_t;

typedef enum {
    UX_ROLE_MASTER = 0,         /**< 主耳角色 */
    UX_ROLE_SLAVE,              /**< 副耳角色 */
} ux_event_tws_role_t;

typedef enum {
    UX_EVENT_RETCODE_SUCC,              /**< 成功 */
    UX_EVENT_RETCODE_FAIL,              /**< 失败 */

    UX_EVENT_RET_CODE_BUTT,             /**< 最大值 */
} ux_event_ret_code_t;

typedef enum {
    DISCONN_PEER_BY_LOCAL,         /**< 本端主动对耳断连 */
    DISCONN_PEER_BY_OVERRANGE,     /**< 超距对耳断连 */
    DISCONN_DEVICE_BY_LOCAL,       /**< 本端主动设备断连 */
    DISCONN_DEVICE_BY_OVERRANGE,   /**< 超距设备断连 */
    DISCONN_DEVICE_BY_PHONE,       /**< 手机侧主动断连 */
    DISCONN_DEVICE_BY_KEY_MISSING, /**< 手机侧删除配对信息断连 */

    DISCONN_CODE_BUTT,             /**< 最大值 */
} ux_event_disconn_code_t;

typedef enum {
    UX_EVENT_RET_HAID_VOL_DEFAULT,         /**< 助听音量默认 */
    UX_EVENT_RET_HAID_VOL_MAX,             /**< 助听音量最大 */
    UX_EVENT_RET_HAID_VOL_MIN,             /**< 助听音量最小 */

    UX_EVENT_RET_HAID_BUTT,                /**< 最大值 */
} ux_event_ret_haid_vol_t;

typedef enum {
    ACC_OFF_ALL = 0,               /**< ACC全部关闭 */
    ACC_OFF_BT,                    /**< ACC关闭BT */
    ACC_ON_BT,                     /**< ACC开启BT */
    ACC_OFF_RESET,                 /**< ACC关闭复位 */

    ACC_BUTT                       /**< 最大值 */
} acc_type_t;

typedef enum {
    INQUIRY_CHARGER_STATE = 0,     /**< 查询充电器状态 */
    INQUIRY_PLUG_STATE,            /**< 查询插拔状态 */
    INQUIRY_BATTERY_LEVEL,         /**< 查询电池电量 */
    INQUIRY_ALL_STATE,             /**< 查询所有状态 */

    INQUIRY_BUTT                   /**< 最大值 */
} power_inquiry_type_t;

typedef enum {
    WEAR_CONTROL_IN = 1,           /**< 强制设置耳机持续在入耳状态 */
    WEAR_CONTROL_OUT,              /**< 强制设置耳机持续不在入耳状态 */
    WEAR_CONTROL_SENSOR,           /**< 由设备自身的Sensor和算法检测入耳状态 */

    WEAR_CONTROL_BUTT              /**< 最大值 */
} wear_control_type_t;

typedef enum {
    BLE_CONN_STATE = 0,             /**< ble连接状态 */
    BLE_DISCONN_STATE,              /**< ble断开状态 */
    BLE_MSG_CHANNEL_READY,          /**< 消息通路创建完成 */

    BLE_CONN_BUTT                   /**< 最大值 */
} ble_conn_state_t;

typedef enum {
    APP_HAID_SWITCH_DELAY = 0,          /**< 音频切助听延时时间 */
    APP_POWER_ON_DELAY,                 /**< 开机延时时间 */
    APP_HAID_MIX_SWITCH,                /**< 混音模式 */
    APP_TOUCH_CONFIG,                   /**< touchpad是否生效 */
    APP_CONTROL_WEAR_STATUS,            /**< 佩戴检测模块控制 */
    APP_WEAR_DETECTION_STATUS,          /**< 佩戴检测开关 */
    APP_LOW_LATENCY_STATUS,             /**< 音频低时延开关 */
    APP_DUAL_CONNECTION_STATUS,         /**< 双设备连接开关 */
    APP_WEAR_SENSOR_DELAY,              /**< 入耳检测开启时延时开启助听时间 */
    APP_WEAR_IN_DELAY,                  /**< 强制入耳延时开启助听时间 */
    APP_BUTTON_CONFIG,                  /**< 按键控制 */
    APP_SET_RESET,                      /**< 重启 */
    APP_SET_EAR_SIDE,                   /**< 更新左右耳 */
    APP_FACTORY_MODE,                   /**< 恢复出厂设置 */
    APP_SET_TONE_ENABLE,                /**< 设置提示音使能或去使能 */
    APP_SET_SCENE_MODE_STATUS,          /**< 设置助听场景模式使能状态 */
    APP_SET_HAID_ENABLE_STATUS,         /**< 设置助听功能使能状态 */
    APP_SET_DEVICE_NAME,                /**< 设置耳机名称状态 */

    APP_NV_BUTT                         /**< 最大值 */
} app_nv_type_t;

typedef enum {
    BT_CONFIG_HAND_OVER_VALID,       /**< 支持主副切 */
    BT_CONFIG_HAND_OVER_FIXED_LEFT,  /**< 不支持主副切，固定左耳 */
    BT_CONFIG_HAND_OVER_FIXED_RIGHT, /**< 不支持主副切，固定右耳 */

    BT_CONFIG_HAND_OVER_BUTT,        /**< 最大值 */
} bt_hand_over_t;

typedef enum {
    ANC_START_LEVEL = 0,             /**< ANC起始级别 */
    ANC_CLOSE_LEVEL = ANC_START_LEVEL, /**< ANC关闭级别 */
    ANC_NORMAL_LEVEL,                /**< ANC普通级别 */
    ANC_ENHANCE_LEVEL,               /**< ANC增强级别 */

    ANC_BUTT_LEVEL                   /**< 最大值 */
} anc_level_type_t;

typedef enum {
    RECONN_DEVICE_BY_START,                /**< 回连设备开始 */
    RECONN_DEVICE_BY_LOCAL,                /**< 非超距设备回连 */
    RECONN_DEVICE_BY_OVERRANGE,            /**< 超距设备回连 */

    RECONN_CODE_BUTT,                      /**< 最大值 */
} ux_event_reconn_code_t;

typedef enum {
    UX_HAID_VOL_RANGE_DEFAULT,       /**< 助听音量范围默认 */
    UX_HAID_VOL_RANGE_MUTE,          /**< 助听音量范围静音 */
    UX_HAID_VOL_RANGE_UNMUTE,        /**< 助听音量范围非静音 */
    UX_HAID_VOL_RANGE_MIX,           /**< 助听音量范围混音 */
    UX_HAID_VOL_RANGE_RECOVER,       /**< 助听音量范围恢复 */
    UX_HAID_VOL_RANGE_MIX_VOL_UP,    /**< 助听音量范围混音增加 */
    UX_HAID_VOL_RANGE_MIX_VOL_DOWN,  /**< 助听音量范围混音减少 */

    UX_HAID_VOL_RANGE_BUTT,          /**< 最大值 */
} ux_haid_vol_range_t;

typedef enum {
    HAID_AFC_CALIBRATE_START_BEGIN = 0,      /**< 啸叫校准开始起始 */
    HAID_AFC_CALIBRATE_START_SUCC,           /**< 啸叫校准启动成功 */
    HAID_AFC_CALIBRATE_START_FAIL,           /**< 啸叫校准启动接口调用失败 */
    HAID_AFC_CALIBRATE_START_TIMER_FAIL,     /**< 啸叫校准定时器启动失败 */
    HAID_AFC_CALIBRATE_START_QUIT_FIT_FAIL,  /**< 啸叫校准启动前停验配失败 */
    HAID_AFC_CALIBRATE_START_QUIT_HAID_FAIL, /**< 啸叫校准启动前停助听失败 */
    HAID_AFC_CALIBRATE_START_ERR,            /**< 啸叫校准启动通用错误码 */

    HAID_AFC_CALIBRATE_START_BUTT            /**< 最大值 */
} haid_afc_start_t;

typedef enum {
    HAID_AFC_CALIBRATE_END_BEGIN = HAID_AFC_CALIBRATE_START_BUTT + 1, /**< 啸叫校准结束起始 */
    HAID_AFC_CALIBRATE_END_GET_HW_PARA_FAIL,       /**< 调用获取硬件参数接口失败 */
    HAID_AFC_CALIBRATE_END_WRITE_HW_NV_FAIL,       /**< 硬件参数写入Nv失败 */
    HAID_AFC_CALIBRATE_END_MALLOC_FAIL,            /**< 获取硬件参数申请内存失败 */
    HAID_AFC_CALIBRATE_END_SAVE_HW_PARA_SUCC,      /**< 硬件参数保存成功 */
    HAID_AFC_CALIBRATE_END_FAIL,                   /**< 调用获取啸叫校准结束接口失败 */
    HAID_AFC_CALIBRATE_END_SUCC,                   /**< 啸叫校准结束成功 */
    HAID_AFC_CALIBRATE_END_ERR,                    /**< 校准结束通用错误码 */

    HAID_AFC_CALIBRATE_END_BUTT                    /**< 最大值 */
} haid_afc_end_t;

typedef enum {
    HAID_AFC_CALIBRATE_STOP_BEGIN = HAID_AFC_CALIBRATE_END_BUTT + 1, /**< 啸叫校准停止起始 */
    HAID_AFC_CALIBRATE_STOP_FAIL,            /**< 啸叫校准停止接口调用失败 */
    HAID_AFC_CALIBRATE_STOP_SUCC,            /**< 啸叫校准停止成功 */
    HAID_AFC_CALIBRATE_STOP_ERR,             /**< 校准停止通用错误码 */

    HAID_AFC_CALIBRATE_STOP_BUTT             /**< 最大值 */
} haid_afc_stop_t;

typedef enum {
    A2DP_PLAY_UNMUTE,  /**< A2DP链路在时DSP检测非静音帧解除静音 */
    A2DP_STOP_UNMUTE,  /**< A2DP通路断开时解除静音 */

    A2DP_UNMUTE_BUTT,  /**< 最大值 */
} a2dp_unmute_t;

typedef enum {
    UX_AUDIO_JACK_PLUG_IN = 0,           /**< 音频插孔插入 */
    UX_AUDIO_JACK_PLUG_OUT,              /**< 音频插孔拔出 */
    UX_AUDIO_LINEIN_MUSIC_START,         /**< LINEIN音乐开始 */
    UX_AUDIO_LINEIN_MUSIC_STOP,          /**< LINEIN音乐停止 */
    UX_AUDIO_TOGGLE_MUTE,                /**< 切换静音 */
    UX_AUDIO_MEDIA_CALL_START,           /**< 媒体通话开始 */
    UX_AUDIO_MEDIA_CALL_STOP,            /**< 媒体通话停止 */
    UX_AUDIO_MEDIA_MUSIC_START,          /**< 媒体音乐开始 */
    UX_AUDIO_MEDIA_MUSIC_STOP,           /**< 媒体音乐停止 */
    UX_AUDIO_VOLUME_CHANGED,             /**< 音量变化 */

    UX_AUDIO_MAX,                        /**< 最大值 */
} ux_event_audio_t;

typedef enum {
    UX_AUDIO_TONE_PLAY = 0,        /**< 提示音播放 */
    UX_AUDIO_TONE_STOP,            /**< 提示音停止 */
    UX_AUDIO_ALL_TONE_STOP,        /**< 所有提示音停止 */
    UX_AUDIO_TONE_COMPLETE,        /**< 提示音完成 */

    UX_AUDIO_TONE_MAX,             /**< 最大值 */
} ux_event_tone_t;

typedef enum {
    UX_PLAYER_EVENT_SET_SOURCE_PATH = 0,    /**< 设置播放源路径 */
    UX_PLAYER_EVENT_START,                  /**< 本地媒体播放 */
    UX_PLAYER_EVENT_PAUSE,                  /**< 本地媒体暂停 */
    UX_PLAYER_EVENT_RESUME,                 /**< 本地媒体恢复 */
    UX_PLAYER_EVENT_STOP,                   /**< 本地媒体销毁 */
    UX_PLAYER_EVENT_GET_PLAY_STATES,        /**< 获取播放状态 */
    UX_PLAYER_EVENT_MAX,                    /**< 最大值 */
} ux_event_player_t;

typedef enum {
    UX_DONGLE_BG_SLE_CONNECTED = 0,        /**< Dongle星闪已连接 */
    UX_DONGLE_BG_SLE_DISCONNECTED,         /**< Dongle星闪已断连 */
    UX_DONGLE_BG_SLE_SET_DEFAULT_VOLUME,   /**< Dongle星闪设置默认音量 */
    UX_DONGLE_BG_AUDIO_ABILITY,            /**< Dongle音频能力 */
    UX_DONGLE_ERAPHONE_CTRL_NOTIFY,        /**< Dongle耳机控制通知 */
    UX_DONGLE_UAC_BUTTON_CTRL_NOTIFY,      /**< Dongle UAC按钮控制通知 */
    UX_DONGLE_UAC_AUDIO_CTRL_NOTIFY,       /**< Dongle UAC音频控制 */
    UX_DONGLE_USB_UAC_EVENT_REPORT,        /**< Dongle USB UAC事件上报 */
    UX_DONGLE_DUT_START,                   /**< Dongle DUT开始 */
    UX_DONGLE_DUT_END,                     /**< Dongle DUT结束 */

    UX_DONGLE_MAX,                         /**< 最大值 */
} ux_event_dongle_t;

typedef enum {
    UX_USB_UAC_UNBIND                       = (1 << 0),  /**< USB UAC解除绑定 */
    UX_USB_UAC_SET_CONFIG                   = (1 << 1),  /**< USB UAC设置配置 */
    UX_USB_UAC_MIC_START                    = (1 << 2),  /**< USB UAC麦克风开始 */
    UX_USB_UAC_MIC_STOP                     = (1 << 3),  /**< USB UAC麦克风停止 */
    UX_USB_UAC_MUTE                         = (1 << 4),  /**< USB UAC静音 */
    UX_USB_UAC_VOLUME_INC                   = (1 << 5),  /**< USB UAC音量增加 */
    UX_USB_UAC_VOLUME_DEC                   = (1 << 6),  /**< USB UAC音量减少 */
    UX_USB_UAC_MIC_SAMPLE_RATE_UPDATE       = (1 << 7),  /**< USB UAC麦克风采样率更新 */
    UX_USB_UAC_SPEAKER_SAMPLE_RATE_UPDATE   = (1 << 8),  /**< USB UAC扬声器采样率更新 */
    UX_USB_UAC_MIC_DEPTH_UPDATE             = (1 << 9),  /**< USB UAC麦克风位深更新 */
    UX_USB_UAC_SPEAKER_START                = (1 << 10), /**< USB UAC扬声器开始 */
    UX_USB_UAC_SPEAKER_STOP                 = (1 << 11), /**< USB UAC扬声器停止 */

    UX_USB_UAC_EVENT_MAX                    = (1 << 12), /**< 最大值 */
} ux_event_usb_uac_t;

typedef enum {
    UX_HAID_SENCE_START_EVENT = 0,        /**< 进入助听事件 */
    UX_HAID_SENCE_STOP_EVENT,             /**< 退出助听事件 */
    UX_HAID_VOL_UP_EVENT,                 /**< 助听音量增大事件 */
    UX_HAID_VOL_DOWN_EVENT,               /**< 助听音量减小事件 */
    UX_HAID_POWER_ON_EVENT,               /**< 开机事件处理 */
    UX_HAID_POWER_OFF_EVENT,              /**< 关机事件处理 */
    UX_HAID_BLE_DISCONNECT_EVENT,         /**< ble断链事件 */
    UX_HAID_CALLING_STATE_EVENT,          /**< 助听来电状态事件 */
    UX_HAID_AFC_CALIBRATE_START_EVENT,    /**< 啸叫校准开始事件 */
    UX_HAID_AFC_CALIBRATE_STOP_EVENT,     /**< 停止啸叫校准事件 */
    UX_HAID_AFC_CALIBRATE_END_EVENT,      /**< 啸叫校准结束事件 */
    UX_HAID_SVR_EVENT_END
} ux_event_haid_t;

#endif
