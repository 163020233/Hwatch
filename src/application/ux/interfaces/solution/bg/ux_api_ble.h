/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux bs api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_API_BLE_H
#define UX_API_BLE_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "ux_api_bs_config.h"
#include "ux_api_bs_connection.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_API_BLE_EXPAND_DATA_LEN              28     /* 受ble广播长度限制，设备ble扩展最长28个字节 */

/**
 * @brief BLE配置类型枚举
 */
typedef enum {
    UX_API_BLE_CONFIG_ADV_DATA,             /**< 设置BLE直接广播数据 */
    UX_API_BLE_CONFIG_SCAN_RSP_ADV_DATA,    /**< 设置BLE扫描响应广播数据 */
    UX_API_BLE_CONFIG_TYPE_MAX,             /**< 最大值 */
} ux_api_ble_config_type_t;

/**
 * @brief BLE扩展数据结构体
 */
typedef struct {
    uint16_t data_len; /**< 数据长度 */
    uint8_t data[UX_API_BLE_EXPAND_DATA_LEN]; /**< 扩展数据内容 */
} ux_api_ble_expand_data_t;

/**
 * @brief BLE消息处理函数类型
 */
typedef errcode_t (*ux_api_ble_msg_handler)(uint8_t *data, uint16_t data_len);

/**
 * @brief 通过ble服务发送数据
 * @param [in] dev_id 用于指定数据接收方的设备
 * @param [in] data 用于存储发送数据的缓冲区
 * @param [in] data_len 缓冲区长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_service_send_data(uint16_t dev_id, uint8_t *data, uint16_t data_len);

/**
 * @brief 注册处理接收到的ble数据
 * @param [in] dev_id 用于指定数据发送方的设备
 * @param [in] handler 用于处理数据的注册函数
 */
void ux_api_ble_service_register_msg_handler(uint16_t dev_id, ux_api_ble_msg_handler handler);

/**
 * @brief 设置ble广播配置信息
 * @param [in] type  配置信息类型 {@ref ux_api_ble_config_type_t}
 * @param [in] data  用于存储配置信息数据的缓冲区
 * @param [in] data_len   缓冲区长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_set_config(ux_api_ble_config_type_t type, void *data, uint16_t data_len);

/**
 * @brief 设置BLE扩展数据
 * @param [in] expand_data 扩展数据指针 {@ref ux_api_ble_expand_data_t}
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_set_expand_data(ux_api_ble_expand_data_t *expand_data);

/**
 * @brief 获取BLE扩展数据
 * @param [out] expand_data 扩展数据指针 {@ref ux_api_ble_expand_data_t}
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_get_expand_data(ux_api_ble_expand_data_t *expand_data);

/**
 * @brief 断开BLE连接
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_disconnect(void);

/**
 * @brief 设置BLE广播数据
 * @param [in] need_add_ble_end_val 是否添加BLE结束标志
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_set_adv_data(bool need_add_ble_end_val);

/**
 * @brief 启动BLE发现模式
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_start_discovery(void);

/**
 * @brief 停止BLE发现模式
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_stop_discovery(void);

/**
 * @defgroup 支持产品 [watch]
 * @brief 连接BLE设备
 * @param [in] addr 待配对连接设备的地址
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_ble_connect(const ux_api_bs_addr_t *addr);

/**
 * @brief 获取BLE配对状态
 * @return 配对状态 {@ref ux_api_bs_pair_state_t}
 */
ux_api_bs_pair_state_t ux_api_ble_get_pairing_state(void);

/**
 * @brief 获取BLE连接状态
 * @return 连接状态 {@ref ux_api_bs_conn_state_t}
 */
ux_api_bs_conn_state_t ux_api_ble_get_connect_state(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif