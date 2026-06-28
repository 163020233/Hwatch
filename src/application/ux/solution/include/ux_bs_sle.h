/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_sle.h
 * Description:  provide ux bg sle header file.
 */
#ifndef UX_BS_SLE_H
#define UX_BS_SLE_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "key_struct.h"
#include "ux_api_system_config.h"
#include "ux_bg_config.h"
#include "ux_bs_sle_wrap.h"
#include "ux_common_define.h"
#include "ux_service_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 星闪协议栈操作者类型枚举
 * 表示触发星闪协议栈状态变化的操作来源
 */
typedef enum {
    UX_BS_SLE_OPERATOR_POWER,                /**< 星闪协议栈操作者：电源 */
    UX_BS_SLE_OPERATOR_TRANSMIT_MODE_CHANGE, /**< 星闪协议栈操作者：工作模式切换 */
    UX_BS_SLE_OPERATOR_RECOVERY,             /**< 星闪协议栈操作者：恢复出厂设置 */
    UX_BS_SLE_OPERATOR_OTHERS,               /**< 星闪协议栈操作者：其他 */

    UX_BS_SLE_OPERATOR_MAX,                  /**< 枚举值上限 */
} ux_bs_sle_operator_t;

/**
 * @brief 星闪协议栈通用状态枚举
 * 表示星闪协议栈或业务的使能状态
 */
typedef enum {
    UX_BS_SLE_COMMON_STATUS_INIT,   /**< 已初始化/已开启状态 */
    UX_BS_SLE_COMMON_STATUS_DEINIT, /**< 已去初始化/已关闭状态 */

    UX_BS_SLE_COMMON_STATUS_MAX,    /**< 枚举值上限 */
} ux_bs_sle_common_status_t;

/**
 * @brief 星闪音频确认类型枚举
 * 表示音频流开启或关闭的确认类型
 */
typedef enum {
    UX_SLE_CONFIRM_STREAM_OPEN,  /**< 开启音频流确认 */
    UX_SLE_CONFIRM_STREAM_CLOSE, /**< 关闭音频流确认 */

    UX_SLE_CONFIRM_STREAM_MAX,   /**< 枚举值上限 */
} ux_sle_confirm_audio_type_t;

/**
 * @brief 星闪流请求类型枚举
 * 表示音频流的创建或销毁请求类型
 */
typedef enum {
    UX_SLE_STREAM_REQUEST_TYPE_CREATE,  /**< 创建音频流 */
    UX_SLE_STREAM_REQUEST_TYPE_DESTROY, /**< 销毁音频流 */

    UX_SLE_STREAM_REQUEST_TYPE_MAX,     /**< 枚举值上限 */
} ux_sle_stream_request_type_t;

typedef struct {
    uint8_t addr[UX_BG_ADDR_LEN];
    uint16_t device_id;
    uint16_t conn_id;
    ux_sle_stream_request_type_t request;
} ux_sle_stream_request_param_t;

/**
 * 用于上报星闪协议栈的使能状态的回调函数
 * @param status 星闪协议栈状态：开启/关闭
 * @param sle_operator 星闪协议栈操作者：电源/工作模式切换
 * @return 错误码，0表示成功，其他值表示失败
 */
typedef errcode_t (*ux_bs_sle_service_state_change_callback)(ux_bs_sle_common_status_t status,
                                                             ux_bs_sle_operator_t sle_operator);

/**
 * 用于上报直播麦星闪业务使能状态的回调函数，开启时直播麦可进行配对/连接/音频业务，关闭时所有业务关闭
 * @param status 星闪直播麦业务状态：开启/关闭
 * @param sle_operator 星闪直播麦业务操作者：电源/工作模式切换
 * @return 错误码，0表示成功，其他值表示失败
 */
typedef errcode_t (*ux_bs_sle_livemic_service_state_change_callback)(ux_bs_sle_common_status_t status,
                                                                     ux_bs_sle_operator_t sle_operator);

typedef struct {
    /* 用于上报星闪协议栈的使能状态的回调函数 */
    ux_bs_sle_service_state_change_callback bs_sle_service_state_change_cbk;
    /* 用于上报直播麦星闪业务开启/关闭的回调函数 */
    ux_bs_sle_livemic_service_state_change_callback bs_sle_livemic_service_state_change_cbk;
} ux_bs_sle_callback_t;

/**
 * 星闪协议栈初始化
 * @param transmit_mode 星闪协议栈工作模式：2T1R、8T1R
 * @param sle_operator 星闪协议栈操作者：电源/工作模式切换
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_bs_sle_service_init(ux_api_sys_transmit_mode_t transmit_mode, ux_bs_sle_operator_t sle_operator);

/**
 * 星闪协议栈去初始化
 * @param sle_operator 星闪协议栈操作者：电源/工作模式切换
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_bs_sle_service_deinit(ux_bs_sle_operator_t sle_operator);

/**
 * 注册星闪回调函数
 * @param func 回调函数结构体指针，包含协议栈状态回调和直播麦业务状态回调
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_register_callback(ux_bs_sle_callback_t *func);

/**
 * 启动星闪配对模式
 * @param method 配对方式：白名单过滤方式或广播过滤方式
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_start_pairing(ux_api_bs_pairing_mode_t method);

/**
 * 停止星闪配对模式
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_stop_pairing(void);

/**
 * 启动星闪连接模式
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_start_connect(void);

/**
 * 停止星闪连接模式
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_stop_connect(void);

/**
 * 启动星闪直播麦业务
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_bs_sle_start_livemic_service(ux_bs_sle_operator_t sle_operator);

/**
 * 关闭星闪直播麦业务
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_bs_sle_stop_livemic_service(ux_bs_sle_operator_t sle_operator);

/**
 * 通过星闪数传通道发送数据
 * @param conn_id 连接ID
 * @param data 发送的数据缓冲区指针
 * @param data_len 数据长度
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_send_data(uint16_t conn_id, uint8_t *data, uint16_t data_len);

/**
 * 启动星闪音频模式
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_start_audio_mode(void);

/**
 * 停止星闪音频模式
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_stop_audio_mode(void);

/**
 * 确认星闪音频通路开启或关闭
 * @param addr 设备地址
 * @param addr_len 地址长度
 * @param conn_id 连接ID
 * @param type 音频类型：开启流或关闭流
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_confirm_audio_path(uint8_t *addr, uint16_t addr_len, uint16_t conn_id,
                                    ux_sle_confirm_audio_type_t type);

/**
 * 获取星闪连接RSSI值
 * @param conn_id 连接ID
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_sle_get_rssi(uint16_t conn_id);

/**
 * 获取直播麦自动码率
 * @return 自动码率值
 */
uint32_t ux_bs_sle_livemic_get_autorate_rate(void);

#ifdef CONFIG_SUPPORT_BS_SLE_L4
/**
 * 向指定地址断连
 * @param addr 设备地址
 * @param addr_len 地址长度
 * @return 错误码，0表示成功，其他值表示失败
 */
errcode_t ux_bs_sle_disconnect_by_addr(uint8_t *addr, uint16_t addr_len);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif