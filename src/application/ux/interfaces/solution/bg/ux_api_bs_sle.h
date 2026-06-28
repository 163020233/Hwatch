/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux bs sle-related functions, such as addresses, names, etc. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_API_BS_SLE_H
#define UX_API_BS_SLE_H

#include "stdint.h"
#include "errcode.h"
#include "ux_common_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  Length of sle UUID.
 * @else
 * @brief  星闪UUID长度。
 * @endif
 */
#define UX_API_BS_SLE_UUID_LEN 16

/**
 * @brief SLE UUID结构体
 * @details 用于存储UUID的长度和UUID数据
 */
typedef struct {
    uint8_t len;                            /*!< @if Eng uuid len
                                                @else   UUID 长度 @endif */
    uint8_t uuid[UX_API_BS_SLE_UUID_LEN];   /*!< @if Eng uuid
                                                @else   UUID字段 @endif */
} ux_api_bs_sle_uuid_t;

/**
 * @brief SLE端口数据接收回调函数类型
 * @param [in] device_id 设备ID
 * @param [in] data 接收到的数据指针
 * @param [in] data_len 接收到的数据长度
 */
typedef void (*ux_api_bs_sle_port_data_receive_handler)(uint16_t device_id, uint8_t *data, uint32_t data_len);

/**
 * @brief 设置SLE端口UUID
 * @param [in] uuid UUID结构体指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_set_sle_port_uuid(ux_api_bs_sle_uuid_t *uuid);

/**
 * @brief 连接SLE端口
 * @param [in] device_id 设备ID
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_sle_port_connect(uint16_t device_id);

/**
 * @brief 断开SLE端口连接
 * @param [in] device_id 设备ID
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_sle_port_disconnect(uint16_t device_id);

/**
 * @brief 注册SLE端口数据接收回调函数
 * @param [in] handler 回调函数指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_sle_port_register_receive_handler(ux_api_bs_sle_port_data_receive_handler handler);

/**
 * @brief 向SLE端口发送数据
 * @param [in] device_id 设备ID
 * @param [in] data 待发送的数据指针
 * @param [in] data_len 待发送的数据长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_sle_port_send_data(uint16_t device_id, uint8_t *data, uint16_t data_len);
 
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
