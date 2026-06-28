/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: 提供 UX API 广播功能 \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-03-05，创建文件。 \n
 */
#ifndef UX_API_BROADCAST_H
#define UX_API_BROADCAST_H

#include <stdbool.h>
#include "errcode.h"
#include "ux_api_audio_common.h"
#include "ux_api_bs_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UX_API_BROADCAST_WAIT_FOREVER 0xFFFFFFFF   /**< 广播等待永远 */

/**
 * @brief   广播事件类型枚举
 * @details 事件分类：\n
 *          - 电源类事件\n
 *          - 连接管理类事件\n
 *          - 音频管理类事件\n
 *          - 其他事件
 */
typedef enum {
    /* 电源类事件 */
    UX_API_BROADCAST_EVENT_ACC_ON,          /**< ACC ON事件 */
    UX_API_BROADCAST_EVENT_POWER_ON,        /**< 开机事件 */
    UX_API_BROADCAST_EVENT_POWER_OFF,       /**< 关机事件 */
    UX_API_BROADCAST_EVENT_ACC_OFF,         /**< 下电事件 */
    UX_API_BROADCAST_EVENT_STANDBY,         /**< 待机事件 */

    /* 连接管理类事件 */
    UX_API_BROADCAST_EVENT_ENTER_PAIRING,   /**< 进入配对模式事件 */
    UX_API_BROADCAST_EVENT_EXIT_PAIRING,    /**< 退出配对模式事件 */
    UX_API_BROADCAST_EVENT_PAIR_TIMEOUT,    /**< 配对超时事件 */
    UX_API_BROADCAST_EVENT_PAIRED,          /**< 配对成功事件，广播数据结构：ux_api_broadcast_bs_data_t */
    UX_API_BROADCAST_EVENT_CONNECTED,       /**< 连接成功事件，广播数据结构：ux_api_broadcast_bs_data_t */
    UX_API_BROADCAST_EVENT_DISCONNECTED,    /**< 断连事件，广播数据结构：ux_api_broadcast_bs_data_t */

    /* 音频管理类事件 */
    UX_API_BROADCAST_EVENT_STREAM_START,    /**< 音频流开始事件，广播数据结构：ux_api_broadcast_audio_stream_data_t */
    UX_API_BROADCAST_EVENT_STREAM_STOP,     /**< 音频流停止事件，广播数据结构：ux_api_broadcast_audio_stream_data_t */
    
    /* 其他事件 */
    UX_API_BROADCAST_EVENT_CUSTOMER,        /**< 自定义事件 */
    UX_API_BROADCAST_EVENT_TYPE_MAX
} ux_api_broadcast_event_type_t;

/**
 * @brief   连接管理类事件广播数据结构
 * @details 对应广播事件：\n
 *          - UX_API_BROADCAST_EVENT_PAIRED\n
 *          - UX_API_BROADCAST_EVENT_CONNECTED\n
 *          - UX_API_BROADCAST_EVENT_DISCONNECTED
 */
typedef struct {
    ux_api_bs_protocol_t protocol;    /**< 工作协议，适用产品：通用 */
    ux_api_bs_addr_t addr;            /**< 地址，适用产品：通用 */
    uint16_t device_id;               /**< 设备id号，适用产品：直播麦 */
    uint16_t logic_id;                /**< 逻辑id号，适用产品：直播麦 */
} ux_api_broadcast_bs_data_t;

/**
 * @brief   音频流开始/停止事件广播数据结构
 * @details 对应广播事件：\n
 *          - UX_API_BROADCAST_EVENT_STREAM_START\n
 *          - UX_API_BROADCAST_EVENT_STREAM_STOP
 */
typedef struct {
    ux_api_audio_stream_type_t type;            /**< 音频流类型，适用产品：通用 */
    ux_api_audio_stream_direction_t direction;  /**< 音频流方向，适用产品：通用 */
    uint16_t device_id;                         /**< 设备id号，适用产品：直播麦 */
} ux_api_broadcast_audio_stream_data_t;

/**
 * @brief 广播消息结构体
 */
typedef struct {
    ux_api_broadcast_event_type_t type; /**< 事件类型 */
    void* data;                         /**< 消息数据 */
    uint32_t data_len;                  /**< 数据长度 */
    bool need_free_data;                /**< 是否需要释放数据 */
} ux_api_broadcast_msg_t;

/**
 * @brief 订阅事件结构体
 */
typedef struct {
    ux_api_broadcast_event_type_t type; /**< 事件类型 */
    bool is_sync;                       /**< 是否同步 */
} ux_api_broadcast_subscribe_event_t;

/**
 * @brief 订阅事件列表结构体
 */
typedef struct {
    ux_api_broadcast_subscribe_event_t subscribe[UX_API_BROADCAST_EVENT_TYPE_MAX]; /**< 订阅事件数组 */
    uint32_t count; /**< 事件数量 */
} ux_api_broadcast_subscribe_list_t;

/**
 * @brief 事件处理器函数指针类型
 */
typedef errcode_t (*ux_api_broadcast_event_handler_t)(ux_api_broadcast_msg_t* msg);

/**
 * @brief 初始化广播模块
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_api_broadcast_init(void);

/**
 * @brief 反初始化广播模块
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_api_broadcast_deinit(void);

/**
 * @brief 注册消费者
 * @param [in] sync_event_handler 同步事件处理器
 * @param [in] subscribe_list 订阅事件列表
 * @param [out] handle 返回的句柄
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_api_broadcast_register_consumer(ux_api_broadcast_event_handler_t sync_event_handler,
    ux_api_broadcast_subscribe_list_t* subscribe_list, uint32_t *handle);

/**
 * @brief 注销消费者
 * @param [in] handle 消费者句柄
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_api_broadcast_unregister_consumer(uint32_t handle);

/**
 * @brief 查询消息
 * @param [in] handle 消费者句柄
 * @param [out] msg 返回的消息
 * @param [in] timeout_ms 超时时间（单位：毫秒），若在指定时间内无消息则返回失败
 * @note 当msg->need_free_data为true时，调用者必须调用ux_api_broadcast_memory_release释放msg->data
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_api_broadcast_query_msg(uint32_t handle, ux_api_broadcast_msg_t* msg, uint32_t timeout_ms);

/**
 * @brief 发送客户消息
 * @param [in] handle 消费者句柄
 * @param [in] msg 消息内容
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref ux_errcode.h
 */
errcode_t ux_api_broadcast_push_customer_msg(uint32_t handle, ux_api_broadcast_msg_t* msg);

/**
 * @brief 释放内存接口
 * @param [in] memory 释放内存地址
 */
void ux_api_broadcast_memory_release(void *memory);
#ifdef __cplusplus
}
#endif

#endif // UX_API_BROADCAST_H