/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux bs api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_API_BS_H
#define UX_API_BS_H

#include "stdint.h"
#include "errcode.h"
#include "ux_common_define.h"
#include "ux_api_bs_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VENDOR_INFO_MAX_LEN 64                            /**< 厂商信息最大长度 */
#define UX_API_BS_MAX_WHITELIST_NUM 8                     /**< 白名单最大数量 */
#define UX_API_BS_SLE_CDS_KEY_LEN 16                      /**< 合作设备集密钥长度 */

/**
 * @brief 信道质量结构体
 */
typedef struct {
    ux_api_bs_addr_t addr;     /**< 设备地址 */
    uint32_t missed_rate;       /**< 丢包率 */
    uint32_t err_package_rate;  /**< 错包率 */
    int8_t rssi;                /**< RSSI */
} ux_api_link_quality_t;

/**
 * @brief 白名单节点结构体
 */
typedef struct {
    uint8_t addr[UX_BG_ADDR_LEN]; /**< 白名单节点地址 */
} ux_api_bs_whitelist_node_t;

/**
 * @brief 白名单结构体
 */
typedef struct {
    ux_api_bs_whitelist_node_t whitelist_arr[UX_API_BS_MAX_WHITELIST_NUM]; /**< 白名单数组 */
    uint8_t whitelist_num; /**< 白名单数量 */
} ux_api_bs_whitelist_t;

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 信道质量上报回调
 * @param [in] link_nums 连接数
 * @param [in] link_quality 信道质量数组，数组长度由link_nums指定，最大长度为8
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
typedef errcode_t (*ux_api_bs_sle_link_quality_callback)(uint8_t link_nums, ux_api_link_quality_t *link_quality);

/**
 * @brief 星闪回调函数结构体
 */
typedef struct {
    ux_api_bs_sle_link_quality_callback bs_sle_link_quality_cbk; /**< 信道质量回调函数 */
} ux_api_bs_sle_callback_t;

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 向UX星闪模块注册回调函数
 * @param [in] func 需要注册的回调函数的结构体的地址
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_sle_register_callback(ux_api_bs_sle_callback_t *func);

/**
 * @defgroup 支持产品 [livemic_rx] [tws]
 * @brief 查询当前生效的码率值
 * @return uint32_t rate 码率值
 */
uint32_t ux_api_bs_get_autorate_value(void);

/**
 * @defgroup 支持产品 [livemic_rx] [dongle]
 * @brief 设置白名单地址
 * @param [in] addr 星闪地址
 * @param [in] addr_len 星闪地址长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_set_whitelist_addr(const uint8_t *addr, uint16_t addr_len);

/**
 * @defgroup 支持产品 [livemic_rx] [dongle]
 * @brief 查询白名单地址
 * @param [in, out] whitelist 存储白名单地址的结构
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_whitelist_addr(ux_api_bs_whitelist_t *whitelist);

/**
 * @defgroup 支持产品 [livemic_rx] [dongle]
 * @brief 删除所有白名单地址
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_clear_all_whitelist(void);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx] [dongle] [soundbox] [soundbar]
 * @brief 设置厂商信息
 * @param [in, out] vendor_info 厂商信息
 * @param [in] vendor_info_len 厂商信息长度，取值范围(0, 64]
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_set_vendor_info(const uint8_t *vendor_info, uint8_t vendor_info_len);

/**
 * @brief 设置星闪合作设备集密钥信息 Cooperative Devices Set Key
 * @param [in] key_data   输入指针，指向包含密钥数据的缓冲区 (16 字节)
 * @param [in] key_len    输入缓冲区长度
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_config_sle_cds_key(const uint8_t *key_data, uint16_t key_len);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx] [dongle] [soundbox] [soundbar]
 * @brief 读取厂商信息
 * @param [in, out] vendor_info 厂商信息
 * @param [out] vendor_info_len 厂商信息长度，取值范围(0, 64]
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_get_vendor_info(uint8_t *vendor_info, uint8_t *vendor_info_len);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx]
 * @brief 清除所有配对信息
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_clear_pair_info(void);

/**
 * @defgroup 支持产品 [livemic_rx] [livemic_tx] [tws]
 * @brief 根据地址清除配对信息
 * @param [in] addr 星闪地址
 * @param [in] addr_len 星闪地址长度
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_remove_pair_info(const uint8_t *addr, uint16_t addr_len);

/**
 * @brief 获取设备ID
 * @param [in] protocol 指定的协议类型
 * @return uint32_t 设备的唯一标识符
 */
uint32_t ux_api_bs_get_device_id(ux_api_bs_protocol_t protocol);

/**
 * @brief 设置电池电量等级
 * @param [in] battery 电池电量等级值，范围通常为0-100
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_send_battery_level(uint8_t battery);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
