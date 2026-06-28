/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux communication header.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-06, Create file. \n
 */
#ifndef UX_API_COMMUNICATION_H
#define UX_API_COMMUNICATION_H

#include <stdbool.h>
#include "stdint.h"
#include "errcode.h"
#include "diag_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COMMU_MSG_LEN_MAX 30                        /**< 通信消息最大长度 */

/**
 * @brief 通信命令类型枚举
 */
typedef enum {
    UX_API_COMMU_CMD_SET_START = 0x0,         /**< 设置指令起始 */
    UX_API_COMMU_CMD_SET_VOLUME_LEVEL,        /**< 设置音量 */
    UX_API_COMMU_CMD_SET_DENOISE_MODE,        /**< 设置降噪模式 */
    UX_API_COMMU_CMD_SET_EQ_MODE,             /**< 设置EQ模式 */
    UX_API_COMMU_CMD_SET_RECORDER_STATE,      /**< 设置录音开关 */
    UX_API_COMMU_CMD_SET_LOW_CUT,             /**< 设置低切开关 */
    UX_API_COMMU_CMD_SET_MUTE,                /**< 设置静音开关 */
    UX_API_COMMU_CMD_SET_REC_BIT,             /**< 设置录音格式 */
    UX_API_COMMU_CMD_SET_HOST_HID_CMD,        /**< 设置HID控制 */
    UX_API_COMMU_CMD_SET_SAMPLE_RATE,         /**< 设置录音采样率 */
    UX_API_COMMU_CMD_SET_BIT_WIDTH,           /**< 设置录音比特位宽 */
    UX_API_COMMU_CMD_SET_FILE_SPLIT_DURATION, /**< 设置录音文件分割时长 */
    UX_API_COMMU_CMD_SET_FILE_SPLIT_SIZE,     /**< 设置录音文件分割大小 */
    UX_API_COMMU_CMD_SET_FILE_FORMAT,         /**< 设置文件格式化 */
    UX_API_COMMU_CMD_SET_FILE_PATH,           /**< 设置文件目录 */
    UX_API_COMMU_CMD_SET_POWER_OFF,           /**< 设置关机 */
    UX_API_COMMU_CMD_SET_POWER_RESET,         /**< 设置重启 */
    UX_API_COMMU_CMD_SET_POWER_OFF_CANCEL,    /**< 设置取消关机 */
    UX_API_COMMU_CMD_SET_POWER_RESET_CANCEL,  /**< 设置取消重启 */
    UX_API_COMMU_CMD_SET_AUTO_POWER_OFF,      /**< 设置自动关机 */
    UX_API_COMMU_CMD_SET_SYS_TIME,            /**< 设置系统时间 */
    UX_API_COMMU_CMD_SET_RECOVERY,            /**< 设置Tx开始恢厂 */
    UX_API_COMMU_CMD_SET_REMOTE_NR_MODE,      /**< 设置对端降噪模式 */
    UX_API_COMMU_CMD_SET_REMOTE_VOLUME,       /**< 设置对端音量 */
    UX_API_COMMU_CMD_SET_REMOTE_MUTE,         /**< 设置对端静音 */
    UX_API_COMMU_CMD_SET_RECORDER_ALG_BYPASS, /**< 设置录音音效算法旁路状态 */
    UX_API_COMMU_CMD_SET_END,

    UX_API_COMMU_CMD_GET_START,                /**< 获取指令起始 */
    UX_API_COMMU_CMD_GET_MUTE_STATUS,          /**< 获取静音状态 */
    UX_API_COMMU_CMD_GET_RECORDER_STATE,       /**< 获取录音开关 */
    UX_API_COMMU_CMD_GET_RECORDER_REMAIN_TIME, /**< 获取剩余录音时长 */
    UX_API_COMMU_CMD_GET_AUTO_POWER_OFF,       /**< 获取自动关机 */
    UX_API_COMMU_CMD_GET_SAMPLE_RATE,          /**< 获取录音采样率 */
    UX_API_COMMU_CMD_GET_BIT_WIDTH,            /**< 获取录音比特位宽 */
    UX_API_COMMU_CMD_GET_FILE_SPLIT_DURATION,  /**< 获取录音文件分割时长 */
    UX_API_COMMU_CMD_GET_FILE_SPLIT_SIZE,      /**< 获取录音文件分割大小 */
    UX_API_COMMU_CMD_GET_FILE_PATH,            /**< 获取文件目录 */
    UX_API_COMMU_CMD_GET_LOW_CUT,              /**< 获取低切开关 */
    UX_API_COMMU_CMD_GET_MUTE,                 /**< 获取静音开关 */
    UX_API_COMMU_CMD_GET_VOLUME_LEVEL,         /**< 获取音量 */
    UX_API_COMMU_CMD_GET_DENOISE_MODE,         /**< 获取降噪模式 */
    UX_API_COMMU_CMD_GET_EQ_MODE,              /**< 获取EQ模式 */
    UX_API_COMMU_CMD_GET_SDK_VERSION,          /**< 获取SDK版本号 */
    UX_API_COMMU_CMD_GET_SYNC_DEMO,            /**< 同步获取demo示例 */
    UX_API_COMMU_CMD_GET_END,

    UX_API_COMMU_CMD_NOTIFY_START,               /**< 通知指令起始 */
    UX_API_COMMU_CMD_NOTIFY_BAT_LEVEL,           /**< 通知电量 */
    UX_API_COMMU_CMD_NOTIFY_DENOISE_MODE,        /**< 通知降噪模式 */
    UX_API_COMMU_CMD_NOTIFY_EQ_MODE,             /**< 通知EQ模式 */
    UX_API_COMMU_CMD_NOTIFY_FILE_PATH,           /**< 通知文件目录 */
    UX_API_COMMU_CMD_NOTIFY_RECORDER_STATE,      /**< 通知当前录音状态 */
    UX_API_COMMU_CMD_NOTIFY_LOW_CUT,             /**< 通知低切开关 */
    UX_API_COMMU_CMD_NOTIFY_MUTE_STATE,          /**< 通知当前静音状态 */
    UX_API_COMMU_CMD_NOTIFY_REMAIN_TIME,         /**< 通知剩余录制时间 */
    UX_API_COMMU_CMD_NOTIFY_VOLUME_LEVEL,        /**< 通知音量值 */
    UX_API_COMMU_CMD_NOTIFY_SAMPLE_RATE,         /**< 通知录音采样率 */
    UX_API_COMMU_CMD_NOTIFY_BIT_WIDTH,           /**< 通知录音比特位宽 */
    UX_API_COMMU_CMD_NOTIFY_FILE_SPLIT_DURATION, /**< 通知录音文件分割时长 */
    UX_API_COMMU_CMD_NOTIFY_FILE_SPLIT_SIZE,     /**< 通知录音文件分割大小 */
    UX_API_COMMU_CMD_NOTIFY_SDK_VERSION,         /**< 通知录音SDK版本号 */
    UX_API_COMMU_CMD_NOTIFY_RECOVERY_RESULT,     /**< 通知恢厂完成 */
    UX_API_COMMU_CMD_NOTIFY_FILE_FORMAT,         /**< 通知设置文件格式化 */
    UX_API_COMMU_CMD_CONNECT_HANDSHAKE_REQUEST,  /**< 连接成功握手请求 */
    UX_API_COMMU_CMD_CONNECT_HANDSHAKE_RESPONSE, /**< 连接成功握手响应 */
    UX_API_COMMU_CMD_TRANSMIT_MODE_REQUEST,      /**< transmit mode切换请求 */
    UX_API_COMMU_CMD_NOTIFY_NR_MODE,             /**< 通知降噪模式切换 */
    UX_API_COMMU_CMD_NOTIFY_VOLUME,              /**< 通知音量值变化 */
    UX_API_COMMU_CMD_NOTIFY_MUTE,                /**< 通知静音变化 */
    UX_API_COMMU_CMD_NOTIFY_RECORDER_ALG_BYPASS, /**< 通知录音音效算法旁路状态 */

    UX_API_COMMU_CMD_NOTIFY_END,
    UX_API_COMMU_CMD_SYNC_MSG_TAG,

    UX_API_COMMU_CMD_NOTIFY_PEER_START,          /**< 对耳通知指令起始 */
    UX_API_COMMU_CMD_NOTIFY_SEND_MSG_TEST,       /**< 私有通路测试 */
    UX_API_COMMU_CMD_NOTIFY_PEER_EVENT,          /**< 对耳事件通知事件 */
    UX_API_COMMU_CMD_NOTIFY_PEER_STATE,          /**< 通知对耳状态变化 */
    UX_API_COMMU_CMD_NOTIFY_SYNC_PEER_INFO,      /**< 同步双耳状态信息 */
    UX_API_COMMU_CMD_NOTIFY_PEER_END,            /**< 对耳通知指令结束 */

    UX_API_COMMU_CMD_NOTIFY_DONGLE_START,        /**< DONGLE通知指令起始 */
    UX_API_COMMU_CMD_NOTIFY_DONGLE_CTRL,         /**< DONGLE控制指令 */
    UX_API_COMMU_CMD_NOTIFY_DONGLE_UAC_CTRL,     /**< DONGLE控制UAC流指令 */
    UX_API_COMMU_CMD_NOTIFY_DONGLE_AUDIO_CTRL,   /**< DONGLE处理音乐控制指令 */
    UX_API_COMMU_CMD_NOTIFY_DONGLE_ROLE,         /**< DONGLE角色 */
    UX_API_COMMU_CMD_NOTIFY_DONGLE_END,          /**< DONGLE通知指令结束 */
    UX_API_COMMU_CMD_CONTROL_MAX,
} ux_api_commu_cmd_t;

