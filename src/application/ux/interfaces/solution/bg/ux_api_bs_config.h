/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux bs configuration-related functions, such as addresses, names, etc. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_API_BS_CONFIG_H
#define UX_API_BS_CONFIG_H

#include "stdint.h"
#include "errcode.h"
#include "ux_common_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief BS地址类型枚举
 */
typedef enum {
    UX_API_BS_ADDR_TYPE_NORMAL = 0, /**< 普通地址 */
    UX_API_BS_ADDR_TYPE_PUBLIC = UX_API_BS_ADDR_TYPE_NORMAL, /**< 公共地址 */
    UX_API_BS_ADDR_TYPE_RANDOM = 1, /**< 随机地址，仅ble可选 */
    UX_API_BS_ADDR_TYPE_MAX
} ux_api_bs_addr_type_t;

/**
 * @brief BS地址结构体
 */
typedef struct {
    uint8_t addr[UX_BG_ADDR_LEN]; /**< 地址数据 */
    ux_api_bs_addr_type_t type; /**< 地址类型，默认为UX_API_BS_ADDR_TYPE_NORMAL */
} ux_api_bs_addr_t;

/**
 * @brief BS协议类型枚举
 */
typedef enum {
    UX_API_BS_PROTOCOL_BT = 0,    /**< 支持经典蓝牙与低功耗蓝牙 */
    UX_API_BS_PROTOCOL_SLE = 1,   /**< 支持星闪低功耗模式 */
    UX_API_BS_PROTOCOL_COMBO = 2, /**< 同时支持经典蓝牙、低功耗蓝牙、与星闪低功耗三种模式 */
    UX_API_BS_PROTOCOL_BR = 3,    /**< 支持经典蓝牙模式 */
    UX_API_BS_PROTOCOL_BLE = 4,   /**< 支持低功耗蓝牙 */
    UX_API_BS_PROTOCOL_MAX
} ux_api_bs_protocol_t;

/**
 * @brief 设置设备本机地址
 * @param [in] protocol 协议类型
 * @param [in] addr 地址信息字符串指针, 小端序
 * @param [in] addr_len 地址信息长度, 12字节
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_set_addr(ux_api_bs_protocol_t protocol, const char *addr, uint16_t addr_len);

/**
 * @brief 获取设备本机地址
 * @param [in] protocol 协议类型
 * @param [in] addr 地址信息指针, 小端序
 * @param [in] addr_len 地址信息长度, 6字节
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_addr(ux_api_bs_protocol_t protocol, uint8_t *addr, uint16_t addr_len);

/**
 * @brief 设置对端地址
 * @defgroup 支持产品 [tws, haid]
 * @param [in] protocol 协议类型
 * @param [in] addr 地址信息字符串指针, 小端序
 * @param [in] addr_len 地址信息长度, 12字节
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_set_peer_addr(ux_api_bs_protocol_t protocol, const char *addr, uint16_t addr_len);

/**
 * @brief 获取对端地址
 * @defgroup 支持产品 [tws, haid]
 * @param [in] protocol 协议类型
 * @param [in] addr 地址信息指针, 小端序
 * @param [in] addr_len 地址信息长度, 6字节
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_peer_addr(ux_api_bs_protocol_t protocol, uint8_t *addr, uint16_t addr_len);

/**
 * @brief 设置设备名称
 * @param [in] protocol 协议类型
 * @param [in] name 设备名称指针
 * @param [in] len 设备名称长度, 最大64字节
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_set_name(ux_api_bs_protocol_t protocol, char *name, uint16_t len);

/**
 * @brief 获取设备名称
 * @param [in] protocol 协议类型
 * @param [out] name 设备名称指针，建议获取设备名称时分配的内存长度大于64字节
 * @param [in] len 设备名称长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_name(ux_api_bs_protocol_t protocol, char *name, uint16_t len);

/**
 * @brief 获取当前协议类型
 * @return ux_api_bs_protocol_t 协议类型 {@ref ux_api_bs_protocol_t}
 */
ux_api_bs_protocol_t ux_api_bs_get_protocol(void);

/**
 * @brief 设置协议类型
 * @param [in] protocol 协议类型 {@ref ux_api_bs_protocol_t}
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_set_protocol(ux_api_bs_protocol_t protocol);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
