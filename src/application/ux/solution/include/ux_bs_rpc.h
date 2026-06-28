/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: provide rpc ability for livemic.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-03-23, Create file. \n
 */
#ifndef UX_BS_RPC_H
#define UX_BS_RPC_H

#include "stddef.h"
#include "stdbool.h"
#include "errcode.h"
#include "ux_api_communication.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 星闪握手参数结构体
 *
 * 该结构体封装了G-node和T-node之间共享的所有握手参数，
 * 用于发送和解析握手数据。
 *
 * @note 请求时:  src = G-node, dest = T-node
 * @note 响应时:  src = T-node, dest = G-node
 */
typedef struct {
    uint8_t src_addr[UX_BG_ADDR_LEN]; /**< 源节点地址（6字节） */
    uint16_t src_device_id;           /**< 源设备ID */
    uint16_t dest_device_id;          /**< 目标设备ID */
    uint8_t src_transmit_mode;        /**< 源发送模式 */
    uint8_t reserve[8];               /**< 保留字段 */
} ux_bs_rpc_param_t;

/**
 * @brief 公有函数：发送握手数据
 *
 * 将握手参数打包成控制消息并发送到目标设备。
 * G-node（发送请求）和T-node（发送响应）都使用此函数。
 *
 * @param cmd_type 发送指令类型
 *
 * @param param 握手参数结构体指针，包含：
 *        - src_addr: 源地址
 *        - src_device_id: 源设备ID
 *        - dest_device_id: 目标设备ID
 *        - src_transmit_mode: 源发送模式
 *        - reserve: 保留字段（应置零）
 *
 * @return 成功返回UX_ERR_SUCCESS，失败返回错误码：
 *         - UX_ERR_INVALID_PARAMETER: 无效参数
 *         - UX_ERR_MEMCPY_FAILED: 内存拷贝失败
 *         - 其他来自ux_api_commu_send_msg_to_device的错误
 *
 * @note 消息格式（19字节）：
 *       src_addr(6) + src_device_id(2) + dest_device_id(2) + transmit_mode(1) + reserve(8)
 */
errcode_t ux_bs_rpc_send_data(ux_api_commu_cmd_t cmd_type, ux_bs_rpc_param_t *param);

/**
 * @brief 公有函数：解析握手数据
 *
 * 解析接收到的握手数据缓冲区的内容，提取握手参数。
 * G-node（解析响应）和T-node（解析请求）都使用此函数。
 *
 * @param data 接收到的数据缓冲区指针
 * @param data_len 数据缓冲区长度（至少19字节）
 * @param parsed_param 接收解析参数的参数结构体指针
 *
 * @return 成功返回UX_ERR_SUCCESS，失败返回错误码：
 *         - UX_ERR_INVALID_PARAMETER: 无效参数或数据过短
 *         - UX_ERR_MEMCPY_FAILED: 内存拷贝失败
 *
 * @note 期望的数据格式（19字节）：
 *       src_addr(6) + src_device_id(2) + dest_device_id(2) + transmit_mode(1) + reserve(8)
 */
errcode_t ux_bs_rpc_parse_data(uint8_t *data, uint16_t data_len, ux_bs_rpc_param_t *parsed_param);

/**
 * @brief RX（G-node）：发送握手请求
 *
 * RX（G-node/直播麦RX）调用此函数向TX发起握手。
 * 将本地设备信息打包并发送到目标TX。
 *
 * @param addr 本地地址指针（应为UX_BG_ADDR_LEN字节）
 * @param addr_len 地址长度（应为UX_BG_ADDR_LEN）
 * @param src_device_id 本地设备ID（RX设备ID）
 * @param dest_device_id 目标设备ID（TX设备ID）
 *
 * @return 成功返回UX_ERR_SUCCESS，失败返回错误码
 */
errcode_t ux_bs_rpc_livemic_send_handshake_request(uint8_t *addr, uint32_t addr_len, uint16_t src_device_id,
                                                   uint16_t dest_device_id);

/**
 * @brief 发送transmit mode变更请求
 *
 * 向所有已连接的远程设备广播发送模式变更请求。
 *
 * @param transmit_mode 目标发送模式
 *
 * @return 成功返回UX_ERR_SUCCESS，失败返回错误码
 */
errcode_t ux_bs_rpc_send_transmit_mode_change_request(ux_api_sys_transmit_mode_t transmit_mode);

/**
 * @brief 处理handshake请求
 *
 * @param device_id 设备ID
 * @param data 请求数据缓冲区
 * @param data_len 数据长度
 *
 * @return 成功返回UX_ERR_SUCCESS，失败返回错误码
 */
errcode_t ux_bs_rpc_handshake_request_handler(uint16_t device_id, uint8_t *data, uint16_t data_len);

/**
 * @brief 处理handshake响应
 *
 * @param device_id 设备ID
 * @param data 响应数据缓冲区
 * @param data_len 数据长度
 *
 * @return 成功返回UX_ERR_SUCCESS，失败返回错误码
 */
errcode_t ux_bs_rpc_handshake_response_handler(uint16_t device_id, uint8_t *data, uint16_t data_len);

/**
 * @brief 处理transmit mode变更请求
 *
 * @param device_id 设备ID
 * @param data 请求数据缓冲区
 * @param data_len 数据长度
 *
 * @return 成功返回UX_ERR_SUCCESS，失败返回错误码
 */
errcode_t ux_bs_rpc_transmit_mode_change_handler(uint16_t device_id, uint8_t *data, uint16_t data_len);

/**
 * @brief RPC模块初始化
 */
void ux_bs_rpc_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UX_BS_RPC_H */