/**
 * @brief 数传通道通信协议结构体定义
 */
typedef struct {
    ux_api_commu_cmd_t cmd;           /**< 命令类型 */
    uint16_t src_dev_id;              /**< 源设备ID */
    uint16_t dst_dev_id;              /**< 目的设备ID */
    uint8_t param[COMMU_MSG_LEN_MAX]; /**< 命令参数 */
    uint16_t param_len;               /**< 命令参数长度 */
    bool immediate_exec;              /**< 是否立即执行 */
    errcode_t ack_code;               /**< 响应错误码 */
} ux_api_commu_control_msg_t;

/**
 * @brief 通信消息处理函数指针类型定义
 * @param [in] device_id 源设备ID
 * @param [in] data 接收到的数据
 * @param [in] data_len 接收到的数据长度
 * @return 错误代码
 */
typedef errcode_t (*ux_api_commu_msg_handler)(uint16_t src_dev_id, uint8_t *data, uint16_t data_len);

/**
 * @brief 注册通信消息处理函数
 * @param [in] cmd_type 命令类型
 * @param [in] handler 消息处理函数
 * @return 错误代码
 */
errcode_t ux_api_commu_register_msg_handler(ux_api_commu_cmd_t cmd_type, ux_api_commu_msg_handler handler);

/**
 * @brief 向设备发送消息
 * @param [in] device_id 目的设备ID
 * @param [in] data 要发送的数据
 * @param [in] data_len 要发送的数据长度
 * @return 错误代码
 */
errcode_t ux_api_commu_send_msg_to_device(uint16_t device_id, uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* DIAG_SERVICE_H */
