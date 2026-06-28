/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: Provide ux logic manager \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-01-30， Create file. \n
 */

#ifndef UX_LOGIC_MANAGER_H
#define UX_LOGIC_MANAGER_H

#ifdef MELODY_DEMO_BMIC_RX
#include <stdint.h>
#include "errcode.h"
#include "key_struct.h"
#include "feature_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef UT_TEST
#define DEVICE_ID_MAX 4
#define LOGIC_ID_MAX 2
#else
#define DEVICE_ID_MAX CONFIG_MAX_PAIR_DEVICE_NUM
#define LOGIC_ID_MAX 8
#endif

/**
 * @brief 逻辑ID与设备ID的映射结构。
 */
typedef struct {
    uint16_t logic_id;  /**< 分配给设备的逻辑ID */
    uint16_t device_id; /**< 物理设备ID */
    uint8_t paired;     /**< 配对状态：0-未配对，1-已配对 */
} ux_id_map_t;

/**
 * @brief ID映射管理器结构体。
 * @details 管理逻辑ID与设备ID之间的双向映射，并维护已配对设备的数量。
 */
typedef struct {
    ux_id_map_t logic_maps[LOGIC_ID_MAX];   /**< 逻辑ID到设备ID的映射数组 */
    ux_id_map_t device_maps[DEVICE_ID_MAX]; /**< 设备ID到逻辑ID的映射数组 */
    uint16_t pair_count;                    /**< 当前已配对的设备数量 */
} ux_id_map_mgr_t;

/**
 * @brief 初始化UX逻辑管理器。
 * @details 设置ID映射表和资源的初始状态。
 * @return errcode_t 错误码，表示成功或失败类型。
 * @retval ERRCODE_OK 初始化成功。
 * @retval ERRCODE_FAIL 初始化失败。
 */
errcode_t ux_logic_mgr_init(void);

/**
 * @brief 去初始化UX逻辑管理器。
 * @details 释放资源并重置内部状态。
 */
void ux_logic_mgr_deinit(void);

/**
 * @brief 设置最大并发连接数。
 * @details 根据配置更新连接限制。
 */
void ux_logic_mgr_update_max_conn_num(void);

/**
 * @brief 配对指定设备ID的设备。
 * @param[in] device_id 待配对的物理设备ID。@ref uint16_t
 * @return errcode_t 错误码，表示成功或失败类型。
 * @retval ERRCODE_OK 配对成功。
 * @retval ERRCODE_FAIL 配对失败。
 */
errcode_t ux_logic_mgr_pair_device(uint16_t device_id);

/**
 * @brief 解除配对指定设备ID的设备。
 * @param[in] device_id 待解除配对的物理设备ID。@ref uint16_t
 * @return errcode_t 错误码，表示成功或失败类型。
 * @retval ERRCODE_OK 解除配对成功。
 * @retval ERRCODE_FAIL 解除配对失败。
 */
errcode_t ux_logic_mgr_unpair_device(uint16_t device_id);

/**
 * @brief 连接指定设备ID的设备。
 * @param[in] device_id 待连接的物理设备ID。@ref uint16_t
 * @return errcode_t 错误码，表示成功或失败类型。
 * @retval ERRCODE_OK 连接成功。
 * @retval ERRCODE_FAIL 连接失败。
 */
errcode_t ux_logic_mgr_connect_device(uint16_t device_id);

/**
 * @brief 断开指定设备ID的设备连接。
 * @param[in] device_id 待断开连接的物理设备ID。@ref uint16_t
 * @return errcode_t 错误码，表示成功或失败类型。
 * @retval ERRCODE_OK 断开连接成功。
 * @retval ERRCODE_FAIL 断开连接失败。
 */
errcode_t ux_logic_mgr_disconnect_device(uint16_t device_id);

/**
 * @brief 重置UX逻辑管理器到初始状态。
 * @details 清除所有配对和连接信息。
 */
void ux_logic_mgr_reset(void);

/**
 * @brief 从非易失性存储恢复UX逻辑管理器信息。
 * @return errcode_t 错误码，表示成功或失败类型。
 * @retval ERRCODE_OK 恢复信息成功。
 * @retval ERRCODE_FAIL 恢复信息失败。
 */
errcode_t ux_logic_mgr_info_restore(void);

/**
 * @brief 获取当前的ID映射结构。
 * @return ux_id_map_mgr_t 当前的ID映射管理器结构。
 */
ux_id_map_mgr_t ux_logic_mgr_get_id_map(void);

/**
 * @brief 根据逻辑ID获取设备ID。
 * @param[in] logic_id 待查询的逻辑ID。@ref uint16_t
 * @param[out] device_id 用于存储设备ID的指针。@ref uint16_t
 * @return errcode_t 错误码，表示成功或失败类型。
 * @retval ERRCODE_OK 获取设备ID成功。
 * @retval ERRCODE_FAIL 获取设备ID失败。
 */
errcode_t ux_logic_mgr_get_device_id(uint16_t logic_id, uint16_t *device_id);

/**
 * @brief 根据设备ID获取逻辑ID。
 * @param[in] device_id 待查询的设备ID。@ref uint16_t
 * @param[out] logic_id 用于存储逻辑ID的指针。@ref uint16_t
 * @return errcode_t 错误码，表示成功或失败类型。
 * @retval ERRCODE_OK 获取逻辑ID成功。
 * @retval ERRCODE_FAIL 获取逻辑ID失败。
 */
errcode_t ux_logic_mgr_get_logic_id(uint16_t device_id, uint16_t *logic_id);

#ifdef UT_TEST
ux_nv_device_map_t *ux_logic_mgr_get_nv_device_map(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#endif
